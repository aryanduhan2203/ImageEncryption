/**
 * @file main.cpp
 * @brief ImageEncryption - Complete implementation and CLI
 * 
 * Combines:
 * - AES-256 encryption/decryption with OpenSSL
 * - Logistic chaotic map pixel permutation
 * - Image I/O with OpenCV
 * - Command-line interface
 */

#include "image_encryption.hpp"
#include "image_handler.hpp"

#include <opencv2/opencv.hpp>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/kdf.h>

#include <iostream>
#include <cstring>
#include <getopt.h>

// ============================================================================
// ImageEncryption Implementation
// ============================================================================

namespace imgenc {

ImageEncryption::ImageEncryption(const std::string& password,
                                 double r,
                                 const std::vector<uint8_t>& salt)
    : password(password), r(r), salt(salt) {
    if (this->salt.empty()) {
        this->salt = generateRandomBytes(SALT_SIZE);
    }
    deriveKey();
}

std::vector<uint8_t> ImageEncryption::generateRandomBytes(size_t length) {
    std::vector<uint8_t> buffer(length);
    if (RAND_bytes(buffer.data(), length) != 1) {
        throw std::runtime_error("Failed to generate random bytes");
    }
    return buffer;
}

void ImageEncryption::deriveKey() {
    key.resize(KEY_SIZE);
    
    int ret = PKCS5_PBKDF2_HMAC(
        password.c_str(),
        password.length(),
        salt.data(),
        salt.size(),
        PBKDF2_ITERATIONS,
        EVP_sha256(),
        KEY_SIZE,
        key.data()
    );
    
    if (ret != 1) {
        throw std::runtime_error("Failed to derive key from password");
    }
}

std::vector<uint8_t> ImageEncryption::aesEncrypt(
    const std::vector<uint8_t>& plaintext,
    const std::vector<uint8_t>& iv) {
    
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) throw std::runtime_error("Failed to create cipher context");
    
    std::vector<uint8_t> ciphertext(plaintext.size() + EVP_MAX_BLOCK_LENGTH);
    int len = 0, ciphertext_len = 0;
    
    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key.data(), iv.data()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Failed to initialize encryption");
    }
    
    if (EVP_EncryptUpdate(ctx, ciphertext.data(), &len, plaintext.data(), plaintext.size()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Failed to encrypt data");
    }
    ciphertext_len = len;
    
    if (EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Failed to finalize encryption");
    }
    ciphertext_len += len;
    EVP_CIPHER_CTX_free(ctx);
    
    ciphertext.resize(ciphertext_len);
    return ciphertext;
}

