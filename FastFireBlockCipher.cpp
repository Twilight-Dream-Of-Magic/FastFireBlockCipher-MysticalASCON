#include "FastFireBlockCipher.h"

uint64_t FastFireBlockCipher::BitRotateLeft(uint64_t number, int bits) {
	return ((number >> (64 - bits)) | (number << bits));
}

uint64_t FastFireBlockCipher::BitRotateRight(uint64_t number, int bits) {
	return ((number << (64 - bits)) | (number >> bits));
}

//Designed by Twilight-Dream
//代换盒子的设计使用了逻辑电路图。
//The design of the substitution box uses logic circuit diagrams.
void FastFireBlockCipher::CustomBoxWithBitwise(uint64_t& x0, uint64_t& x1, uint64_t& x2, uint64_t& x3, uint64_t& x4, uint64_t& x5, uint64_t& x6, uint64_t& x7)
{
	// Step 1: Initial XOR Shuffle Mixing
	x1 ^= x4;
	x3 ^= x7;
	x2 ^= x0;
	x6 ^= x5;
	x4 ^= x6;
	x7 ^= x1;
	x4 ^= x2;
	x2 ^= x3;
	
	// Step 2: Bitwise NOT-AND, NOT-OR Operations for Nonlinearity function
	uint64_t t0 = ~(x1 & x6);
	uint64_t t1 = ~(x0 | x1);
	uint64_t t2 = ~(x2 & x4);
	uint64_t t3 = ~(x4 | x1);
	uint64_t t4 = ~(x3 & x5);
	uint64_t t5 = ~(x5 | x2);
	uint64_t t6 = ~(x4 & x3);
	uint64_t t7 = ~(x6 | x7);

	// Step 3: Use Nonlinear function
	x2 ^= t0;
	x1 ^= t1;
	x3 ^= t2;
	x5 ^= t3;
	x7 ^= t4;
	x4 ^= t5;
	x6 ^= t6;
	x0 ^= t7;

	//Step 3-1: Finally XNOR Shuffle Mixinng
	x0 ^= x1;
	x1 ^= ~x2;
	x2 ^= x3;
	x3 ^= x4;
	x4 ^= x5;
	x5 ^= x6;
	x6 ^= ~x7;
	x7 ^= x0;
}

void FastFireBlockCipher::Permutation(uint32_t PermutationRound)
{
	std::array<std::uint64_t, 17> TransformedState = std::array<std::uint64_t, 17>(State);

	for(uint32_t round = 0; round < PermutationRound; ++round)
	{
		/* XOR Constants */
		TransformedState[8] ^= RoundConstants[round % RoundConstants.size()];

		/* Nonlinear Substitution like a Uniform Permutation (Based bitwise) (Confusion) */
		CustomBoxWithBitwise(TransformedState[0], TransformedState[1], TransformedState[2], TransformedState[3], TransformedState[4], TransformedState[5], TransformedState[6], TransformedState[7]);

		/*Linear Permutation (Diffusion)*/
		//Bit rotation amount generated from reference code 2
		TransformedState[0] ^= BitRotateLeft(TransformedState[0], 10) ^ BitRotateLeft(TransformedState[0], 37);
		TransformedState[1] ^= BitRotateRight(TransformedState[1], 53) ^ BitRotateRight(TransformedState[1], 26);
		TransformedState[2] ^= BitRotateLeft(TransformedState[2], 13) ^ BitRotateRight(TransformedState[2], 6);
		TransformedState[3] ^= BitRotateRight(TransformedState[3], 50) ^ BitRotateLeft(TransformedState[3], 57);

		TransformedState[4] ^= BitRotateLeft(TransformedState[4], 19) ^ BitRotateLeft(TransformedState[4], 9);
		TransformedState[5] ^= BitRotateRight(TransformedState[5], 25) ^ BitRotateRight(TransformedState[5], 44);
		TransformedState[6] ^= BitRotateLeft(TransformedState[6], 17) ^ BitRotateRight(TransformedState[6], 8);
		TransformedState[7] ^= BitRotateRight(TransformedState[7], 46) ^ BitRotateLeft(TransformedState[7], 55);

		TransformedState[8] ^= BitRotateLeft(TransformedState[8], 59) ^ BitRotateLeft(TransformedState[8], 29);
		TransformedState[9] ^= BitRotateRight(TransformedState[9], 25) ^ BitRotateRight(TransformedState[9], 51);
		TransformedState[10] ^= BitRotateLeft(TransformedState[10], 7) ^ BitRotateRight(TransformedState[10], 15);
		TransformedState[11] ^= BitRotateRight(TransformedState[11], 35) ^ BitRotateLeft(TransformedState[11], 6);

		TransformedState[12] ^= BitRotateLeft(TransformedState[12], 61) ^ BitRotateLeft(TransformedState[12], 30);
		TransformedState[13] ^= BitRotateRight(TransformedState[13], 29) ^ BitRotateRight(TransformedState[13], 14);
		TransformedState[14] ^= BitRotateLeft(TransformedState[14], 41) ^ BitRotateRight(TransformedState[14], 20);
		TransformedState[15] ^= BitRotateRight(TransformedState[15], 22) ^ BitRotateLeft(TransformedState[15], 43);

		TransformedState[16] ^= BitRotateRight(TransformedState[16], 38) ^ BitRotateLeft(TransformedState[16], 13);

		/* State Array Content Left Rotate 1 */
		std::rotate(TransformedState.begin(), TransformedState.begin() + 1, TransformedState.end());
	}

	std::copy(TransformedState.begin(), TransformedState.end(), State.begin());
}

