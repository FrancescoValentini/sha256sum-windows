#pragma once

#include <vector>
#include <cstddef>
#include <windows.h>

namespace sha256 {
	/// <summary>
	/// A C++ wrapper for Windows CNG (Cryptography API: Next Generation) SHA-256 implementation.
	/// </summary>
	class Sha256Calculator {
	private:
		/// @brief Handle to the CNG SHA-256 algorithm provider.
		BCRYPT_ALG_HANDLE  hAlg = nullptr;

		/// @brief Handle to the CNG hash object for the current hashing operation.
		BCRYPT_HASH_HANDLE hHash = nullptr;

		/// @brief Buffer to hold the hash object context required by CNG.
		std::vector<BYTE>  hashObject;

		/// @brief Buffer to store the computed SHA-256 digest (32 bytes/256 bits).
		std::vector<BYTE>  digest;

	public:
		/// <summary>
		/// Constructs a new Sha256Calculator instance and initializes CNG resources
		/// </summary>
		Sha256Calculator();

		/// <summary>
		/// Destroys the Sha256Calculator instance and releases all CNG resources.
		/// </summary>
		~Sha256Calculator();

		/// <summary>
		///  Adds data to the ongoing hash computation.
		///  Updates the hash state with the provided data.
		/// </summary>
		/// <param name="data">Pointer to the data to be hashed.</param>
		/// <param name="size">Number of bytes to hash from the data buffer. </param>
		void update(const void* data, size_t size);

		/// <summary>
		/// Finalizes the hash computation and returns the digest.
		/// </summary>
		/// <returns>
		/// Reference to the computed SHA-256 digest.
		/// </returns>
		const std::vector<BYTE>& doFinal();
	};
}