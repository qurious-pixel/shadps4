#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "common/endian.h"

struct PSFHeader {
    u32_be magic; // big endian
    u32_le version;
    u32_le key_table_offset;
    u32_le data_table_offset;
    u32_le index_table_entries;
};

struct PSFEntry {
    enum Fmt : u16 {
        TEXT_RAW = 0x0400,    // String in UTF-8 format and not NULL terminated
        TEXT_NORMAL = 0x0402, // String in UTF-8 format and NULL terminated
        INTEGER = 0x0404,     // Unsigned 32-bit integer
    };

    u16_le key_offset;
    u16_be param_fmt; // big endian
    u32_le param_len;
    u32_le param_max_len;
    u32_le data_offset;
};

class PSF {
    std::vector<u8> psf;
    std::unordered_map<std::string, std::string> map_strings;
    std::unordered_map<std::string, u32> map_integers;

public:
    PSF();
    ~PSF();
    bool open(const std::string& filepath);

    // Access data
    std::string get_string(const std::string& key);
    u32 get_integer(const std::string& key);
};