void FastFireBlockCipher::Initalization()
{
	//PERMUTATION_ROUND_A(State)
	this->Permutation(PERMUTATION_ROUND_A);

	//State : State_{Capacity...} ⊕ (0* || Original Key)
	State[9] ^= Key[0];
	State[10] ^= Key[1];
	State[11] ^= Key[2];
	State[12] ^= Key[3];
	State[13] ^= Key[4];
	State[14] ^= Key[5];
	State[15] ^= Key[6];
	State[16] ^= Key[7];
}

void FastFireBlockCipher::ProcessingAssociatedData(const std::vector<uint64_t>& AssociatedData)
{
	//PERMUTATION_ROUND_B(State_{Rate...} ⊕ AssociatedData_{i} || State_{Capacity...})
	for (size_t i = 0; i < AssociatedData.size(); i++)
	{
		State[0] ^= AssociatedData[i];
		this->Permutation(PERMUTATION_ROUND_B);
	}

	//State = State_{Capacity...} ⊕ (O...*1088 || 1)
	State[16] ^= 1;
}

void FastFireBlockCipher::ProcessingPlainText(const std::vector<uint64_t>& PlainText, std::vector<uint64_t>& CipherText)
{
	//Reference Algorithm: ASCON
#if 0
	//Since it's in 64-bit units, it doesn't need to be padded, assuming we've already padded the bits.
	//Then we need to take the plaintext bits, divide them into blocks of State_{Rate} size each, and divide them into "t" blocks.
	
	size_t bit_shift_size = PlainText.size() * 64 % BIT_RATE_SIZE;
	for (size_t i = 0; i < PlainText.size(); i++)
	{
		CipherText[i] = State[0] ^ PlainText[i];
		State[0] = CipherText[i];
		
		if(i < PlainText.size() - 1)
		{
			//"t" - 1 parts
			this->Permutation(PERMUTATION_ROUND_B);
		}
		else
		{
			//"t" part
			//The last ciphertext block, CipherText_{t}, requires special handling: the first few bits are intercepted and the following bits are discarded.
			//This is because the plaintext is padded, and if the first few bits are not intercepted, the ciphertext bits may be longer than the plaintext bits, so the first BitSize(P) mod State_{Rate} bits need to be intercepted.
			CipherText[i] &= ((0xFFFFFFFFFFFFFFFF >> (64 - bit_shift_size)) << (64 - bit_shift_size));
		}
	}

#else
	//i < size
	//State_{Rate...} = CipherText_{i} = State_{Rate...} ⊕ PlainText_{i}
	//State = PERMUTATION_ROUND_B(State_{Rate...} || State_{Capacity...})
	for (size_t i = 0; i < PlainText.size() - 1; i++)
	{
		CipherText[i] = State[0] ^ PlainText[i];
		State[0] = CipherText[i];
		this->Permutation(PERMUTATION_ROUND_B);
	}
	//State_{Rate...} = CipherText_{size} = State_{Rate} ⊕ PlainText_{size}
	CipherText.back() = State[0] ^ PlainText.back();
	State[0] = CipherText.back();
#endif // 0
}

