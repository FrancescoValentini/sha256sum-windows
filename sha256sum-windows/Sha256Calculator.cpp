#include "Sha256Calculator.h"
#include <bcrypt.h>
#include <stdexcept>

#pragma comment(lib, "bcrypt.lib")

namespace sha256 {
    Sha256Calculator::Sha256Calculator() {
        DWORD cbData = 0;
        DWORD hashObjectSize = 0;
        DWORD hashSize = 0;

        // Opens an algorithm provider for the SHA-256 hashing algorithm.
        if (BCryptOpenAlgorithmProvider(
            &hAlg,
            BCRYPT_SHA256_ALGORITHM,
            nullptr,
            0) != 0)
            throw std::runtime_error("BCryptOpenAlgorithmProvider failed");

        // Retrieves the size, in bytes, of the hash object required by the algorithm.
        // The application must allocate a buffer of this size to store internal hash state.
        BCryptGetProperty(
            hAlg,
            BCRYPT_OBJECT_LENGTH,
            (PUCHAR)&hashObjectSize,
            sizeof(hashObjectSize),
            &cbData,
            0);

        // Retrieves the length, in bytes, of the resulting hash value (digest).
        // For SHA-256 this value is 32 bytes.
        BCryptGetProperty(
            hAlg,
            BCRYPT_HASH_LENGTH,
            (PUCHAR)&hashSize,
            sizeof(hashSize),
            &cbData,
            0);

        // Allocates memory for the hash object and the final digest.
        hashObject.resize(hashObjectSize);
        digest.resize(hashSize);

        // Creates a hash object that can be used to hash data incrementally.
        if (BCryptCreateHash(
            hAlg,
            &hHash,
            hashObject.data(),
            (ULONG)hashObject.size(),
            nullptr,
            0,
            0) != 0)
            throw std::runtime_error("BCryptCreateHash failed");
    }

    void Sha256Calculator::update(const void* data, size_t size) {
        BCryptHashData(
            hHash,
            (PUCHAR)data,
            (ULONG)size,
            0);
    }

    const std::vector<BYTE>& Sha256Calculator::doFinal() {
        BCryptFinishHash(
            hHash,
            digest.data(),
            (ULONG)digest.size(),
            0);
        return digest;
    }

    Sha256Calculator::~Sha256Calculator() {
        // Releases the hash object and frees associated resources.
        if (hHash) BCryptDestroyHash(hHash);

        // Closes the algorithm provider and releases system resources.
        if (hAlg)  BCryptCloseAlgorithmProvider(hAlg, 0);
    }
}