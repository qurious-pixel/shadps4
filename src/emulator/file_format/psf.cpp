#include <fstream>
#include <iostream>

#include "common/io_file.h"
#include "psf.h"

PSF::PSF() {}

PSF::~PSF() {}
bool PSF::open(const std::string& filepath) {
    IOFile file;
    if (!file.open(filepath)) {
        return false;
    }

    const u64 psfSize = file.size().value();
    psf.resize(psfSize);
    file.seek(0);
    file.readBytes(&psf[0], psfSize);

    // Parse file contents
    const auto& header = (PSFHeader&)psf[0];
    for (u32 i = 0; i < header.index_table_entries; i++) {
        const u32 offset = sizeof(PSFHeader) + i * sizeof(PSFEntry);
        auto& entry = (PSFEntry&)psf[offset];

        std::string key = (char*)&psf[header.key_table_offset + entry.key_offset];
        if (entry.param_fmt == PSFEntry::Fmt::TEXT_RAW ||
            entry.param_fmt == PSFEntry::Fmt::TEXT_NORMAL) {
            map_strings[key] = (char*)&psf[header.data_table_offset + entry.data_offset];
        }
        if (entry.param_fmt == PSFEntry::Fmt::INTEGER) {
            map_integers[key] = (u32&)psf[header.data_table_offset + entry.data_offset];
        }
    }

    return true;
}
std::string PSF::get_string(const std::string& key) {
    if (map_strings.find(key) != map_strings.end()) {
        return map_strings.at(key);
    }
    return "";
}
u32 PSF::get_integer(const std::string& key) {
    if (map_integers.find(key) != map_integers.end()) {
        return map_integers.at(key); // TODO std::invalid_argument exception if it fails?
    }
    return 0;
}
