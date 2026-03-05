🔐 AES and Chaotic Permutation Based Image Encryption
📌 Project Description

This project implements a secure image encryption scheme that combines AES-256 encryption with chaotic permutation techniques to protect digital images from unauthorized access.

Traditional encryption algorithms encrypt pixel values but may leave spatial relationships partially intact. To enhance security, this project first shuffles image pixels using chaotic maps and then encrypts the shuffled data using AES encryption derived from a password-based key.

The system ensures strong resistance against statistical attacks by significantly reducing pixel correlation and increasing entropy in the encrypted image.

🚀 Key Features

AES-256 based image encryption

Chaotic pixel permutation for additional security

Password-based key derivation using PBKDF2

Salt and Initialization Vector generation

Secure encrypted file generation (.enc)

Complete image reconstruction during decryption

Security evaluation through entropy and correlation analysis

⚙️ Encryption Workflow
Input Image
     │
     ▼
Chaotic Pixel Permutation
     │
     ▼
Password → PBKDF2 → AES-256 Key
     │
     ▼
AES Encryption
     │
     ▼
Encrypted File (.enc)
🔓 Decryption Workflow
Encrypted File (.enc)
        │
        ▼
Extract Salt + IV
        │
        ▼
Password → PBKDF2 → AES Key
        │
        ▼
AES Decryption
        │
        ▼
Reverse Chaotic Permutation
        │
        ▼
Reconstructed Image
🧠 Methodology
Encryption Phase

Load the input image.

Generate a chaotic sequence to permute pixel positions.

Shuffle the image pixels using chaotic permutation.

Generate a random salt.

Derive a 256-bit AES key using PBKDF2 and the user password.

Generate an Initialization Vector (IV).

Encrypt the permuted image using AES.

Store the encrypted data along with salt and IV in a .enc file.

Decryption Phase

Load the encrypted .enc file.

Extract salt and IV.

Derive the same AES key using the password and PBKDF2.

Decrypt the ciphertext using AES.

Regenerate the chaotic permutation sequence.

Apply the reverse permutation.

Reconstruct the original image.

🔎 Security Analysis

To evaluate the effectiveness of the encryption algorithm, the following metrics are used.

1️⃣ Entropy Analysis

Entropy measures randomness in the image data.

An ideal encrypted image should have entropy close to 8.

Image Type	Entropy
Original Image	7.794651
Encrypted Image	≈ 7.99

Higher entropy indicates stronger resistance against statistical analysis.

2️⃣ Correlation Analysis

Correlation measures dependency between neighboring pixels.

For a secure encrypted image, correlation should be close to zero.

Direction	Original Image	Encrypted Image
Horizontal	High (~0.9)	Near 0
Vertical	High (~0.9)	Near 0
Diagonal	High (~0.9)	Near 0

This confirms that the encryption process effectively removes pixel relationships.

🛠 Technologies Used

Python

NumPy

OpenCV

Cryptography Library

AES Encryption

PBKDF2 Key Derivation

📂 Project Structure
Image-Encryption/
│
├── encrypt.py
├── decrypt.py
├── chaotic_permutation.py
├── security_analysis.py
│
├── images/
│   ├── original/
│   ├── encrypted/
│   └── decrypted/
│
├── results/
│   ├── entropy_results.txt
│   └── correlation_results.txt
│
└── README.md
▶️ How to Run
Install Dependencies
pip install numpy opencv-python cryptography
Encrypt an Image
python encrypt.py input_image.png

Output:

encrypted_image.enc
Decrypt an Image
python decrypt.py encrypted_image.enc

Output:

reconstructed_image.png
🔬 Applications

Secure medical image transmission

Military communication systems

Digital image protection

Secure cloud image storage

Confidential image sharing

⚠️ Limitations

Encryption time increases with larger images

Requires the correct password for decryption

Chaotic parameters must match during encryption and decryption

📈 Future Improvements

GPU-accelerated image encryption

Real-time encrypted image transmission

Multi-layer chaotic encryption

Integration with secure network protocols

📜 License

This project is intended for educational and research purposes.