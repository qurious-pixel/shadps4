#pragma once

#include "endianness.h"

/**
 * Endianness
 */
template <typename T>
using LE = NativeEndian<T>;
template <typename T>
using BE = SwappedEndian<T>;