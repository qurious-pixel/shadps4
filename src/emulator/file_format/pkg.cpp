#include "pkg.h"

PKG::PKG() {}

PKG::~PKG() {}

bool PKG::open(const std::string& filepath) {
    IOFile file;
    if (!file.open(filepath)) {
        return false;
    }
    pkgSize = file.size().value();
    PKGHeader pkgheader;
    // we have already checked magic should be ok
    file.readBytes(&pkgheader, sizeof(PKGHeader));

    // find title id it is part of pkg_content_id starting at offset 0x40
    file.seek(0x47); // skip first 7 characters of content_id
    file.readBytes(&pkgTitleID, sizeof(pkgTitleID));

    file.close();

    return true;
}

bool PKG::extract(const std::string& filepath, const std::string& extractPath,
                  std::string& failreason) {
    this->extractPath = extractPath;
    pkgpath = filepath;
    IOFile file;
    if (!file.open(filepath)) {
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
    pkg.resize(pkgSize);
    file.readBytes(pkg.data(), pkgSize);

    u32 offset = pkgheader.pkg_table_entry_offset;
    u32 n_files = pkgheader.pkg_table_entry_count;

    unsigned char seedDigest[32];
    unsigned char** digest1 = new unsigned char*[7];
    unsigned char** key1 = new unsigned char*[7];
    unsigned char* imgkeydata = new unsigned char[256];

    for (int i = 0; i < 7; i++) {
        digest1[i] = new unsigned char[32];
        key1[i] = new unsigned char[256];
    }

    for (int i = 0; i < n_files; i++) {
        PKGEntry entry = (PKGEntry&)pkg[offset + i * 0x20];

        // try to figure out the name
        std::string name = getEntryNameByType(entry.id);

        if (!name.empty()) {
            QString filepath = QString::fromStdString(extractPath + "/sce_sys/" + name);
            QDir dir = QFileInfo(filepath).dir();
            if (!dir.exists()) {
                dir.mkpath(dir.path());
            }

            if (entry.id == 0x1) {         // DIGESTS, seek;
                                           // file.Seek(entry.offset, fsSeekSet);
            } else if (entry.id == 0x10) { // ENTRY_KEYS, seek;
                file.seek(entry.offset);
                file.readBytes(seedDigest, 32);

                for (int i = 0; i < 7; i++)
                    file.readBytes(digest1[i], 32);

                for (int i = 0; i < 7; i++)
                    file.readBytes(key1[i], 256);

                PKG::crypto.RSA2048Decrypt(dk3_, key1[3], true); // decrypt DK3
            } else if (entry.id == 0x20) {                       // IMAGE_KEY, seek; IV_KEY
                file.seek(entry.offset);
                file.readBytes(imgkeydata, 256);

                PKGEntry tmp_entry = entry;
                // WriteBE(tmp_entry);
                CryptoPP::byte* tmpbuf = tmp_entry.GetBytes(); // get pkgentry bytes
                CryptoPP::byte* concatenated_ivkey_dk3 = new CryptoPP::byte[32 + 32];
                concatenate(
                    tmpbuf, dk3_,
                    concatenated_ivkey_dk3); // The Concatenated iv + dk3 imagekey for HASH256

                PKG::crypto.ivKeyHASH256(concatenated_ivkey_dk3, ivKey); // ivkey_
                PKG::crypto.aesCbcCfb128Decrypt(
                    ivKey, imgkeydata,
                    imgKey); // imgkey_ to use for last step to get ekpfs
                PKG::crypto.RSA2048Decrypt(ekpfsKey, imgKey,
                                           false); // ekpfs key to get data and tweak keys.
            } else if (entry.id == 0x80) {
                // GENERAL_DIGESTS, seek;
                // file.Seek(entry.offset, fsSeekSet);
            }

            IOFile out;
            out.open(extractPath + "/sce_sys/" + name, "wb");
            out.writeBytes(pkg.data() + entry.offset, entry.size);
            out.close();
        } else {
            // just print with id
            IOFile out;
            out.open(extractPath + "/sce_sys/" + std::to_string(entry.id), "wb");
            out.writeBytes(pkg.data() + entry.offset, entry.size);
            out.close();
        }
    }

    CryptoPP::byte* seed = new CryptoPP::byte[16];

    if (!file.seek(pkgheader.pfs_image_offset + 0x370)) {
        return false;
    }
    file.readBytes(seed, 16);

    // Get data and tweak keys.
    PKG::crypto.PfsGenCryptoKey(dataTweakKey, ekpfsKey, seed, dataKey, tweakKey);

    delete[] seed;
    int length = pkgheader.pfs_cache_size * 0x2; // Seems to be ok.

    std::vector<u08> pfs_copy(length);
    if (!file.seek(pkgheader.pfs_image_offset)) {
        return false;
    }
    file.readBytes(pfs_copy.data(), length);

    u08* pfs_decrypted = new u08[length];
    // std::memcpy(pfs_decrypted, pfs_copy, 0x10000);  // copy the first 16 blocks "as is", they
    // will retain the pfs header. Not Encrypted.
    // pfs_decrypted[0x1C] = pfs_copy[0x1C] & ~4;	    // remove the "encrypted" flag, no need to
    // do this but ok.

    PKG::crypto.decryptPFS(dataKey, tweakKey, pfs_copy.data(), pfs_decrypted, length,
                           0); // Decrypt the pfs_image.

    pfscPos = get_pfsc_pos(pfs_decrypted, length);
    u08* pfsc = new u08[length];
    std::memcpy(pfsc, pfs_decrypted + pfscPos, length - pfscPos);

    delete[] pfs_decrypted;

    PFSCHdr pfsChdr = (PFSCHdr&)pfsc[0];


    int num_blocks = (int)(pfsChdr.DataLength / pfsChdr.BlockSz2);
    sectorMap = new u64[num_blocks + 1]; // 8 bytes, need extra 1 to get the last offset.

    for (int i = 0; i < num_blocks + 1; i++) {
        std::memcpy(&sectorMap[i], pfsc + pfsChdr.BlockOffsets + i * 8, 8);
    }

    int ent_size = 0;
    int ndinode = 0;
    int dinodePos = 0;

    // Get iNdoes.
    for (int i = 0; i < num_blocks; i++) {
        u64 sectorOffset = sectorMap[i];
        u64 sectorSize = sectorMap[i + 1] - sectorOffset;

        char* compressedData = new char[sectorSize];
        char* decompressedData = new char[0x10000];

        std::memcpy(compressedData, pfsc + sectorOffset, sectorSize);

        if (sectorSize == 0x10000) // Uncompressed data
            std::memcpy(decompressedData, compressedData, 0x10000);
        else if (sectorSize < 0x10000) // Compressed data
            decompress_pfsc(compressedData, sectorSize, decompressedData, 0x10000);

        if (i == 0) {
            std::memcpy(&ndinode, decompressedData + 0x30, 4); // number of folders and files
        }

        int occupied_blocks =
            (ndinode * 0xA8) / 0x10000; // how many blocks(0x10000) are taken by iNodes.
        if (((ndinode * 0xA8) % 0x10000) != 0)
            occupied_blocks += 1;

        if (i >= 1 && i <= occupied_blocks) { // Get all iNodes, gives type, file size and location.
            for (int p = 0; p < 0x10000; p += 0xA8) {
                Inode tmp = (Inode&)decompressedData[p];

                if (tmp.Mode == 0)
                    break;

                iNodeBuf.push_back((Inode&)decompressedData[p]);
            }
        }

        std::string dot(&decompressedData[0x10], 1);
        std::string dotdot(&decompressedData[0x28], 2);

        if (dot == "." && dotdot == "..") {
            dinodePos = i;
            delete[] compressedData;
            delete[] decompressedData;
            break;
        }

        delete[] compressedData;
        delete[] decompressedData;
    }

    // Get folder and file names.
    for (int i = dinodePos; i < num_blocks; i++) {
        u64 sectorOffset = sectorMap[i];
        u64 sectorSize = sectorMap[i + 1] - sectorOffset;

        char* compressedData = new char[sectorSize];
        char* decompressedData = new char[0x10000];

        std::memcpy(compressedData, pfsc + sectorOffset, sectorSize);

        if (sectorSize == 0x10000) // Uncompressed data
            std::memcpy(decompressedData, compressedData, 0x10000);
        else if (sectorSize < 0x10000) // Compressed data
            decompress_pfsc(compressedData, sectorSize, decompressedData, 0x10000);

        Dirent dirent = (Dirent&)decompressedData[0];

        std::string rightsprx(&decompressedData[0x40], 10);

        for (int j = 0; j < 0x10000; j += ent_size) { // skip the first parent and child.
            Dirent dirent1 = (Dirent&)decompressedData[j];

            if (dirent1.ino == 0) // stop here and continue the main loop
                break;

            std::string name(dirent1.name, dirent1.namelen);
            ent_size = dirent1.entsize;

            pfs_fs_table tmp;
            tmp.name = name;
            tmp.inode = dirent1.ino;
            tmp.type = dirent1.type;
            fsTable.push_back(tmp);

            if (tmp.type == PFS_DIR) {
                folderMap[tmp.inode] = tmp.name;
            }
        }

        if (rightsprx == "right.sprx") { // Seems to be the last entry, at least for the games I
                                         // tested. To check as we go.
            delete[] compressedData;
            delete[] decompressedData;
            break;
        }
        delete[] compressedData;
        delete[] decompressedData;
    }
    delete[] pfsc;

    // Create Folders.
    folderMap[2] = PKG::getTitleID(); // set up game path instead of calling it uroot
    game_dir_ = QDir::currentPath() + "/game/";
    gameDir = QDir(game_dir_ + QString(folderMap[2].c_str()));
    extract_path = gameDir.absolutePath();

    PKG::createDirectory(
        game_dir_ +
        (folderMap[2].c_str())); // game dir already created but ok let's leave it for now.

    for (int i = 0; i < fsTable.size(); i++) {
        int inode_number = fsTable[i].inode;
        int inode_type = fsTable[i].type;
        std::string inode_name = fsTable[i].name;

        if (inode_type == PFS_CURRENT_DIR) {
            currentDir = folderMap[inode_number].c_str();
        } else if (inode_type == PFS_PARENT_DIR) {
            parentDir = folderMap[inode_number].c_str();

            if (i > 1) { // skip uroot folder. we create our own game uid folder
                QString par_dir;

                if (inode_number == 2) // Check the parent dir first.
                    par_dir = PKG::findDirectory(QDir(game_dir_), parentDir);
                else
                    par_dir = PKG::findDirectory(gameDir, parentDir);

                // Use the checked parent Dir path to check for children dir and avoid using the
                // game folder as a point of search.
                QString cur_dir = PKG::findDirectory(QDir(par_dir), currentDir);

                if (cur_dir == "") {
                    extract_path = par_dir + QString("/") + (currentDir);
                    createDirectory(extract_path);
                } else {
                    extract_path = cur_dir;
                }
            }
        }
        extractMap[inode_number] = extract_path;
    }
    return true;
}

// Decrypt and Extract pfs_image content.
void PKG::extractFiles(const int& index) {

    int inode_number = fsTable[index].inode;
    int inode_type = fsTable[index].type;
    std::string inode_name = fsTable[index].name;

    if (inode_type == PFS_FILE) {
        int sector_loc = iNodeBuf[inode_number].loc;
        int nblocks = iNodeBuf[inode_number].Blocks;
        int bsize = iNodeBuf[inode_number].Size;
        std::string file_extracted = extractMap[inode_number].toStdString() + "/" + inode_name;

        IOFile inflated;
        inflated.open(file_extracted, "wb");

        int size_decompressed = 0;

        for (int j = 0; j < nblocks; j++) {
            u64 pfsc_buf_size = 0x11000; // extra 0x1000
            u64 sectorOffset =
                sectorMap[sector_loc + j]; // offset into PFSC_image and not pfs_image.
            u64 sectorSize = sectorMap[sector_loc + j + 1] -
                             sectorOffset; // indicates if data is compressed or not.
            u64 fileOffset = (pkgheader.pfs_image_offset + pfscPos + sectorOffset);
            u64 currentSector1 =
                (pfscPos + sectorOffset) / 0x1000; // block size is 0x1000 for xts decryption.

            int sectorOffsetMask = (sectorOffset + pfscPos) & 0xFFFFF000;
            int previousData = (sectorOffset + pfscPos) - sectorOffsetMask;

            u08* pfsc = new u08[pfsc_buf_size];
            u08* pfs_decrypted = new u08[pfsc_buf_size];

            IOFile file; // Open the file for each iteration to avoid conflict.
            if (!file.open(pkgpath)) {
                // return false;
            }

            file.seek(fileOffset - previousData);
            file.readBytes(pfsc, pfsc_buf_size);

            PKG::crypto.decryptPFS(dataKey, tweakKey, pfsc, pfs_decrypted, pfsc_buf_size,
                                   currentSector1);

            delete[] pfsc;

            char* compressedData = new char[sectorSize];
            char* decompressedData = new char[0x10000];

            std::memcpy(compressedData, pfs_decrypted + previousData, sectorSize);
            delete[] pfs_decrypted;

            if (sectorSize == 0x10000) // Uncompressed data
                std::memcpy(decompressedData, compressedData, 0x10000);
            else if (sectorSize < 0x10000) // Compressed data
                decompress_pfsc(compressedData, sectorSize, decompressedData, 0x10000);

            size_decompressed += 0x10000;

            if (j < nblocks - 1)
                inflated.writeBytes(decompressedData, 0x10000);
            else
                inflated.writeBytes(
                    decompressedData,
                    0x10000 - (size_decompressed -
                               bsize)); // This is to remove the zeros at the end of the file.

            delete[] compressedData;
            delete[] decompressedData;
            file.close();// Close the file for each iteration to avoid conflict.
        }
        inflated.close();
    }
}

u32 PKG::getNumberOfFiles() {
    return fsTable.size();
}
