/**
 * @file image_encryption.hpp
 * @brief Image encryption and decryption using AES-256 with Logistic Chaotic Map
 * 
 * Main header file containing:
 * - LogisticChaoticMap for pixel permutation
 * - ImageEncryption for AES-256 encryption/decryption
 * - Configuration constants
 */

#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <algorithm>
#include <numeric>
#include <cmath>

namespace imgenc {

// ============================================================================
// Configuration Constants
// ============================================================================

constexpr int KEY_SIZE = 32;                    // 256 bits for AES-256
constexpr int SALT_SIZE = 16;                  // 128 bits
constexpr int IV_SIZE = 16;                    // 128 bits
constexpr int PBKDF2_ITERATIONS = 100000;
constexpr double DEFAULT_CHAOTIC_PARAMETER = 3.99;
constexpr const char* MAGIC_NUMBER = "IMGENC";
constexpr uint8_t FILE_VERSION = 0x01;

// ============================================================================
// Logistic Chaotic Map Class
// ============================================================================

/**
 * @class LogisticChaoticMap
 * @brief Logistic chaotic map implementation for pseudo-random pixel permutation
 * 
 * Implements the logistic map equation: x(n+1) = r * x(n) * (1 - x(n))
 * Used for generating pixel permutations in image encryption.
 * 
 * Example:
 * @code
 * LogisticChaoticMap chaotic(3.99, 0.1);
 * auto permutation = chaotic.getPermutation(1000);
 * @endcode
 */
class LogisticChaoticMap {
public:
    /**
     * @brief Constructor
     * @param r The parameter r for the logistic map (typically 3.57-4.0 for chaotic behavior)
     * @param x0 Initial value (typically between 0 and 1)
     */
    LogisticChaoticMap(double r = 3.99, double x0 = 0.1)
        : r(r), x0(x0), x(x0) {}

    /**
     * @brief Generate a sequence of chaotic values
     * @param length Length of sequence to generate
     * @return Vector of chaotic values (each between 0 and 1)
     */
    std::vector<double> generateSequence(size_t length) {
        std::vector<double> sequence(length);
        double currentX = x0;
        
        for (size_t i = 0; i < length; ++i) {
            currentX = r * currentX * (1.0 - currentX);
            sequence[i] = currentX;
        }
        
        x = currentX;
        return sequence;
    }

    /**
     * @brief Generate a permutation based on chaotic sequence
     * @param size Size of the permutation array
     * @return Vector of permutation indices
     */
    std::vector<size_t> getPermutation(size_t size) {
        auto sequence = generateSequence(size);
        
        std::vector<size_t> indices(size);
        std::iota(indices.begin(), indices.end(), 0);
        
        std::sort(indices.begin(), indices.end(),
            [&sequence](size_t a, size_t b) {
                return sequence[a] < sequence[b];
            });
        
        return indices;
    }

    /**
     * @brief Reset the chaotic map to initial state
     * @param newX0 New initial value (optional)
     */
    void reset(double newX0 = -1.0) {
        if (newX0 >= 0.0) x0 = newX0;
        x = x0;
    }

    /**
     * @brief Get current value in the sequence
     * @return Current x value
     */
    double getCurrentValue() const { return x; }

    /**
     * @brief Set the r parameter
     * @param newR New r value
     */
    void setR(double newR) { r = newR; }

