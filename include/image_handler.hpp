#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <cstdio>

namespace fs = std::filesystem;

namespace imgenc {

    // ============================================================================
    // Path and String Utilities (MOVE THIS FIRST)
    // ============================================================================

    inline std::string getFileExtension(const std::string& path) {
        fs::path p(path);
        std::string ext = p.extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        return ext;
    }

    // ============================================================================
    // File and Image Validation
    // ============================================================================

    inline bool isValidImagePath(const std::string& path) {
        if (!fs::exists(path) || !fs::is_regular_file(path)) {
            return false;
        }

        std::string ext = getFileExtension(path);
        static const std::vector<std::string> valid_exts = {
            ".jpg", ".jpeg", ".png", ".bmp", ".gif", ".tiff", ".webp"
        };

        return std::find(valid_exts.begin(), valid_exts.end(), ext) != valid_exts.end();
    }

    inline bool isEncryptedImage(const std::string& path) {
        if (!fs::exists(path) || !fs::is_regular_file(path)) {
            return false;
        }

        std::ifstream file(path, std::ios::binary);
        if (!file) return false;

        char magic[6];
        file.read(magic, 6);
        return std::string(magic, 6) == "IMGENC";
    }

    inline std::string getOutputPath(const std::string& inputPath, bool encrypted = true) {
        fs::path p(inputPath);

        if (encrypted) {
            return p.string() + ".enc";
        }
        else {
            std::string stem = p.stem().string();
            std::string ext = p.extension().string();
            return (p.parent_path() / (stem + "_decrypted" + ext)).string();
        }
    }

    inline std::string formatFileSize(uint64_t sizeBytes) {
        double size = static_cast<double>(sizeBytes);
        const char* units[] = { "B", "KB", "MB", "GB", "TB" };
        int unit_index = 0;

        while (size >= 1024.0 && unit_index < 4) {
            size /= 1024.0;
            unit_index++;
        }

        char buffer[32];
        std::snprintf(buffer, sizeof(buffer), "%.2f %s", size, units[unit_index]);
        return std::string(buffer);
    }

    inline bool fileExists(const std::string& path) {
        return fs::exists(path) && fs::is_regular_file(path);
    }

    inline uint64_t getFileSize(const std::string& path) {
        if (!fs::exists(path)) {
            return 0;
        }
        return fs::file_size(path);
    }

    inline bool createDirectoryIfNotExists(const std::string& path) {
        if (fs::exists(path)) {
            return fs::is_directory(path);
        }
        try {
            fs::create_directories(path);
            return true;
        }
        catch (const fs::filesystem_error&) {
            return false;
        }
    }

    inline std::string getAbsolutePath(const std::string& path) {
        return fs::absolute(path).string();
    }

} // namespace imgenc