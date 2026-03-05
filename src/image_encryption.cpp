#include "image_encryption.hpp"

#include <opencv2/opencv.hpp>
#include <openssl/evp.h>
#include <openssl/rand.h>

#include <fstream>
#include <stdexcept>
#include <vector>
#include <algorithm>
#include <utility>

namespace imgenc {

    // ============================================================
    // 🔐 Chaotic Helper Functions (STATIC - no header conflict)
    // ============================================================

    static std::vector<double> chaoticSequence(size_t size, double r, double x0)
    {
        std::vector<double> seq(size);
        double x = x0;

        for (size_t i = 0; i < size; ++i) {
            x = r * x * (1.0 - x);
            seq[i] = x;
        }

        return seq;
    }

    static std::vector<size_t> chaoticPermutation(size_t size, double r, double x0)
    {
        auto chaos = chaoticSequence(size, r, x0);

        std::vector<std::pair<double, size_t>> indexed(size);

        for (size_t i = 0; i < size; ++i)
            indexed[i] = { chaos[i], i };

        std::sort(indexed.begin(), indexed.end());

        std::vector<size_t> perm(size);

        for (size_t i = 0; i < size; ++i)
            perm[i] = indexed[i].second;

        return perm;
    }

    static std::vector<uint8_t> applyPermutation(
        const std::vector<uint8_t>& data,
        const std::vector<size_t>& perm)
    {
        std::vector<uint8_t> result(data.size());

        for (size_t i = 0; i < data.size(); ++i)
            result[i] = data[perm[i]];

        return result;
    }

    static std::vector<uint8_t> reversePermutation(
        const std::vector<uint8_t>& data,
        const std::vector<size_t>& perm)
    {
        std::vector<uint8_t> result(data.size());

        for (size_t i = 0; i < data.size(); ++i)
            result[perm[i]] = data[i];

        return result;
    }

    // ============================================================
    // Constructor
    // ============================================================

    ImageEncryption::ImageEncryption(const std::string& password,
        double r,
        const std::vector<uint8_t>& salt)
        : password(password), r(r), salt(salt)
    {
        if (this->salt.empty())
            this->salt = generateRandomBytes(SALT_SIZE);

        deriveKey();
    }

    // ============================================================
    // Random Bytes
    // ============================================================

    std::vector<uint8_t> ImageEncryption::generateRandomBytes(size_t length)
    {
        std::vector<uint8_t> buffer(length);

        if (RAND_bytes(buffer.data(), static_cast<int>(length)) != 1)
            throw std::runtime_error("Random generation failed");

        return buffer;
    }

    // ============================================================
    // Key Derivation
    // ============================================================

    void ImageEncryption::deriveKey()
    {
        key.resize(KEY_SIZE);

        if (!PKCS5_PBKDF2_HMAC(
            password.c_str(),
            static_cast<int>(password.length()),
            salt.data(),
            static_cast<int>(salt.size()),
            PBKDF2_ITERATIONS,
            EVP_sha256(),
            KEY_SIZE,
            key.data()))
        {
            throw std::runtime_error("Key derivation failed");
        }
    }

    // ============================================================
    // AES Encrypt
    // ============================================================

    std::vector<uint8_t> ImageEncryption::aesEncrypt(
        const std::vector<uint8_t>& plaintext,
        const std::vector<uint8_t>& iv)
    {
        EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
        if (!ctx)
            throw std::runtime_error("Failed to create cipher context");

        std::vector<uint8_t> ciphertext(plaintext.size() + EVP_MAX_BLOCK_LENGTH);

        int len = 0;
        int total = 0;

        EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key.data(), iv.data());
        EVP_EncryptUpdate(ctx, ciphertext.data(), &len,
            plaintext.data(), static_cast<int>(plaintext.size()));
        total = len;

        EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len);
        total += len;

        EVP_CIPHER_CTX_free(ctx);

        ciphertext.resize(total);
        return ciphertext;
    }

    // ============================================================
    // AES Decrypt
    // ============================================================

    std::vector<uint8_t> ImageEncryption::aesDecrypt(
        const std::vector<uint8_t>& ciphertext,
        const std::vector<uint8_t>& iv)
    {
        EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
        if (!ctx)
            throw std::runtime_error("Failed to create cipher context");

        std::vector<uint8_t> plaintext(ciphertext.size());

        int len = 0;
        int total = 0;

        EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key.data(), iv.data());
        EVP_DecryptUpdate(ctx, plaintext.data(), &len,
            ciphertext.data(), static_cast<int>(ciphertext.size()));
        total = len;

        if (!EVP_DecryptFinal_ex(ctx, plaintext.data() + len, &len)) {
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("Wrong password or corrupted file");
        }

        total += len;
        EVP_CIPHER_CTX_free(ctx);

        plaintext.resize(total);
        return plaintext;
    }

    // ============================================================
    // Encrypt Image (Permutation + AES)
    // ============================================================

    EncryptionMetadata ImageEncryption::encryptImage(
        const std::string& inputPath,
        const std::string& outputPath)
    {
        cv::Mat image = cv::imread(inputPath, cv::IMREAD_COLOR);
        if (image.empty())
            throw std::runtime_error("Image load failed");

        int width = image.cols;
        int height = image.rows;
        int channels = image.channels();

        std::vector<uint8_t> pixelData(
            image.data,
            image.data + image.total() * channels);

        // ---- Chaotic Permutation ----
        double chaotic_r = 3.99;
        double chaotic_x0 = 0.5;

        auto perm = chaoticPermutation(pixelData.size(), chaotic_r, chaotic_x0);
        auto permuted = applyPermutation(pixelData, perm);

        // ================= TEMPORARY: Save Chaotic Permuted Image =================
        cv::Mat noisy(height, width, CV_8UC3, permuted.data());
        cv::imwrite("noise.jpg", noisy);
        // ========================================================================

        // ---- AES ----
        std::vector<uint8_t> iv = generateRandomBytes(IV_SIZE);
        auto encryptedData = aesEncrypt(permuted, iv);

        std::ofstream file(outputPath, std::ios::binary);
        if (!file)
            throw std::runtime_error("Failed to open output file");

        file.write(reinterpret_cast<char*>(salt.data()), SALT_SIZE);
        file.write(reinterpret_cast<char*>(iv.data()), IV_SIZE);
        file.write(reinterpret_cast<char*>(&width), sizeof(int));
        file.write(reinterpret_cast<char*>(&height), sizeof(int));
        file.write(reinterpret_cast<char*>(&channels), sizeof(int));

        file.write(reinterpret_cast<char*>(encryptedData.data()),
            encryptedData.size());

        file.close();

        return {};
    }

    // ============================================================
    // Decrypt Image (AES + Reverse Permutation)
    // ============================================================

    bool ImageEncryption::decryptImage(
        const std::string& inputPath,
        const std::string& outputPath)
    {
        std::ifstream file(inputPath, std::ios::binary);
        if (!file)
            throw std::runtime_error("Cannot open encrypted file");

        salt.resize(SALT_SIZE);
        file.read(reinterpret_cast<char*>(salt.data()), SALT_SIZE);

        std::vector<uint8_t> iv(IV_SIZE);
        file.read(reinterpret_cast<char*>(iv.data()), IV_SIZE);

        int width, height, channels;
        file.read(reinterpret_cast<char*>(&width), sizeof(int));
        file.read(reinterpret_cast<char*>(&height), sizeof(int));
        file.read(reinterpret_cast<char*>(&channels), sizeof(int));

        std::vector<uint8_t> encryptedData(
            (std::istreambuf_iterator<char>(file)),
            std::istreambuf_iterator<char>());

        file.close();

        deriveKey();

        auto decrypted = aesDecrypt(encryptedData, iv);

        double chaotic_r = 3.99;
        double chaotic_x0 = 0.5;

        auto perm = chaoticPermutation(decrypted.size(), chaotic_r, chaotic_x0);
        auto original = reversePermutation(decrypted, perm);

        cv::Mat image(height, width, CV_8UC3, original.data());

        if (!cv::imwrite(outputPath, image))
            throw std::runtime_error("Failed to write decrypted image");

        return true;
    }

} // namespace imgenc