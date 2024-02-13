#pragma once

#include <string_view>
#include "common/types.h"

/// Retrieves the PKG entry name from its type identifier.
std::string_view GetEntryNameByType(u32 type);
