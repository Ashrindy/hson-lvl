#pragma once

#define NAME_HASH(STRING) ulvl::ut::nameHash(STRING)

namespace ulvl::ut {
    constexpr unsigned int nameHash(const char* str) {
        unsigned int hash{ 0 };
        while (*str)
            hash = *str++ + 31 * hash;
        return hash & 0x7FFFFFFF;
    }

    static inline unsigned int nameHash(const std::string& str) {
        return nameHash(str.c_str());
    }
}