void FastFireBlockCipher::ProcessingCipherText(const std::vector<uint64_t>& CipherText, std::vector<uint64_t>& PlainText)
{
	//Reference Algorithm: ASCON
#if 0
	//The ciphertext does not need to be bits padded.
	//Then we need to divide the ciphertext bits, each of which is the size of a State_{Rate}, and divide them into "t" blocks.
	
	//"t" - 1 parts
	for (size_t i = 0; i < CipherText.size() - 1; i++)
	{
		PlainText[i] = State[0] ^ CipherText[i];
		State[0] = CipherText[i];
		this->Permutation(PERMUTATION_ROUND_B);
	}

	//"t" part
	size_t offset_index = CipherText.size() - 1;
	size_t shift_bit_size = CipherText.size() * 64 % BIT_RATE_SIZE;

	//This is because CipherText_{t} is only shift_bit_size bits long, so you only need to use the first shift_bit_size bits of State_{Rate}.
	PlainText[offset_index] = ((State[0] >> (64 - shift_bit_size)) << (64 - shift_bit_size)) ^ CipherText[offset_index];
	//Here you are actually replacing the first shift_bit_size bit of State with CipherText_{t}, which XOR to 1 for the first shift_bit_size + 1 bit.
	State[0] = CipherText[offset_index] | (((State[0] << shift_bit_size) >> shift_bit_size) ^ (uint64_t(1) << (63 - shift_bit_size)));
#else
	//i < size
	//State_{Rate...} = PlainText_{i} = State_{Rate...} ⊕ CipherText_{i}
	//State = PERMUTATION_ROUND_B(State_{Rate...} || State_{Capacity})
	for (size_t i = 0; i < CipherText.size() - 1; i++)
	{
		PlainText[i] = State[0] ^ CipherText[i];
		State[0] = CipherText[i];
		this->Permutation(PERMUTATION_ROUND_B);
	}
	//State_{Rate...} = PlainText_{size} = State_{Rate} ⊕ CipherText_{size}
	PlainText.back() = State[0] ^ CipherText.back();
	State[0] = CipherText.back();
#endif // 0
}

void FastFireBlockCipher::Finalization(FastFireBlockCipherMode Mode, const std::array<uint64_t, 4>& AuthenticationTag, bool& ValidityResult)
{
	//State : State_{Capacity...} ⊕ (Original Key || 0*)
	State[1] ^= Key[0];
	State[2] ^= Key[1];
	State[3] ^= Key[2];
	State[4] ^= Key[3];
	State[5] ^= Key[4];
	State[6] ^= Key[5];
	State[7] ^= Key[6];
	State[8] ^= Key[7];

	this->Permutation(PERMUTATION_ROUND_A);

	//Generate Authentication Tag
	//Authentication Tag = State_{Capacity...} ⊕ Original Key
	ComputedAuthenticationTag[0] = State[1] ^ Key[0];
	ComputedAuthenticationTag[1] = State[2] ^ Key[1];
	ComputedAuthenticationTag[2] = State[3] ^ Key[2];
	ComputedAuthenticationTag[3] = State[4] ^ Key[3];

	switch (Mode)
	{
		case FastFireBlockCipherMode::ENCRYPTION:
		{
			ValidityResult = true;

			break;
		}
		
		case FastFireBlockCipherMode::DECRYPTION:
		{
			ValidityResult = ComputedAuthenticationTag == AuthenticationTag;

			break;
		}
		default:
			ValidityResult = false;
			break;
	}

	//Clean Original Key
	std::array<uint64_t, 8> ZeroKey
	{
		0, 0, 0, 0,
		0, 0, 0, 0
	};

	std::copy(ZeroKey.begin(), ZeroKey.end(), Key.begin());
	IsInitalization = false;
}

void FastFireBlockCipher::InitialVectorOnly()
{
	//Reset state
	std::array<uint64_t, 17> ZeroState
	{
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0
	};

	std::copy(ZeroState.begin(), ZeroState.end(), State.begin());

	/* Initial Vector */
	//SHA3-256Bit(FASTFIRECIPHER01)
	//0xe43ad043ebf17448, 0xa3de870f2d8d2081, 0xde4dc5b75ea0d95c, 0x581ebaaace163621
	State[0] = 0xe43ad043ebf17448, State[1] = 0xa3de870f2d8d2081, State[2] = 0xde4dc5b75ea0d95c, State[3] = 0x581ebaaace163621;

	Initalization();
	IsInitalization = true;
}