std::vector<uint8_t> ImageEncryption::aesDecrypt(
    const std::vector<uint8_t>& ciphertext,
    const std::vector<uint8_t>& iv) {
    
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) throw std::runtime_error("Failed to create cipher context");
    
    std::vector<uint8_t> plaintext(ciphertext.size() + EVP_MAX_BLOCK_LENGTH);
    int len = 0, plaintext_len = 0;
    
    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key.data(), iv.data()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Failed to initialize decryption");
    }
    
    if (EVP_DecryptUpdate(ctx, plaintext.data(), &len, ciphertext.data(), ciphertext.size()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Failed to decrypt data");
    }
    plaintext_len = len;
    
    if (EVP_DecryptFinal_ex(ctx, plaintext.data() + len, &len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Wrong password or corrupted file");
    }
    plaintext_len += len;
    EVP_CIPHER_CTX_free(ctx);
    
    plaintext.resize(plaintext_len);
    return plaintext;
}

std::vector<uint8_t> ImageEncryption::addPadding(const std::vector<uint8_t>& data) {
    int padding_length = EVP_MAX_BLOCK_LENGTH - (data.size() % EVP_MAX_BLOCK_LENGTH);
    std::vector<uint8_t> padded = data;
    padded.insert(padded.end(), padding_length, static_cast<uint8_t>(padding_length));
    return padded;
}

std::vector<uint8_t> ImageEncryption::removePadding(const std::vector<uint8_t>& data) {
    if (data.empty()) return data;
    int padding_length = data.back();
    if (padding_length > EVP_MAX_BLOCK_LENGTH || padding_length == 0) return data;
    return std::vector<uint8_t>(data.begin(), data.end() - padding_length);
}

std::vector<uint8_t> ImageEncryption::permutePixels(
    const std::vector<uint8_t>& pixelData,
    bool encrypt,
    size_t size) {
    
    LogisticChaoticMap chaotic(r, 0.123456);
    auto permutation = chaotic.getPermutation(size);
    
    std::vector<uint8_t> permuted(pixelData.size());
    
    if (encrypt) {
        for (size_t i = 0; i < size; ++i) {
            permuted[i] = pixelData[permutation[i]];
        }
    } else {
        for (size_t i = 0; i < size; ++i) {
            permuted[permutation[i]] = pixelData[i];
        }
    }
    
    return permuted;
}

EncryptionMetadata ImageEncryption::encryptImage(
    const std::string& inputPath,
    const std::string& outputPath) {
    
    cv::Mat image = cv::imread(inputPath, cv::IMREAD_COLOR);
    if (image.empty()) {
        throw std::runtime_error("Failed to load image: " + inputPath);
    }
    
    cv::cvtColor(image, image, cv::COLOR_BGR2RGB);
    
    EncryptionMetadata metadata;
    metadata.height = image.rows;
    metadata.width = image.cols;
    metadata.channels = image.channels();
    metadata.salt = salt;
    metadata.iv = generateRandomBytes(IV_SIZE);
    metadata.originalMode = "RGB";
    
    std::vector<uint8_t> pixelData(image.data, image.data + image.total() * image.channels());
    
    auto permuted = permutePixels(pixelData, true, pixelData.size());
    auto padded = addPadding(permuted);
    auto encrypted = aesEncrypt(padded, metadata.iv);
    
    saveEncryptedImage(outputPath, encrypted, metadata);
    
    return metadata;
}

bool ImageEncryption::decryptImage(const std::string& inputPath,
                                   const std::string& outputPath) {
    
    EncryptionMetadata metadata;
    auto encrypted = loadEncryptedImage(inputPath, metadata);
    
    salt = metadata.salt;
    deriveKey();
    
    auto padded = aesDecrypt(encrypted, metadata.iv);
    auto permuted = removePadding(padded);
    auto decrypted = permutePixels(permuted, false, permuted.size());
    
    cv::Mat image(metadata.height, metadata.width, CV_8UC3, decrypted.data());
    cv::cvtColor(image, image, cv::COLOR_RGB2BGR);
    
    if (!cv::imwrite(outputPath, image)) {
        throw std::runtime_error("Failed to save decrypted image: " + outputPath);
    }
    
    return true;
}

void ImageEncryption::saveEncryptedImage(
    const std::string& path,
    const std::vector<uint8_t>& encryptedData,
    const EncryptionMetadata& metadata) {
    
    std::ofstream file(path, std::ios::binary);
    if (!file) throw std::runtime_error("Failed to open output file: " + path);
    
    file.write(MAGIC_NUMBER, std::strlen(MAGIC_NUMBER));
    file.write(reinterpret_cast<char*>(const_cast<uint8_t*>(&FILE_VERSION)), 1);
    
    uint8_t salt_len = metadata.salt.size();
    file.write(reinterpret_cast<char*>(&salt_len), 1);
    file.write(reinterpret_cast<const char*>(metadata.salt.data()), metadata.salt.size());
    
    uint8_t iv_len = metadata.iv.size();
    file.write(reinterpret_cast<char*>(&iv_len), 1);
    file.write(reinterpret_cast<const char*>(metadata.iv.data()), metadata.iv.size());
    
    uint32_t height = metadata.height;
    uint32_t width = metadata.width;
    uint8_t channels = metadata.channels;
    
    file.write(reinterpret_cast<char*>(&height), sizeof(height));
    file.write(reinterpret_cast<char*>(&width), sizeof(width));
    file.write(reinterpret_cast<char*>(&channels), sizeof(channels));
    
    uint8_t mode_len = metadata.originalMode.length();
    file.write(reinterpret_cast<char*>(&mode_len), 1);
    file.write(metadata.originalMode.c_str(), metadata.originalMode.length());
    
    uint64_t data_len = encryptedData.size();
    file.write(reinterpret_cast<char*>(&data_len), sizeof(data_len));
    file.write(reinterpret_cast<const char*>(encryptedData.data()), encryptedData.size());
    
    file.close();
}

std::vector<uint8_t> ImageEncryption::loadEncryptedImage(
    const std::string& path,
    EncryptionMetadata& metadata) {
    
    std::ifstream file(path, std::ios::binary);
    if (!file) throw std::runtime_error("Failed to open encrypted file: " + path);
    
    char magic[6];
    file.read(magic, 6);
    if (std::string(magic, 6) != MAGIC_NUMBER) {
        throw std::runtime_error("Invalid encrypted file format");
    }
    
    uint8_t version;
    file.read(reinterpret_cast<char*>(&version), 1);
    if (version != FILE_VERSION) {
        throw std::runtime_error("Unsupported file version");
    }
    
    uint8_t salt_len;
    file.read(reinterpret_cast<char*>(&salt_len), 1);
    metadata.salt.resize(salt_len);
    file.read(reinterpret_cast<char*>(metadata.salt.data()), salt_len);
    
    uint8_t iv_len;
    file.read(reinterpret_cast<char*>(&iv_len), 1);
    metadata.iv.resize(iv_len);
    file.read(reinterpret_cast<char*>(metadata.iv.data()), iv_len);
    
    uint32_t height, width;
    uint8_t channels;
    file.read(reinterpret_cast<char*>(&height), sizeof(height));
    file.read(reinterpret_cast<char*>(&width), sizeof(width));
    file.read(reinterpret_cast<char*>(&channels), sizeof(channels));
    
    metadata.height = height;
    metadata.width = width;
    metadata.channels = channels;
    
    uint8_t mode_len;
    file.read(reinterpret_cast<char*>(&mode_len), 1);
    std::vector<char> mode_buffer(mode_len);
    file.read(mode_buffer.data(), mode_len);
    metadata.originalMode = std::string(mode_buffer.begin(), mode_buffer.end());
    
    uint64_t data_len;
    file.read(reinterpret_cast<char*>(&data_len), sizeof(data_len));
    
    std::vector<uint8_t> encrypted_data(data_len);
    file.read(reinterpret_cast<char*>(encrypted_data.data()), data_len);
    file.close();
    
    return encrypted_data;
}

} // namespace imgenc

