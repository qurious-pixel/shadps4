#include "common/io_file.h"
#include "emulator/file_format/pkg.h"
#include "emulator/file_format/pkg_type.h"

#include <zlib-ng.h>

static void DecompressPFSC(std::span<const char> compressed_data,
                           std::span<char> decompressed_data) {
    zng_stream decompressStream;
    decompressStream.zalloc = Z_NULL;
    decompressStream.zfree = Z_NULL;
    decompressStream.opaque = Z_NULL;

    if (zng_inflateInit(&decompressStream) != Z_OK) {
        // std::cerr << "Error initializing zlib for deflation." << std::endl;
    }

    decompressStream.avail_in = compressed_data.size();
    decompressStream.next_in = reinterpret_cast<const Bytef*>(compressed_data.data());
    decompressStream.avail_out = decompressed_data.size();
    decompressStream.next_out = reinterpret_cast<Bytef*>(decompressed_data.data());

    if (zng_inflate(&decompressStream, Z_FINISH)) {
    }
    if (zng_inflateEnd(&decompressStream) != Z_OK) {
        // std::cerr << "Error ending zlib inflate" << std::endl;
    }
}

u32 GetPFSCOffset(std::span<const u8> pfs_image) {
    static constexpr u32 PfscMagic = 0x43534650;
    u32 value;
    for (u32 i = 0x20000; i < pfs_image.size(); i += 0x10000) {
        std::memcpy(&value, &pfs_image[i], sizeof(u32));
        if (value == PfscMagic)
            return i;
    }
    return -1;
}

std::filesystem::path findDirectory(const std::filesystem::path& rootFolder,
                                    const std::filesystem::path& targetDirectory) {
    for (const auto& entry : std::filesystem::recursive_directory_iterator(rootFolder)) {
        if (std::filesystem::is_directory(entry) &&
            entry.path().filename() == targetDirectory.filename()) {
            return entry.path();
        }
    }
    return std::filesystem::path(); // Return an empty path if not found
}

PKG::PKG() = default;

PKG::~PKG() = default;

bool PKG::Open(const std::string& filepath) {
    IOFile file(filepath);
    if (!file.isOpen()) {
        return false;
    }
    pkgSize = file.size().value();

    PKGHeader pkgheader;
    file.readBytes(&pkgheader, sizeof(PKGHeader));

    // Find title id it is part of pkg_content_id starting at offset 0x40
    file.seek(0x47); // skip first 7 characters of content_id
    file.readBytes(&pkgTitleID, sizeof(pkgTitleID));

    file.close();

    return true;
}

