#ifndef P_COMMON_H
#define P_COMMON_H

#include <string>
#include <cstdint>

#include "utf8/utf8.h"

using u8str = std::basic_string<uint8_t>;
using u32str = std::basic_string<char32_t>;

inline std::string u32_to_string(u32str str) {
    std::string result;
    utf8::utf32to8(str.begin(), str.end(), std::back_inserter(result));
    return result;
}

#endif