// ============================================================================
// CLI Application
// ============================================================================

void printUsage(const char* program) {
    std::cout << "\n" << std::string(60, '=') << "\n"
              << "ImageEncryption - AES-256 with Logistic Chaotic Map\n"
              << std::string(60, '=') << "\n\n"
              << "Usage: " << program << " <command> [options]\n\n"
              << "Commands:\n"
              << "  encrypt <input>     Encrypt an image\n"
              << "  decrypt <input>     Decrypt an image\n"
              << "  help                Show this help message\n\n"
              << "Options:\n"
              << "  -o, --output PATH   Output file path\n"
              << "  -p, --password PASS Encryption/decryption password\n"
              << "  -r, --r VALUE       Chaotic map parameter (default: 3.99)\n\n"
              << "Examples:\n"
              << "  " << program << " encrypt photo.jpg -p \"Password123\"\n"
              << "  " << program << " decrypt photo.enc -p \"Password123\" -o decrypted.jpg\n"
              << std::string(60, '=') << "\n\n";
}

int encryptCommand(int argc, char* argv[]) {
    std::string input = argv[2];
    std::string output, password;
    double r = 3.99;
    
    for (int i = 3; i < argc; ++i) {
        std::string arg = argv[i];
        if ((arg == "-o" || arg == "--output") && i + 1 < argc) {
            output = argv[++i];
        } else if ((arg == "-p" || arg == "--password") && i + 1 < argc) {
            password = argv[++i];
        } else if ((arg == "-r" || arg == "--r") && i + 1 < argc) {
            r = std::stod(argv[++i]);
        }
    }
    
    if (!imgenc::isValidImagePath(input)) {
        std::cerr << "Error: Invalid image file '" << input << "'\n";
        return 1;
    }
    
    if (output.empty()) {
        output = imgenc::getOutputPath(input, true);
    }
    
    if (password.empty()) {
        std::cout << "Enter encryption password: ";
        std::getline(std::cin, password);
    }
    
    try {
        imgenc::ImageEncryption encryptor(password, r);
        
        std::cout << "\nEncrypting: " << input << "\n";
        std::cout << "Output: " << output << "\n";
        
        auto metadata = encryptor.encryptImage(input, output);
        
        auto input_size = imgenc::getFileSize(input);
        auto output_size = imgenc::getFileSize(output);
        
        std::cout << "✓ Success!\n"
                  << "  Input: " << imgenc::formatFileSize(input_size) << "\n"
                  << "  Output: " << imgenc::formatFileSize(output_size) << "\n"
                  << "  Size: " << metadata.width << "x" << metadata.height << "\n\n";
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n\n";
        return 1;
    }
}

