#pragma once

#if defined(_WIN32)
#include <windows.h>
#else
#include <unistd.h>
#endif

#define NOMINMAX

#include <vector>
#include <memory>
#include <string>
#include <filesystem>

#include "log.h"
#include "utilities/namehash.h"

#define ARRAY_SIZE(array) sizeof(array) / sizeof(array[0])

#define STR(x) #x
#define XSTR(x) STR(x)

inline std::filesystem::path getExeDir() {
    static std::filesystem::path exeDir;

    if (exeDir.empty()) {
#if defined(_WIN32)
        char buffer[MAX_PATH];
        GetModuleFileNameA(NULL, buffer, MAX_PATH);
        exeDir = std::filesystem::path{ buffer }.parent_path();
#else
        char buffer[1024];
        ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
        if (len != -1) {
            buffer[len] = '\0';
            exeDir = std::filesystem::path{ buffer }.parent_path();
        }
#endif
    }

    return exeDir;
}

inline std::filesystem::path rel_to_exe(const std::filesystem::path& relativePath) {
    return getExeDir() / relativePath;
}