void FastFireBlockCipher::InitialVectorAndKey(const std::array<uint64_t, 8>& UserKey)
{
	if(UserKey.empty())
	{
		std::cout << "Work has stopped! There is no key data that needs to be processed." << std::endl;
		return;
	}
	else if(UserKey.size() > 8)
	{
		std::cout << "Work has stopped! There is too long key data size" << std::endl;
		return;
	}

	//Reset state
	std::array<uint64_t, 17> ZeroState
	{
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0
	};

	std::copy(ZeroState.begin(), ZeroState.end(), State.begin());

	/* Initial Vector */
	//SHA3-256Bit(FASTFIRECIPHER01)
	//0xe43ad043ebf17448, 0xa3de870f2d8d2081, 0xde4dc5b75ea0d95c, 0x581ebaaace163621
	State[0] = 0xe43ad043ebf17448, State[1] = 0xa3de870f2d8d2081, State[2] = 0xde4dc5b75ea0d95c, State[3] = 0x581ebaaace163621;
		
	/* Key */
	std::copy(UserKey.begin(), UserKey.end(), State.begin() + 4);

	//Clone Original Key
	std::copy(UserKey.begin(), UserKey.end(), Key.begin());
	
	Initalization();
	IsInitalization = true;
}

void FastFireBlockCipher::InitialVectorAndKeyAndNonce(const std::array<uint64_t, 8>& UserKey, const std::array<uint64_t, 2>& UserNumberOnce)
{
	if(UserKey.empty())
	{
		std::cout << "Work has stopped! There is no key data that needs to be processed." << std::endl;
		return;
	}
	else if(UserKey.size() > 8)
	{
		std::cout << "Work has stopped! There is too long key data size." << std::endl;
		return;
	}
	
	if(UserNumberOnce.empty())
	{
		std::cout << "Work has stopped! There is no number once data that needs to be processed." << std::endl;
		return;
	}
	else if(UserNumberOnce.size() > 2)
	{
		std::cout << "Work has stopped! There is too long number once data size." << std::endl;
		return;
	}

	//Reset state
	std::array<uint64_t, 17> ZeroState
	{
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0
	};

	std::copy(ZeroState.begin(), ZeroState.end(), State.begin());

	/* Initial Vector */
	//SHA3-256Bit(FASTFIRECIPHER01)
	//0xe43ad043ebf17448, 0xa3de870f2d8d2081, 0xde4dc5b75ea0d95c, 0x581ebaaace163621
	State[0] = 0xe43ad043ebf17448, State[1] = 0xa3de870f2d8d2081, State[2] = 0xde4dc5b75ea0d95c, State[3] = 0x581ebaaace163621;
		
	/* Key || Nonce Number */
	std::copy(UserKey.begin(), UserKey.end(), State.begin() + 4);
	std::copy(UserNumberOnce.begin(), UserNumberOnce.end(), State.begin() + 12);

	//Clone Original Key
	std::copy(UserKey.begin(), UserKey.end(), Key.begin());
	
	Initalization();
	IsInitalization = true;
}

void FastFireBlockCipher::Encryption(const std::vector<uint64_t>& PlainText, std::vector<uint64_t>& CipherText, std::array<uint64_t, 4>& NewAuthenticationTag)
{
	if(!IsInitalization)
	{
		std::cout << "Work has stopped! You didn't initialize the state data." << std::endl;
	}

	if(PlainText.empty())
	{
		std::cout << "Work has stopped! There is no plaintext data that needs to be processed." << std::endl;
		return;
	}
	else if(CipherText.empty())
	{
		CipherText.resize(PlainText.size(), 0);
	}

	this->ProcessingPlainText(PlainText, CipherText);

	bool ValidityResult = false;
	this->Finalization(FastFireBlockCipherMode::ENCRYPTION, NewAuthenticationTag, ValidityResult);
	NewAuthenticationTag = ComputedAuthenticationTag;
}

void FastFireBlockCipher::Decryption(const std::vector<uint64_t>& CipherText, std::vector<uint64_t>& PlainText, const std::array<uint64_t, 4>& OldAuthenticationTag)
{
	if(!IsInitalization)
	{
		std::cout << "Work has stopped! You didn't initialize the state data." << std::endl;
	}

	if(CipherText.empty())
	{
		std::cout << "Work has stopped! There is no ciphertext data that needs to be processed." << std::endl;
		return;
	}
	else if(PlainText.empty())
	{
		PlainText.resize(CipherText.size(), 0);
	}

	this->ProcessingCipherText(CipherText, PlainText);

	bool ValidityResult = false;
	this->Finalization(FastFireBlockCipherMode::DECRYPTION, OldAuthenticationTag, ValidityResult);

	if(!ValidityResult)
		std::cout << "Authentication tag has no validity! Possibly due to corrupted data or wrong key?" << std::endl;
}

