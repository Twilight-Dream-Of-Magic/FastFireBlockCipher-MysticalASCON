#pragma once

#include <cstdint>

#include <iostream>
#include <array>
#include <vector>

enum class FastFireBlockCipherMode : uint32_t
{
	ENCRYPTION,
	DECRYPTION
};

/*
	Encryption/Decryption:
	Sponge-Based

	Key Expansion Scheme:
	LFSR + BR + FSM(Based ARX)
*/
class FastFireBlockCipher
{

public:
	void InitialVectorOnly();
	void InitialVectorAndKey(const std::array<uint64_t, 8>& UserKey);
	void InitialVectorAndKeyAndNonce(const std::array<uint64_t, 8>& UserKey, const std::array<uint64_t, 2>& UserNumberOnce);

	void Encryption(const std::vector<uint64_t>& PlainText, std::vector<uint64_t>& CipherText, std::array<uint64_t, 4>& NewAuthenticationTag);
	void Decryption(const std::vector<uint64_t>& CipherText, std::vector<uint64_t>& PlainText, const std::array<uint64_t, 4>& OldAuthenticationTag);

	void Encryption(const std::vector<uint64_t>& AssociatedData, const std::vector<uint64_t>& PlainText, std::vector<uint64_t>& CipherText, std::array<uint64_t, 4>& NewAuthenticationTag);
	void Decryption(const std::vector<uint64_t>& AssociatedData, const std::vector<uint64_t>& CipherText, std::vector<uint64_t>& PlainText, const std::array<uint64_t, 4>& OldAuthenticationTag);

	//Hash value size is 512 bits
	void Hash(const std::vector<uint64_t>& Message, std::array<uint64_t, 8>& HashValue);

private:

	bool IsInitalization = false;

	static constexpr uint32_t PERMUTATION_ROUND_A = 18;
	static constexpr uint32_t PERMUTATION_ROUND_B = 9;

	static constexpr uint32_t BIT_RATE_SIZE = 64;

	std::array<uint64_t, 8> Key
	{
		0, 0, 0, 0,
		0, 0, 0, 0
	};

	/*
		InitialVector: 0,1,2,3
		Key: 4,5,6,7,8,9,10,11
		NoceNumber: 12,13

		AuthenticationTag size is 256 bit
		
		State Bit-rate size is 64-bits (Base information unit size)
		State Bit-capacity size is 1024-bits
		State security size is 512-bits

		State Bit-capacity size = State size - State Bit-rate size
		State size is 1088-bits = State security size × 2 + Bit-rate size
	*/
	std::array<uint64_t, 17> State
	{
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0
	};

	std::array<uint64_t, 4> ComputedAuthenticationTag { 0, 0, 0, 0 };

	static constexpr std::array<uint64_t, 17> RoundConstants
	{
		//Constants generate from Reference code 1
		0xe80ac7166cf28f32,0x8c4eea5619edf50e,0x9c59c393f8bc9f31,0x0bdf57c08b5d36ef,
		0xb890d67c306b27c7,0x01a54cd116627b85,0x401e07294ce8c8bf,0x61d65684a349baae,
		0x6a578466d51d25ee,0xaf10cfa7c6ac0dfa,0x178aaa79a01623fe,0xe0d6a6f52e58afab,
		0x0543d0ed6e9b4d29,

		//Concatenation of Fibonacci numbers., π, φ, e
		0x01B70C8E97AD5F98,0x243F6A8885A308D3,0x9E3779B97F4A7C15,0xB7E151628AED2A6A
	};

	uint64_t BitRotateLeft(uint64_t number, int bits);
	uint64_t BitRotateRight(uint64_t number, int bits);

	void CustomBoxWithBitwise(uint64_t& x0, uint64_t& x1, uint64_t& x2, uint64_t& x3, uint64_t& x4, uint64_t& x5, uint64_t& x6, uint64_t& x7);
	void Permutation(uint32_t PermutationRound);

	void Initalization();
	void ProcessingAssociatedData(const std::vector<uint64_t>& AssociatedData);
	void ProcessingPlainText(const std::vector<uint64_t>& PlainText, std::vector<uint64_t>& CipherText);
	void ProcessingCipherText(const std::vector<uint64_t>& CipherText, std::vector<uint64_t>& PlainText);
	void Finalization(FastFireBlockCipherMode Mode, const std::array<uint64_t, 4>& AuthenticationTag, bool& ValidityResult);
};