bool PKG::Extract(const std::string& filepath, const std::filesystem::path& extract,
                  std::string& failreason) {
    extract_path = extract;
    pkgpath = filepath;
    IOFile file(filepath);
    if (!file.isOpen()) {
        return false;
    }
    pkgSize = file.size().value();
    if (const auto [is_read, num_read] = file.readBytes(&pkgheader, sizeof(PKGHeader));
        !is_read || num_read != sizeof(PKGHeader)) {
        failreason = "Could not read PKG header";
        return false;
    }

    if (pkgheader.pkg_size > pkgSize) {
        failreason = "PKG file size is different";
        return false;
    }
    if ((pkgheader.pkg_content_size + pkgheader.pkg_content_offset) > pkgheader.pkg_size) {
        failreason = "Content size is bigger than pkg size";
        return false;
    }
    file.seek(0);
    pkg.resize(pkgheader.pkg_promote_size);
    file.readBytes(pkg.data(), pkgheader.pkg_promote_size);

    u32 offset = pkgheader.pkg_table_entry_offset;
    u32 n_files = pkgheader.pkg_table_entry_count;

    std::array<u8, 32> seed_digest;
    std::array<std::array<u8, 32>, 7> digest1;
    std::array<std::array<u8, 256>, 7> key1;
    std::array<u8, 256> imgkeydata;

    for (int i = 0; i < n_files; i++) {
        PKGEntry entry;
        std::memcpy(&entry, &pkg[offset + i * 0x20], sizeof(entry));

        // Try to figure out the name
        const auto name = GetEntryNameByType(entry.id);
        if (name.empty()) {
            // Just print with id
            IOFile out(extract_path / "sce_sys" / std::to_string(entry.id), "wb");
            out.writeBytes(pkg.data() + entry.offset, entry.size);
            out.close();
            continue;
        }

        const auto filepath = extract_path / "sce_sys" / name;
        std::filesystem::create_directories(filepath.parent_path());

        if (entry.id == 0x1) {         // DIGESTS, seek;
                                       // file.Seek(entry.offset, fsSeekSet);
        } else if (entry.id == 0x10) { // ENTRY_KEYS, seek;
            file.seek(entry.offset);
            file.readBytes(seed_digest.data(), seed_digest.size());

            for (int i = 0; i < 7; i++) {
                file.readBytes(digest1[i].data(), digest1[i].size());
            }

            for (int i = 0; i < 7; i++) {
                file.readBytes(key1[i].data(), key1[i].size());
            }

            PKG::crypto.RSA2048Decrypt(dk3_, key1[3], true); // decrypt DK3
        } else if (entry.id == 0x20) {                       // IMAGE_KEY, seek; IV_KEY
            file.seek(entry.offset);
            file.readBytes(imgkeydata.data(), imgkeydata.size());

            // The Concatenated iv + dk3 imagekey for HASH256
            std::array<CryptoPP::byte, 64> concatenated_ivkey_dk3;
            std::memcpy(concatenated_ivkey_dk3.data(), &entry, sizeof(entry));
            std::memcpy(concatenated_ivkey_dk3.data() + sizeof(entry), dk3_.data(), sizeof(dk3_));

            PKG::crypto.ivKeyHASH256(concatenated_ivkey_dk3, ivKey); // ivkey_
            // imgkey_ to use for last step to get ekpfs
            PKG::crypto.aesCbcCfb128Decrypt(ivKey, imgkeydata, imgKey);
            // ekpfs key to get data and tweak keys.
            PKG::crypto.RSA2048Decrypt(ekpfsKey, imgKey, false);
        } else if (entry.id == 0x80) {
            // GENERAL_DIGESTS, seek;
            // file.Seek(entry.offset, fsSeekSet);
        }

        IOFile out(extract_path / "sce_sys" / name, "wb");
        out.writeBytes(pkg.data() + entry.offset, entry.size);
        out.close();

        if (name == "pic1.png") {
            std::vector<u8> imageData(entry.size);
            std::memcpy(imageData.data(), pkg.data() + entry.offset, entry.size);
            QImage image = QImage::fromData((imageData.data()), entry.size);
            QImage image_ = game_utils.BlurImage(image, image.rect(), 18);

            std::filesystem::path img_path = std::filesystem::path("game_data/") / GetTitleID();
            std::filesystem::create_directories(img_path);
            if (!image_.save(QString::fromStdString(img_path.string() + "/pic1.png"), "PNG")) {
                // qDebug() << "Error: Unable to save image.";
            }
        }
    }

    // Read the seed
    std::array<CryptoPP::byte, 16> seed;
    file.seek(pkgheader.pfs_image_offset + 0x370);
    file.readBytes(seed.data(), seed.size());

    // Get data and tweak keys.
    PKG::crypto.PfsGenCryptoKey(ekpfsKey, seed, dataKey, tweakKey);
    const u32 length = pkgheader.pfs_cache_size * 0x2; // Seems to be ok.

    // Read encrypted pfs_image
    std::vector<u8> pfs_encrypted(length);
    file.seek(pkgheader.pfs_image_offset);
    file.readBytes(pfs_encrypted.data(), length);

    // Decrypt the pfs_image.
    std::vector<u8> pfs_decrypted(length);
    PKG::crypto.decryptPFS(dataKey, tweakKey, pfs_encrypted, pfs_decrypted, 0);

    // Retrieve PFSC from decrypted pfs_image.
    pfsc_offset = GetPFSCOffset(pfs_decrypted);
    std::vector<u8> pfsc(length);
    std::memcpy(pfsc.data(), pfs_decrypted.data() + pfsc_offset, length - pfsc_offset);

    PFSCHdr pfsChdr;
    std::memcpy(&pfsChdr, pfsc.data(), sizeof(pfsChdr));

    const int num_blocks = (int)(pfsChdr.data_length / pfsChdr.block_sz2);
    sectorMap.resize(num_blocks + 1); // 8 bytes, need extra 1 to get the last offset.

    for (int i = 0; i < num_blocks + 1; i++) {
        std::memcpy(&sectorMap[i], pfsc.data() + pfsChdr.block_offsets + i * 8, 8);
    }

    u32 ent_size = 0;
    u32 ndinode = 0;

    std::vector<char> compressedData;
    std::vector<char> decompressedData(0x10000);
    bool dinode_reched = false;
    // Get iNdoes.
    for (int i = 0; i < num_blocks; i++) {
        const u64 sectorOffset = sectorMap[i];
        const u64 sectorSize = sectorMap[i + 1] - sectorOffset;

        compressedData.resize(sectorSize);
        std::memcpy(compressedData.data(), pfsc.data() + sectorOffset, sectorSize);

        if (sectorSize == 0x10000) // Uncompressed data
            std::memcpy(decompressedData.data(), compressedData.data(), 0x10000);
        else if (sectorSize < 0x10000) // Compressed data
            DecompressPFSC(compressedData, decompressedData);

        if (i == 0) {
            std::memcpy(&ndinode, decompressedData.data() + 0x30, 4); // number of folders and files
        }

        int occupied_blocks =
            (ndinode * 0xA8) / 0x10000; // how many blocks(0x10000) are taken by iNodes.
        if (((ndinode * 0xA8) % 0x10000) != 0)
            occupied_blocks += 1;

        if (i >= 1 && i <= occupied_blocks) { // Get all iNodes, gives type, file size and location.
            for (int p = 0; p < 0x10000; p += 0xA8) {
                Inode node;
                std::memcpy(&node, &decompressedData[p], sizeof(node));
                if (node.Mode == 0) {
                    break;
                }
                iNodeBuf.push_back(node);
            }
        }

        const char dot = decompressedData[0x10];
        const std::string_view dotdot(&decompressedData[0x28], 2);
        if (dot == '.' && dotdot == "..") {
            dinode_reched = true;
        }

        // Get folder and file names.
        if (dinode_reched) {
            for (int j = 0; j < 0x10000; j += ent_size) { // Skip the first parent and child.
                Dirent dirent;
                std::memcpy(&dirent, &decompressedData[j], sizeof(dirent));

                // Stop here and continue the main loop
                if (dirent.ino == 0) {
                    break;
                }

                ent_size = dirent.entsize;

                auto& table = fsTable.emplace_back();
                table.name = std::string(dirent.name, dirent.namelen);
                table.inode = dirent.ino;
                table.type = dirent.type;

                if (table.type == PFS_DIR) {
                    folderMap[table.inode] = table.name;
                }
            }

            // Seems to be the last entry, at least for the games I tested. To check as we go.
            const std::string_view rightsprx(&decompressedData[0x40], 10);
            if (rightsprx == "right.sprx") {
                break;
            }
        }
    }

    // Create Folders.
    folderMap[2] = GetTitleID(); // Set up game path instead of calling it uroot
    game_dir = extract_path;     // std::filesystem::current_path() / "game";
    title_dir = game_dir / GetTitleID();

    // Game dir already created but ok let's leave it for now.
    std::filesystem::create_directories(title_dir);

    for (int i = 0; i < fsTable.size(); i++) {
        const u32 inode_number = fsTable[i].inode;
        const u32 inode_type = fsTable[i].type;
        const auto inode_name = fsTable[i].name;

        if (inode_type == PFS_CURRENT_DIR) {
            current_dir = folderMap[inode_number];
        } else if (inode_type == PFS_PARENT_DIR) {
            parent_dir = folderMap[inode_number];
            // Skip uroot folder. we create our own game uid folder
            if (i > 1) {
                const auto par_dir = inode_number == 2 ? findDirectory(game_dir, parent_dir)
                                                       : findDirectory(title_dir, parent_dir);
                const auto cur_dir = findDirectory(par_dir, current_dir);

                if (cur_dir == "") {
                    extract_path = par_dir / current_dir;
                    std::filesystem::create_directories(extract_path);
                } else {
                    extract_path = cur_dir;
                }
            }
        }
        extractMap[inode_number] = extract_path.string();
    }
    return true;
}