void FastFireBlockCipher::Encryption(const std::vector<uint64_t>& AssociatedData, const std::vector<uint64_t>& PlainText, std::vector<uint64_t>& CipherText, std::array<uint64_t, 4>& NewAuthenticationTag)
{
	if(!IsInitalization)
	{
		std::cout << "Work has stopped! You didn't initialize the state data." << std::endl;
	}

	if(AssociatedData.empty())
	{
		std::cout << "Work has stopped! There is no associated data that needs to be processed." << std::endl;
		return;
	}

	if(PlainText.empty())
	{
		std::cout << "Work has stopped! There is no plaintext data that needs to be processed." << std::endl;
		return;
	}
	else if(CipherText.empty())
	{
		CipherText.resize(PlainText.size(), 0);
	}

	this->ProcessingAssociatedData(AssociatedData);
	this->ProcessingPlainText(PlainText, CipherText);

	bool ValidityResult = false;
	this->Finalization(FastFireBlockCipherMode::ENCRYPTION, NewAuthenticationTag, ValidityResult);
	NewAuthenticationTag = ComputedAuthenticationTag;
}

void FastFireBlockCipher::Decryption(const std::vector<uint64_t>& AssociatedData, const std::vector<uint64_t>& CipherText, std::vector<uint64_t>& PlainText, const std::array<uint64_t, 4>& OldAuthenticationTag)
{
	if(!IsInitalization)
	{
		std::cout << "Work has stopped! You didn't initialize the state data." << std::endl;
	}

	if(AssociatedData.empty())
	{
		std::cout << "Work has stopped! There is no associated data that needs to be processed." << std::endl;
		return;
	}

	if(CipherText.empty())
	{
		std::cout << "Work has stopped! There is no ciphertext data that needs to be processed." << std::endl;
		return;
	}
	else if(PlainText.empty())
	{
		PlainText.resize(CipherText.size(), 0);
	}

	this->ProcessingAssociatedData(AssociatedData);
	this->ProcessingCipherText(CipherText, PlainText);

	bool ValidityResult = false;
	this->Finalization(FastFireBlockCipherMode::DECRYPTION, OldAuthenticationTag, ValidityResult);

	if(!ValidityResult)
		std::cout << "Warnning: Authentication tag has no validity! Possibly due to corrupted data or wrong key?" << std::endl;
}

void FastFireBlockCipher::Hash(const std::vector<uint64_t>& Message, std::array<uint64_t, 8>& HashValue)
{
	//Reset state
	std::array<uint64_t, 17> ZeroState
	{
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0
	};

	std::copy(ZeroState.begin(), ZeroState.end(), State.begin());

	/* Initial Vector */
	//SHA3-256Bit(FASTFIRECIPHER01)
	//0xe43ad043ebf17448, 0xa3de870f2d8d2081, 0xde4dc5b75ea0d95c, 0x581ebaaace163621
	State[0] = 0xe43ad043ebf17448, State[1] = 0xa3de870f2d8d2081, State[2] = 0xde4dc5b75ea0d95c, State[3] = 0x581ebaaace163621;

	this->Permutation(PERMUTATION_ROUND_A);

	/* Absorption data */

	std::vector<uint64_t> TemporaryMessage {};
	if (Message.empty()) 
		TemporaryMessage.push_back(0x8000000000000000);
	else
		TemporaryMessage = Message;
	//i < size
	//State_{Rate...} = HashValue_{i} = State_{Rate...} ⊕ Message_{i}
	//State = PERMUTATION_ROUND_B(State_{Rate...} || State_{Capacity...})
	for (int i = 0; i < TemporaryMessage.size() - 1; i++)
	{
		State[0] ^= TemporaryMessage[i];
		this->Permutation(PERMUTATION_ROUND_B);
	}
	//State_{Rate...} = State_{Rate...} ⊕ Message_{size}
	State[0] ^= TemporaryMessage.back();

	/* Squeeze data */

	//State = PERMUTATION_ROUND_A(State)
	//HashValue_{i} = State_{Rate...}
	//State = PERMUTATION_ROUND_B(State)
	this->Permutation(PERMUTATION_ROUND_A);
	for (int i = 0; i < HashValue.size(); ++i)
	{
		HashValue[i] = State[0];
		this->Permutation(PERMUTATION_ROUND_B);
	}
}
