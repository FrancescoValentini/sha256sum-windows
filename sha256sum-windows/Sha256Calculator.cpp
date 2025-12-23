#include "Sha256Calculator.h"
#include <bcrypt.h>
#include <stdexcept>
#include <ntstatus.h>
#include <string>


#pragma comment(lib, "bcrypt.lib")

#define THROW_NTSTATUS_ERROR(msg, status)                           \
    throw std::runtime_error(                                       \
        std::string(msg) +                                          \
        ", NTSTATUS=" + std::to_string(static_cast<long>(status))   \
    )


namespace sha256 {

    inline bool SUCCESS(NTSTATUS status) {
        return status == 0;
    }
    Sha256Calculator::Sha256Calculator() {
        DWORD cbData = 0;
        DWORD hashObjectSize = 0;
        DWORD hashSize = 0;
        NTSTATUS status = STATUS_SUCCESS;

        // Opens an algorithm provider for the SHA-256 hashing algorithm.
        status = BCryptOpenAlgorithmProvider(
            &hAlg,
            BCRYPT_SHA256_ALGORITHM,
            nullptr,
            0);
            
        if (!SUCCESS(status)) THROW_NTSTATUS_ERROR("BCryptOpenAlgorithmProvider failed", status);

        // Retrieves the size, in bytes, of the hash object required by the algorithm.
        // The application must allocate a buffer of this size to store internal hash state.
        status = BCryptGetProperty(
            hAlg,
            BCRYPT_OBJECT_LENGTH,
            (PUCHAR)&hashObjectSize,
            sizeof(hashObjectSize),
            &cbData,
            0);

        if (!SUCCESS(status)) THROW_NTSTATUS_ERROR("BCryptGetProperty(BCRYPT_OBJECT_LENGTH) failed", status);


        // Retrieves the length, in bytes, of the resulting hash value (digest).
        // For SHA-256 this value is 32 bytes.
        status = BCryptGetProperty(
            hAlg,
            BCRYPT_HASH_LENGTH,
            (PUCHAR)&hashSize,
            sizeof(hashSize),
            &cbData,
            0);

        if (!SUCCESS(status)) THROW_NTSTATUS_ERROR("BCryptGetProperty(BCRYPT_HASH_LENGTH) failed", status);

        // Allocates memory for the hash object and the final digest.
        hashObject.resize(hashObjectSize);
        digest.resize(hashSize);

        // Creates a hash object that can be used to hash data incrementally.
        status = BCryptCreateHash(
            hAlg,
            &hHash,
            hashObject.data(),
            static_cast<ULONG>(hashObject.size()),
            nullptr,
            0,
            0);
       
        if (!SUCCESS(status)) THROW_NTSTATUS_ERROR("BCryptCreateHash failed", status);
    }

    void Sha256Calculator::update(const void* data, size_t size) {
        NTSTATUS status = BCryptHashData(
            hHash,
            (PUCHAR)data,
            (ULONG)size,
            0);

        if (!SUCCESS(status)) THROW_NTSTATUS_ERROR("BCryptHashData failed", status);
    }

    const std::vector<BYTE>& Sha256Calculator::doFinal() {
        NTSTATUS status = BCryptFinishHash(
            hHash,
            digest.data(),
            (ULONG)digest.size(),
            0);
        if (!SUCCESS(status)) THROW_NTSTATUS_ERROR("BCryptFinishHash failed", status);

        return digest;
    }

    Sha256Calculator::~Sha256Calculator() {
        // Releases the hash object and frees associated resources.
        if (hHash) BCryptDestroyHash(hHash);

        // Closes the algorithm provider and releases system resources.
        if (hAlg)  BCryptCloseAlgorithmProvider(hAlg, 0);
    }
}