void PKG::ExtractFiles(const int& index) {
    int inode_number = fsTable[index].inode;
    int inode_type = fsTable[index].type;
    std::string inode_name = fsTable[index].name;

    if (inode_type == PFS_FILE) {
        int sector_loc = iNodeBuf[inode_number].loc;
        int nblocks = iNodeBuf[inode_number].Blocks;
        int bsize = iNodeBuf[inode_number].Size;
        std::string file_extracted = extractMap[inode_number] + "/" + inode_name;

        IOFile inflated;
        inflated.open(file_extracted, "wb");

        IOFile pkgFile; // Open the file for each iteration to avoid conflict.
        pkgFile.open(pkgpath);

        int size_decompressed = 0;
        std::vector<char> compressedData;
        std::vector<char> decompressedData(0x10000);

        u64 pfsc_buf_size = 0x11000; // extra 0x1000
        std::vector<u8> pfsc(pfsc_buf_size);
        std::vector<u8> pfs_decrypted(pfsc_buf_size);

        for (int j = 0; j < nblocks; j++) {
            u64 sectorOffset =
                sectorMap[sector_loc + j]; // offset into PFSC_image and not pfs_image.
            u64 sectorSize = sectorMap[sector_loc + j + 1] -
                             sectorOffset; // indicates if data is compressed or not.
            u64 fileOffset = (pkgheader.pfs_image_offset + pfsc_offset + sectorOffset);
            u64 currentSector1 =
                (pfsc_offset + sectorOffset) / 0x1000; // block size is 0x1000 for xts decryption.

            int sectorOffsetMask = (sectorOffset + pfsc_offset) & 0xFFFFF000;
            int previousData = (sectorOffset + pfsc_offset) - sectorOffsetMask;

            pkgFile.seek(fileOffset - previousData);
            pkgFile.readBytes(pfsc.data(), pfsc.size());

            PKG::crypto.decryptPFS(dataKey, tweakKey, pfsc, pfs_decrypted, currentSector1);

            compressedData.resize(sectorSize);
            std::memcpy(compressedData.data(), pfs_decrypted.data() + previousData, sectorSize);

            if (sectorSize == 0x10000) // Uncompressed data
                std::memcpy(decompressedData.data(), compressedData.data(), 0x10000);
            else if (sectorSize < 0x10000) // Compressed data
                DecompressPFSC(compressedData, decompressedData);

            size_decompressed += 0x10000;

            if (j < nblocks - 1) {
                inflated.writeBytes(decompressedData.data(), decompressedData.size());
            } else {
                // This is to remove the zeros at the end of the file.
                const u32 write_size = decompressedData.size() - (size_decompressed - bsize);
                inflated.writeBytes(decompressedData.data(), write_size);
            }

            // Close the file for each iteration to avoid conflict.
        }
        pkgFile.close();
        inflated.close();
    }
}
