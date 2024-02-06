#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "../../common/common.h" //TODO fix me!
#include "../../common/types.h"  //TODO fix me!

struct PSFHeader {
    BE<u32> magic; // big endian
    LE<u32> version;
    LE<u32> key_table_offset;
    LE<u32> data_table_offset;
    LE<u32> index_table_entries;
};

struct PSFEntry {
    enum Fmt : u16 {
        TEXT_RAW = 0x0400,    // String in UTF-8 format and not NULL terminated
        TEXT_NORMAL = 0x0402, // String in UTF-8 format and NULL terminated
        INTEGER = 0x0404,     // Unsigned 32-bit integer
    };

    LE<u16> key_offset;
    BE<u16> param_fmt; // big endian
    LE<u32> param_len;
    LE<u32> param_max_len;
    LE<u32> data_offset;
};

class PSF {
    std::vector<u08> psf;
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
