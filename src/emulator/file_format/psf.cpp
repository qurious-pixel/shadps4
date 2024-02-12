#include <cstring>
#include <fstream>
#include <iostream>

#include "common/io_file.h"
#include "psf.h"

PSF::PSF() = default;

PSF::~PSF() = default;

bool PSF::open(const std::string& filepath) {
    IOFile file(filepath);
    if (!file.isOpen()) {
        return false;
    }

    const u64 psfSize = file.size().value();
    psf.resize(psfSize);
    file.seek(0);
    file.readBytes(&psf[0], psfSize);

    // Parse file contents
    PSFHeader header;
    std::memcpy(&header, psf.data(), sizeof(header));
    for (u32 i = 0; i < header.index_table_entries; i++) {
        PSFEntry entry;
        std::memcpy(&entry, &psf[sizeof(PSFHeader) + i * sizeof(PSFEntry)], sizeof(entry));

        const std::string key = (char*)&psf[header.key_table_offset + entry.key_offset];
        if (entry.param_fmt == PSFEntry::Fmt::TextRaw ||
            entry.param_fmt == PSFEntry::Fmt::TextNormal) {
            map_strings[key] = (char*)&psf[header.data_table_offset + entry.data_offset];
        }
        if (entry.param_fmt == PSFEntry::Fmt::Integer) {
            u32 value;
            std::memcpy(&value, &psf[header.data_table_offset + entry.data_offset], sizeof(value));
            map_integers[key] = value;
        }
    }

    return true;
}

std::string PSF::GetString(const std::string& key) {
    if (map_strings.find(key) != map_strings.end()) {
        return map_strings.at(key);
    }
    return "";
}

u32 PSF::GetInteger(const std::string& key) {
    if (map_integers.find(key) != map_integers.end()) {
        return map_integers.at(key); // TODO std::invalid_argument exception if it fails?
    }
    return 0;
}
