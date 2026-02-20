/**
 * @file image_handler.hpp
 * @brief Image I/O and utility functions
 * 
 * Provides utilities for:
 * - File validation and handling
 * - Image format checking
 * - Path manipulation
 * - File size formatting
 */

#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

namespace imgenc {

// ============================================================================
// File and Image Validation
// ============================================================================

/**
 * @brief Validate if file is a valid image
 * @param path Path to file
 * @return True if file exists and has supported image extension
 * 
 * Supported formats: .jpg, .jpeg, .png, .bmp, .gif, .tiff, .webp
 */
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

/**
 * @brief Check if file is encrypted
 * @param path Path to file
 * @return True if file has IMGENC magic number (is encrypted image)
 */
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

// ============================================================================
// Path and String Utilities
// ============================================================================

/**
 * @brief Get file extension (lowercase)
 * @param path File path
 * @return File extension (e.g., ".jpg")
 */
inline std::string getFileExtension(const std::string& path) {
    fs::path p(path);
    std::string ext = p.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext;
}

/**
 * @brief Generate output path for encrypted/decrypted image
 * @param inputPath Input file path
 * @param encrypted If true, adds .enc extension; otherwise adds _decrypted suffix
 * @return Generated output path
 * 
 * Example:
 * - input: "photo.jpg", encrypted=true  -> "photo.jpg.enc"
 * - input: "photo.enc", encrypted=false -> "photo_decrypted.jpg"
 */
inline std::string getOutputPath(const std::string& inputPath, bool encrypted = true) {
    fs::path p(inputPath);
    
    if (encrypted) {
        return p.string() + ".enc";
    } else {
        std::string stem = p.stem().string();
        std::string ext = p.extension().string();
        return (p.parent_path() / (stem + "_decrypted" + ext)).string();
    }
}

/**
 * @brief Format file size in human-readable format
 * @param sizeBytes Size in bytes
 * @return Formatted size string (e.g., "2.5 MB")
 * 
 * Example:
 * - 1024 bytes -> "1.00 KB"
 * - 1048576 bytes -> "1.00 MB"
 * - 1073741824 bytes -> "1.00 GB"
 */
inline std::string formatFileSize(uint64_t sizeBytes) {
    double size = static_cast<double>(sizeBytes);
    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    int unit_index = 0;
    
    while (size >= 1024.0 && unit_index < 4) {
        size /= 1024.0;
        unit_index++;
    }
    
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%.2f %s", size, units[unit_index]);
    return std::string(buffer);
}

// ============================================================================
// File System Operations
// ============================================================================

/**
 * @brief Check if file exists
 * @param path File path
 * @return True if file exists
 */
inline bool fileExists(const std::string& path) {
    return fs::exists(path) && fs::is_regular_file(path);
}

/**
 * @brief Get file size in bytes
 * @param path File path
 * @return File size in bytes, or 0 if file not found
 */
inline uint64_t getFileSize(const std::string& path) {
    if (!fs::exists(path)) {
        return 0;
    }
    return fs::file_size(path);
}

/**
 * @brief Create directory if it doesn't exist
 * @param path Directory path
 * @return True if directory was created or already exists
 */
inline bool createDirectoryIfNotExists(const std::string& path) {
    if (fs::exists(path)) {
        return fs::is_directory(path);
    }
    try {
        fs::create_directories(path);
        return true;
    } catch (const fs::filesystem_error&) {
        return false;
    }
}

/**
 * @brief Get absolute path
 * @param path Relative or absolute path
 * @return Absolute path
 */
inline std::string getAbsolutePath(const std::string& path) {
    return fs::absolute(path).string();
}

} // namespace imgenc
