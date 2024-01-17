#include <chrono>
#include <random>

#include "FastFireBlockCipher.h"
#include "FunctionTest.h"

void TestFastFireBlockCipher()
{
	// Define a key, nonce, plaintext, and associated data for testing
	std::array<uint64_t, 8> key = {0x12345678, 0x23456789, 0x34567890, 0x45678901, 0x56789012, 0x67890123, 0x78901234, 0x89012345};
	std::array<uint64_t, 2> nonce = {0x12345678, 0x23456789};

	std::mt19937 prng(1);

	std::vector<uint64_t> plaintext(32, 0);
	std::generate(plaintext.begin(), plaintext.end(), prng);
	std::cout << "Original Text: ";
	for (auto t : plaintext) {
		std::cout << std::hex << t << " ";
	}
	std::cout << "\n\n";

	std::vector<uint64_t> associatedData = {0x12345678};

	// Create an instance of FastFireBlockCipher
	FastFireBlockCipher cipher;

	// Initialize with key and nonce
	cipher.InitialVectorAndKeyAndNonce(key, nonce);

	// Encrypt the plaintext
	std::vector<uint64_t> encryptedtext(plaintext.size());
	std::array<uint64_t, 4> authenticationTag {0, 0, 0, 0};
	cipher.Encryption(associatedData, plaintext, encryptedtext, authenticationTag);

	// Print the ciphertext and authentication tag
	std::cout << "Encrypted Text: ";
	for (auto c : encryptedtext) {
		std::cout << std::hex << c << " ";
	}
	std::cout << "\n\n";

	std::cout << "Authentication Tag: ";
	for (auto tag : authenticationTag) {
		std::cout << std::hex << tag << " ";
	}
	std::cout << "\n\n";

	// Reinitialize the cipher
	cipher.InitialVectorAndKeyAndNonce(key, nonce);

	// Decrypt the ciphertext
	std::vector<uint64_t> decryptedText(encryptedtext.size());
	cipher.Decryption(associatedData, encryptedtext, decryptedText, authenticationTag);

	// Print the decrypted text
	std::cout << "Decrypted Text: ";
	for (auto d : decryptedText) {
		std::cout << std::hex << d << " ";
	}
	std::cout << "\n\n";

	// Check if the decrypted text matches the original plaintext
	if (decryptedText == plaintext) {
		std::cout << "Test successful!" << std::endl;
	} else {
		std::cout << "Test failed!" << std::endl;
	}
}

void BenchmarkFastFireBlockCipher()
{
	// Define a key, nonce, plaintext, and associated data for testing
	std::array<uint64_t, 8> key = {0x12345678, 0x23456789, 0x34567890, 0x45678901, 0x56789012, 0x67890123, 0x78901234, 0x89012345};
	std::array<uint64_t, 2> nonce = {0x12345678, 0x23456789};

	std::mt19937 prng(1);

	std::vector<uint64_t> plaintext(13107200, 0);
	std::generate(plaintext.begin(), plaintext.end(), prng);

	std::vector<uint64_t> associatedData = {0x12345678};

	// Create an instance of FastFireBlockCipher
	FastFireBlockCipher cipher;

	// Initialize with key and nonce
	cipher.InitialVectorAndKeyAndNonce(key, nonce);

	// Encrypt the plaintext
	std::vector<uint64_t> encryptedtext(plaintext.size());
	std::array<uint64_t, 4> authenticationTag {0, 0, 0, 0};

	auto start = std::chrono::high_resolution_clock::now();
	cipher.Encryption(associatedData, plaintext, encryptedtext, authenticationTag);
	auto end = std::chrono::high_resolution_clock::now();
	auto encryptionTime = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();

	// Reinitialize the cipher
	cipher.InitialVectorAndKeyAndNonce(key, nonce);

	// Decrypt the ciphertext
	std::vector<uint64_t> decryptedText(encryptedtext.size());

	start = std::chrono::high_resolution_clock::now();
	cipher.Decryption(associatedData, encryptedtext, decryptedText, authenticationTag);
	end = std::chrono::high_resolution_clock::now();
	auto decryptionTime = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();

	// Check if the decrypted text matches the original plaintext
	if (decryptedText == plaintext) {
		std::cout << "Benchmark Test successful!" << std::endl;
	} else {
		std::cout << "Benchmark Test failed!" << std::endl;
	}

	std::cout << "Data Size: " << plaintext.size() * sizeof(uint64_t) << " bytes" << '\n';
	std::cout << "Encryption Time: " << encryptionTime << " seconds" << '\n';
	std::cout << "Decryption Time: " << decryptionTime << " seconds" << '\n';
}

int main()
{
	TestFastFireBlockCipher();
	BenchmarkFastFireBlockCipher();

	//FunctionTest::GeneratePermutationTest();
	return 0;
}