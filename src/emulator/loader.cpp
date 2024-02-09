#include "../common/io_file.h"
#include "../common/types.h"
#include "loader.h"

FileTypes detectFileType(const std::string& filepath) {
    if (filepath.size() == 0) // no file loaded
    {
        return FILETYPE_UNKNOWN;
    }
    IOFile file;
    file.open(filepath);
    file.seek(0);
    u32 magic;
    file.readBytes(&magic, sizeof(magic));
    file.close();
    switch (magic) {
    case 0x544e437f: // PS4 PKG
        return FILETYPE_PKG;
    }
    return FILETYPE_UNKNOWN;
}
