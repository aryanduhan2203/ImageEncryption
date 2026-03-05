#include "image_encryption.hpp"
#include "image_handler.hpp"

#include <iostream>
#include <string>

void printUsage(const char* program) {
    std::cout << "\nImageEncryption\n\n"
        << "Usage:\n"
        << "  " << program << " encrypt <input> -p <password>\n"
        << "  " << program << " decrypt <input> -p <password>\n\n";
}

int encryptCommand(int argc, char* argv[]) {
    std::string input = argv[2];
    std::string output;
    std::string password;
    double r = 3.99;

    for (int i = 3; i < argc; ++i) {
        std::string arg = argv[i];
        if ((arg == "-o" || arg == "--output") && i + 1 < argc)
            output = argv[++i];
        else if ((arg == "-p" || arg == "--password") && i + 1 < argc)
            password = argv[++i];
        else if ((arg == "-r" || arg == "--r") && i + 1 < argc)
            r = std::stod(argv[++i]);
    }

    if (!imgenc::isValidImagePath(input)) {
        std::cerr << "Invalid image file\n";
        return 1;
    }

    if (output.empty())
        output = imgenc::getOutputPath(input, true);

    if (password.empty()) {
        std::cout << "Enter password: ";
        std::getline(std::cin, password);
    }

    try {
        imgenc::ImageEncryption encryptor(password, r);
        encryptor.encryptImage(input, output);
        std::cout << "Encryption successful\n";
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        return 1;
    }
}

int decryptCommand(int argc, char* argv[]) {
    std::string input = argv[2];
    std::string output;
    std::string password;

    for (int i = 3; i < argc; ++i) {
        std::string arg = argv[i];
        if ((arg == "-o" || arg == "--output") && i + 1 < argc)
            output = argv[++i];
        else if ((arg == "-p" || arg == "--password") && i + 1 < argc)
            password = argv[++i];
    }

    if (!imgenc::fileExists(input)) {
        std::cerr << "File not found\n";
        return 1;
    }

    if (output.empty())
        output = imgenc::getOutputPath(input, false);

    if (password.empty()) {
        std::cout << "Enter password: ";
        std::getline(std::cin, password);
    }

    try {
        imgenc::ImageEncryption decryptor(password);
        decryptor.decryptImage(input, output);
        std::cout << "Decryption successful\n";
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
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
        if (argc < 3) return 1;
        return encryptCommand(argc, argv);
    }
    else if (command == "decrypt") {
        if (argc < 3) return 1;
        return decryptCommand(argc, argv);
    }
    else {
        printUsage(argv[0]);
        return 1;
    }
}