int decryptCommand(int argc, char* argv[]) {
    std::string input = argv[2];
    std::string output, password;
    
    for (int i = 3; i < argc; ++i) {
        std::string arg = argv[i];
        if ((arg == "-o" || arg == "--output") && i + 1 < argc) {
            output = argv[++i];
        } else if ((arg == "-p" || arg == "--password") && i + 1 < argc) {
            password = argv[++i];
        }
    }
    
    if (!imgenc::fileExists(input)) {
        std::cerr << "Error: File not found '" << input << "'\n";
        return 1;
    }
    
    if (!imgenc::isEncryptedImage(input)) {
        std::cerr << "Error: Not a valid encrypted image\n";
        return 1;
    }
    
    if (output.empty()) {
        output = imgenc::getOutputPath(input, false);
    }
    
    if (password.empty()) {
        std::cout << "Enter decryption password: ";
        std::getline(std::cin, password);
    }
    
    try {
        imgenc::ImageEncryption decryptor(password);
        
        std::cout << "\nDecrypting: " << input << "\n";
        std::cout << "Output: " << output << "\n";
        
        decryptor.decryptImage(input, output);
        
        auto input_size = imgenc::getFileSize(input);
        auto output_size = imgenc::getFileSize(output);
        
        std::cout << "✓ Success!\n"
                  << "  Input: " << imgenc::formatFileSize(input_size) << "\n"
                  << "  Output: " << imgenc::formatFileSize(output_size) << "\n\n";
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n\n";
        return 1;
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }
    
    std::string command = argv[1];
    
    if (command == "encrypt") {
        if (argc < 3) {
            std::cerr << "Error: encrypt requires input file\n";
            printUsage(argv[0]);
            return 1;
        }
        return encryptCommand(argc, argv);
    } else if (command == "decrypt") {
        if (argc < 3) {
            std::cerr << "Error: decrypt requires input file\n";
            printUsage(argv[0]);
            return 1;
        }
        return decryptCommand(argc, argv);
    } else if (command == "help" || command == "-h" || command == "--help") {
        printUsage(argv[0]);
        return 0;
    } else {
        std::cerr << "Error: Unknown command '" << command << "'\n";
        printUsage(argv[0]);
        return 1;
    }
}
