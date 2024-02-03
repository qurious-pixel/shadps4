#include "PKG.h"
#include "../../core/FsFile.h"
#include <direct.h> 

PKG::PKG()
{
}

PKG::~PKG()
{
}

bool PKG::open(const std::string& filepath) {
	FsFile file;
	if (!file.Open(filepath, fsRead))
	{
		return false;
	}
	pkgSize = file.getFileSize();
	PKGHeader pkgheader;
	file.ReadBE(pkgheader);
	//we have already checked magic should be ok

	//find title id it is part of pkg_content_id starting at offset 0x40
	file.Seek(0x47, fsSeekSet);//skip first 7 characters of content_id 
	file.Read(&pkgTitleID, sizeof(pkgTitleID));

	file.Close();

	return true;
}

bool PKG::extract(const std::string& filepath, const std::string& extractPath, std::string& failreason)
{
	this->extractPath = extractPath;
	FsFile file;
	if (!file.Open(filepath, fsRead))
	{
		return false;
	}
	pkgSize = file.getFileSize();
	PKGHeader pkgheader;
	file.ReadBE(pkgheader);

	if (pkgheader.pkg_size > pkgSize)
	{
		failreason = "PKG file size is different";
		return false;
	}
	if ((pkgheader.pkg_content_size + pkgheader.pkg_content_offset) > pkgheader.pkg_size)
	{
		failreason = "Content size is bigger than pkg size";
		return false;
	}
	file.Seek(0, fsSeekSet);
	pkg = (U08*)mmap(pkgSize, file.fileDescr(), 0);
	if (pkg == nullptr)
	{
		failreason = "Can't allocate size for image";
		return false;
	}

	file.Read(pkg, pkgSize);

	U32 offset = pkgheader.pkg_table_entry_offset;
	U32 n_files = pkgheader.pkg_table_entry_count;

	Crypto_Keys crypto_keys;
	unsigned char seedDigest[32];
	unsigned char** digest1 = new unsigned char* [7];
	unsigned char** key1 = new unsigned char* [7];
	unsigned char* imgkeydata = new unsigned char[256];

	for (int i = 0; i < 7; i++) {
		digest1[i] = new unsigned char[32];
		key1[i] = new unsigned char[256];
	}


	for (int i = 0; i < n_files; i++) {
		PKGEntry entry = (PKGEntry&)pkg[offset + i * 0x20];
		ReadBE(entry);
		//try to figure out the name
		std::string name = getEntryNameByType(entry.id);

		if (!name.empty())
		{
			QString filepath = QString::fromStdString(extractPath + "/sce_sys/" + name);
			QDir dir = QFileInfo(filepath).dir();
			if (!dir.exists()) {
				dir.mkpath(dir.path());
			}

			if (entry.id == 0x1)// DIGESTS, seek;
			{
				// file.Seek(entry.offset, fsSeekSet);
			}
			else if (entry.id == 0x10)// ENTRY_KEYS, seek;
			{
				file.Seek(entry.offset, fsSeekSet);
				file.Read(seedDigest, 32);

				for (int i = 0; i < 7; i++)
					file.Read(digest1[i], 32);

				for (int i = 0; i < 7; i++)
					file.Read(key1[i], 256);

				PKG::crypto.RSA2048Decrypt(crypto_keys.dk3_, key1[3], true); // decrypt DK3
			}
			else if (entry.id == 0x20) // IMAGE_KEY, seek; IV_KEY
			{
				file.Seek(entry.offset, fsSeekSet);
				file.Read(imgkeydata, 256); // what do we do with this now? Ok, this is the imagekey to use in aesCbcCfb128Decrypt

				PKGEntry tmp_entry = entry;
				WriteBE(tmp_entry);
				CryptoPP::byte* tmpbuf = tmp_entry.GetBytes();//get pkgentry bytes
				CryptoPP::byte* concatenated_ivkey_dk3 = new CryptoPP::byte[32 + 32];
				concatenate(tmpbuf, crypto_keys.dk3_, concatenated_ivkey_dk3);// The Concatenated iv + dk3 imagekey for HASH256


				PKG::crypto.ivKeyHASH256(concatenated_ivkey_dk3, crypto_keys.ivkey); //ivkey_
				PKG::crypto.aesCbcCfb128Decrypt(crypto_keys.ivkey, imgkeydata, crypto_keys.imgkey); //imgkey_ to use for last step to get ekpfs
				PKG::crypto.RSA2048Decrypt(crypto_keys.ekpfs_key, crypto_keys.imgkey, false);   //ekpfs key to get data and tweak keys.
			}
			else if (entry.id == 0x80)
			{
				// GENERAL_DIGESTS, seek;
				// file.Seek(entry.offset, fsSeekSet);
			}

			FsFile out;
			out.Open(extractPath + "/sce_sys/" + name, fsWrite);
			out.Write(pkg + entry.offset, entry.size);
			out.Close();
		}
		else
		{
			//just print with id
			FsFile out;
			out.Open(extractPath + "/sce_sys/" + std::to_string(entry.id), fsWrite);
			out.Write(pkg + entry.offset, entry.size);
			out.Close();
		}
	}
	munmap(pkg); // not needed anymore, free some memory.

	CryptoPP::byte* seed = new CryptoPP::byte[16];

	file.Seek(pkgheader.pfs_image_offset + 0x370, fsSeekSet);
	file.Read(seed, 16);

	//Get data and tweak keys.
	PKG::crypto.PfsGenCryptoKey(crypto_keys.data_tweak_key, crypto_keys.ekpfs_key, seed, crypto_keys.dataKey, crypto_keys.tweakKey);

	delete[] seed;// not needed anymore, free some memory.

	int length = 0x1200000;//pkgheader.pfs_image_size; // 0x1200000 should be enough for now, we can always change it later if we encounter an issue.
	U08* pfs_copy = (U08*)mmap(length, file.fileDescr(), pkgheader.pfs_image_offset);

	U08* pfs_decrypted = new U08[length];
	//std::memcpy(pfs_decrypted, pfs_copy, 0x10000);  // copy the first 16 blocks "as is", they will retain the pfs header. Not Encrypted.
	//pfs_decrypted[0x1C] = pfs_copy[0x1C] & ~4;	    // remove the "encrypted" flag, no need to do this but ok.

	PKG::crypto.decryptPFS(crypto_keys.dataKey, crypto_keys.tweakKey, pfs_copy, pfs_decrypted, length, 0); // Decrypt the pfs_image.

	munmap(pfs_copy); // not needed anymore, free some memory.

	U64 pfsc_pos = get_pfsc_pos(pfs_decrypted, length);
	U08* pfsc = new U08[length];
	std::memcpy(pfsc, pfs_decrypted + pfsc_pos, length - pfsc_pos);

	delete[] pfs_decrypted; // not needed anymore, free some memory.

	PFSCHdr pfsChdr = (PFSCHdr&)pfsc[0];

	int num_blocks = (int)(pfsChdr.DataLength / pfsChdr.BlockSz2);
	U64* sectorMap = new U64[num_blocks + 1]; // 8 bytes, need extra 1 to get the last offset.

	for (int i = 0; i < num_blocks + 1; i++)
	{
		std::memcpy(&sectorMap[i], pfsc + pfsChdr.BlockOffsets + i * 8, 8);
	}

	int ent_size = 0;
	std::vector<Inode> iNode_buf;
	int ndinode = 0;
	std::vector<pfs_fs_table> fs_table;
	std::unordered_map <int, std::string> folder_map;

	int dinodePos = 0;

	// Get iNdoes.
	for (int i = 0; i < num_blocks; i++)
	{
		U64 sectorOffset = sectorMap[i];
		U64 sectorSize = sectorMap[i + 1] - sectorOffset;

		char* compressedData = new char[sectorSize];
		char* decompressedData = new char[0x10000];

		std::memcpy(compressedData, pfsc + sectorOffset, sectorSize);

		if (sectorSize == 0x10000) // Uncompressed data 
			std::memcpy(decompressedData, compressedData, 0x10000);
		else if (sectorSize < 0x10000) // Compressed data
			decompress_pfsc(compressedData, sectorSize, decompressedData, 0x10000);

		if (i == 0)
		{
			std::memcpy(&ndinode, decompressedData + 0x30, 4); // number of folders and files
		}
		
		int occupied_blocks = (ndinode * 0xA8) / 0x10000; // how many blocks(0x10000) are taken by iNodes.
		if (((ndinode * 0xA8) % 0x10000) != 0)
			occupied_blocks += 1;

		if (i >= 1 && i <= occupied_blocks) // Get all iNodes, gives type, file size and location.
		{
			for (int p = 0; p < 0x10000; p += 0xA8)
			{
				Inode tmp = (Inode&)decompressedData[p];

				if (tmp.Mode == 0)
					break;

				iNode_buf.push_back((Inode&)decompressedData[p]);
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
	for (int i = dinodePos; i < num_blocks; i++)
	{
		U64 sectorOffset = sectorMap[i];
		U64 sectorSize = sectorMap[i + 1] - sectorOffset;

		char* compressedData = new char[sectorSize];
		char* decompressedData = new char[0x10000];

		std::memcpy(compressedData, pfsc + sectorOffset, sectorSize);

		if (sectorSize == 0x10000) // Uncompressed data 
			std::memcpy(decompressedData, compressedData, 0x10000);
		else if (sectorSize < 0x10000) // Compressed data
			decompress_pfsc(compressedData, sectorSize, decompressedData, 0x10000);

		Dirent dirent = (Dirent&)decompressedData[0];

		std::string rightsprx(&decompressedData[0x40], 10);		

		for (int j = 0; j < 0x10000; j += ent_size) //skip the first parent and child.
		{
			Dirent dirent1 = (Dirent&)decompressedData[j];

			if (dirent1.ino == 0)  //stop here and continue the main loop
				break;

			std::string name(dirent1.name, dirent1.namelen);
			ent_size = dirent1.entsize;

			pfs_fs_table tmp;
			tmp.name = name;
			tmp.inode = dirent1.ino;
			tmp.type = dirent1.type;
			fs_table.push_back(tmp);

			if (tmp.type == PFS_DIR)
			{
				folder_map[tmp.inode] = tmp.name;
			}
		}

		if (rightsprx == "right.sprx") { // Seems to be the last entry, at least for the games I tested. To check as we go.
			delete[] compressedData;
			delete[] decompressedData;
			break;
		}
		delete[] compressedData;
		delete[] decompressedData;
	}
	delete[] pfsc;

	// Create Folders, Decrypt and Extract pfs_image content. 

	QString currentDir = "";
	QString parentDir = "";
	folder_map[2] = PKG::getTitleID();//set up game path instead of calling it uroot
	QString game_dir_ = QDir::currentPath() + "/game/";
	QDir gameDir(game_dir_ + QString(folder_map[2].c_str()));
	QString extract_path = gameDir.absolutePath();

	PKG::createDirectory(game_dir_ + (folder_map[2].c_str())); // game dir already created but ok let's leave it for now.	

	for (int i = 0; i < fs_table.size(); i++)
	{
		QApplication::processEvents(); // not ideal but it helps when the window freezes.

		int inode_number = fs_table[i].inode;
		int inode_type = fs_table[i].type;
		std::string inode_name = fs_table[i].name;

		if (inode_type == PFS_CURRENT_DIR)
		{
			currentDir = folder_map[inode_number].c_str();
		}
		else if (inode_type == PFS_PARENT_DIR)
		{
			parentDir = folder_map[inode_number].c_str();

			if (i > 1) {//skip uroot folder. we create our own game uid folder
				QString par_dir;

				if (inode_number == 2) //Check the parent dir first.
					par_dir = PKG::findDirectory(QDir(game_dir_), parentDir);
				else
					par_dir = PKG::findDirectory(gameDir, parentDir);

				//Use the checked parent Dir path to check for children dir and avoid using the game folder as a point of search.
				QString cur_dir = PKG::findDirectory(QDir(par_dir), currentDir);

				if (cur_dir == "")
				{
					extract_path = par_dir + QString("/") + (currentDir);
					createDirectory(extract_path);
				}
				else
				{
					extract_path = cur_dir;
				}
			}
		}
		else if (inode_type == PFS_FILE)
		{
			int sector_loc = iNode_buf[inode_number].loc;
			int nblocks = iNode_buf[inode_number].Blocks;
			int bsize = iNode_buf[inode_number].Size;
			std::string file_extracted = extract_path.toStdString() + "/" + inode_name;

			U64 pfsc_buf_size = nblocks * 0x10000;

			U64 sectorOffset = sectorMap[sector_loc]; // offset into PFSC_image and not pfs_image.
			U64 sectorSize = sectorMap[sector_loc + 1] - sectorOffset; // indicates if data is compressed or not.
			U64 file_offset_ = (pkgheader.pfs_image_offset + pfsc_pos + sectorOffset);

			U64 currentSector = (sectorOffset + pfsc_pos) / 0x1000; // block size is 0x1000 for xts decryption.

			// Always mask the last 3 digist as the decryption is done every 0x1000 block and has to start from 0x0000 to 0x1000. decrypting by 0x10 blocks.
			int sectorOffsetMask = (sectorOffset + pfsc_pos) & 0xFFFFF000;
			int previousData = (sectorOffset + pfsc_pos) - sectorOffsetMask;

			U08* pfsc_copy_ = new U08[pfsc_buf_size + previousData];
			U08* pfs_decrypted_tmp = new U08[pfsc_buf_size + previousData];

			file.Seek(file_offset_ - previousData, fsSeekSet);
			file.ReadBytes(pfsc_copy_, pfsc_buf_size + previousData);

			//Decrypt data. // Move this later inside the loop
			PKG::crypto.decryptPFS(crypto_keys.dataKey, crypto_keys.tweakKey, pfsc_copy_, pfs_decrypted_tmp, pfsc_buf_size, currentSector); // Decrypt the pfs_image.

			delete[] pfsc_copy_;

			U08* pfs_decrypted_ = new U08[pfsc_buf_size];
			std::memcpy(pfs_decrypted_, pfs_decrypted_tmp + previousData, pfsc_buf_size);

			delete[] pfs_decrypted_tmp;

			FsFile inflated;
			inflated.Open(file_extracted, fsWrite);

			U64 sectorOffsetFile = 0;
			U64 sectorSizeFile = 0;
			int size_decompressed = 0;

			for (int j = 0; j < nblocks; j++)
			{
				QApplication::processEvents(); // not ideal but it helps.

				U64 sectorOffsetTmp = sectorMap[sector_loc + j]; // offset into PFSC_image and not pfs_image.
				U64 sectorSizeTmp = sectorMap[sector_loc + j + 1] - sectorOffsetTmp; // indicates if data is compressed or not.

				if (j == 0)
					sectorOffsetFile = 0;
				else
					sectorOffsetFile += sectorSizeFile;

				sectorSizeFile = sectorSizeTmp;

				char* compressedData = new char[sectorSizeTmp];
				char* decompressedData = new char[0x10000];

				std::memcpy(compressedData, pfs_decrypted_ + sectorOffsetFile, sectorSizeTmp);

				if (sectorSizeTmp == 0x10000) // Uncompressed data
					std::memcpy(decompressedData, compressedData, 0x10000);
				else if (sectorSizeTmp < 0x10000) // Compressed data
					decompress_pfsc(compressedData, sectorSizeTmp, decompressedData, 0x10000);

				size_decompressed += 0x10000;

				if (j < nblocks - 1)
					inflated.Write(decompressedData, 0x10000);
				else
					inflated.Write(decompressedData, 0x10000 - (size_decompressed - bsize)); // This is to remove the zeros.

				delete[] compressedData;
				delete[] decompressedData;
			}
			delete[] pfs_decrypted_;
			inflated.Close();
		}
	}

	delete[] sectorMap;
	fs_table.clear();
	folder_map.clear();
	iNode_buf.clear();

	return true;
}