    /**
     * @brief Get the r parameter
     * @return Current r value
     */
    double getR() const { return r; }

private:
    double r;                          ///< Logistic map parameter
    double x0;                         ///< Initial value
    double x;                          ///< Current value
    std::vector<double> sequence;      ///< Stored sequence
};

// ============================================================================
// Encryption Metadata Structure
// ============================================================================

/**
 * @struct EncryptionMetadata
 * @brief Metadata for encrypted images
 */
struct EncryptionMetadata {
    std::vector<uint8_t> salt;      ///< Salt used for key derivation
    std::vector<uint8_t> iv;        ///< Initialization vector for AES
    size_t height;                  ///< Original image height
    size_t width;                   ///< Original image width
    size_t channels;                ///< Number of color channels
    std::string originalMode;       ///< Original image mode/format
};

// ============================================================================
// Image Encryption Class
// ============================================================================

/**
 * @class ImageEncryption
 * @brief Main class for secure image encryption and decryption
 * 
 * Provides secure image encryption using:
 * - AES-256 in CBC mode (via OpenSSL)
 * - PBKDF2 key derivation (100,000 iterations)
 * - Logistic chaotic map pixel permutation
 * - Secure random IV and salt generation
 * 
 * Example:
 * @code
 * // Encrypt
 * ImageEncryption enc("MyPassword123");
 * auto meta = enc.encryptImage("photo.jpg", "photo.enc");
 * 
 * // Decrypt
 * ImageEncryption dec("MyPassword123", 3.99, meta.salt);
 * dec.decryptImage("photo.enc", "decrypted.jpg");
 * @endcode
 */
class ImageEncryption {
public:
    /**
     * @brief Constructor
     * @param password Password for encryption/decryption
     * @param r Chaotic map parameter (default: 3.99)
     * @param salt Optional salt for key derivation (auto-generated if empty)
     * @throws std::runtime_error if key derivation fails
     */
    ImageEncryption(const std::string& password, 
                    double r = 3.99, 
                    const std::vector<uint8_t>& salt = {});

    /**
     * @brief Encrypt an image file
     * @param inputPath Path to input image
     * @param outputPath Path to save encrypted image
     * @return Encryption metadata (contains salt and IV needed for decryption)
     * @throws std::runtime_error on failure (invalid image, I/O error, etc.)
     * 
     * Process:
     * 1. Load image from file
     * 2. Permute pixels using chaotic map
     * 3. Encrypt pixel data with AES-256
     * 4. Save encrypted data with metadata
     */
    EncryptionMetadata encryptImage(const std::string& inputPath,
                                    const std::string& outputPath);

    /**
     * @brief Decrypt an image file
     * @param inputPath Path to encrypted image
     * @param outputPath Path to save decrypted image
     * @return True if successful
     * @throws std::runtime_error on failure (wrong password, corrupted file, etc.)
     * 
     * Process:
     * 1. Load encrypted data with metadata
     * 2. Decrypt pixel data with AES-256
     * 3. Apply inverse pixel permutation
     * 4. Save decrypted image to file
     */
    bool decryptImage(const std::string& inputPath,
                     const std::string& outputPath);

    /**
     * @brief Get the salt used for key derivation
     * @return Reference to salt vector (needed for decryption)
     */
    const std::vector<uint8_t>& getSalt() const { return salt; }

private:
    std::string password;
    double r;
    std::vector<uint8_t> salt;
    std::vector<uint8_t> key;

    void deriveKey();
    std::vector<uint8_t> generateRandomBytes(size_t length);
    std::vector<uint8_t> permutePixels(const std::vector<uint8_t>& pixelData,
                                       bool encrypt, size_t size);
    std::vector<uint8_t> aesEncrypt(const std::vector<uint8_t>& plaintext,
                                    const std::vector<uint8_t>& iv);
    std::vector<uint8_t> aesDecrypt(const std::vector<uint8_t>& ciphertext,
                                    const std::vector<uint8_t>& iv);
    void saveEncryptedImage(const std::string& path,
                           const std::vector<uint8_t>& encryptedData,
                           const EncryptionMetadata& metadata);
    std::vector<uint8_t> loadEncryptedImage(const std::string& path,
                                            EncryptionMetadata& metadata);
    std::vector<uint8_t> addPadding(const std::vector<uint8_t>& data);
    std::vector<uint8_t> removePadding(const std::vector<uint8_t>& data);
};

} // namespace imgenc
