#include "FunctionTest.h"

namespace FunctionTest
{

	/*
		NOT-OR Gate:
		1 OR 1 = 1 -> NOT -> 0
		1 OR 0 = 1 -> NOT -> 0
		0 OR 1 = 1 -> NOT -> 0
		0 OR 0 = 0 -> NOT -> 1

		NOT-AND Gate:
		1 AND 1 = 1 -> NOT -> 0
		1 AND 0 = 0 -> NOT -> 1
		0 AND 1 = 0 -> NOT -> 1
		0 AND 0 = 0 -> NOT -> 1

		XOR Gate:
		1 XOR 1 = 0
		1 XOR 0 = 1
		0 XOR 1 = 1
		0 XOR 0 = 0

		XNOR Gate:
		1 XOR 1 = 0 -> NOT -> 1
		1 XOR 0 = 1 -> NOT -> 0
		0 XOR 1 = 1 -> NOT -> 0
		0 XOR 0 = 0 -> NOT -> 1
	*/

	/*

	# Reference code 1:
		
	import numpy as np
	import struct
	from scipy.integrate import odeint
	import bcrypt
	import base64

	# Constants for the double pendulum
	LENGTH_1, LENGTH_2 = 1.0, 1.0
	MASS_1, MASS_2 = 1.0, 1.0
	GRAVITY = 9.81


	def double_pendulum_equations(state, time):
		"""Differential equations for the double pendulum system."""
		theta1, angular_velocity1, theta2, angular_velocity2 = state
		cos_diff, sin_diff = np.cos(theta1 - theta2), np.sin(theta1 - theta2)
	
		theta1_rate = angular_velocity1
		theta2_rate = angular_velocity2
	
		angular_velocity1_rate = (
			MASS_2 * GRAVITY * np.sin(theta2) * cos_diff 
			- MASS_2 * sin_diff * (LENGTH_1 * angular_velocity1**2 * cos_diff + LENGTH_2 * angular_velocity2**2) 
			- (MASS_1 + MASS_2) * GRAVITY * np.sin(theta1)
		) / LENGTH_1 / (MASS_1 + MASS_2 * sin_diff**2)
    
		angular_velocity2_rate = (
			(MASS_1 + MASS_2) * (LENGTH_1 * angular_velocity1**2 * sin_diff - GRAVITY * np.sin(theta2) + GRAVITY * np.sin(theta1) * cos_diff) 
			+ MASS_2 * LENGTH_2 * angular_velocity2**2 * sin_diff * cos_diff
		) / LENGTH_2 / (MASS_1 + MASS_2 * sin_diff**2)
    
		return theta1_rate, angular_velocity1_rate, theta2_rate, angular_velocity2_rate


	def decode_modified_base64(encoded_string):
		"""Custom function to decode bcrypt's modified base64 encoding."""
		modified_string = (encoded_string.replace('.', '+')
						   .replace('/', '.')
						   .replace('I', '/')
						   .replace('l', 'i')
						   .replace('O', 'o')
						   .replace('0', 'o'))
		modified_string += '=='
		return base64.b64decode(modified_string, altchars=b'+.')


	def extract_random_from_hash_and_convert_to_hex(bcrypt_hash):
		"""Function to extract random part from bcrypt hash and convert to hex 64-bit integer."""
		random_part = bcrypt_hash[-31:-1]
		decoded_bytes = decode_modified_base64(random_part)
		integer_value = struct.unpack('Q', decoded_bytes[:8])[0]
		hex_value = hex(integer_value)
		return hex_value


	# Initial conditions and time array
	initial_conditions = [np.pi / 4, 0.0, np.pi / 2, 0.0]
	time_values = np.linspace(0, 10, 500)

	# Solve the double pendulum system
	solution = odeint(double_pendulum_equations, initial_conditions, time_values)

	# Convert the floating-point numbers to 64-bit integers
	integers_from_solution = [struct.unpack('Q', struct.pack('d', value))[0] for value in solution[:, 0][:13]]

	# Fixed salt (hardcoded)
	DETERMINISTIC_SALT = b'$2b$12$TG.4e7GhgfVXIsx6CseUDO'

	# Convert the integers to strings, hash using bcrypt with the fixed salt, and apply the extraction function
	hashed_strings_with_fixed_salt = [bcrypt.hashpw(str(integer).encode(), DETERMINISTIC_SALT).decode() for integer in integers_from_solution]
	hex_values_from_hashed_strings = [extract_random_from_hash_and_convert_to_hex(hashed_string) for hashed_string in hashed_strings_with_fixed_salt]

	# Print the result
	for hex_value in hex_values_from_hashed_strings:
		print(hex_value)

	*/

	/*

	//Reference code 2:
	
	#include <cmath>
	#include <iostream>
	#include <vector>
	#include <numeric>
	#include <unordered_set>

	struct PairHash {
		template <class T1, class T2>
		std::size_t operator()(const std::pair<T1, T2>& pair) const {
			return std::hash<T1>()(std::min(pair.first, pair.second)) ^ std::hash<T2>()(std::max(pair.first, pair.second));
		}
	};

	int main() {
		int prime_numbers[16] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53};
		std::unordered_set<std::pair<int, int>, PairHash> co_prime_pairs;

		for (int i = 0; i < 16; ++i) {
			double random_float1 = (prime_numbers[i] * prime_numbers[(i + 1) % 16]) / 1000.0;
			double random_float2 = (prime_numbers[(i + 2) % 16] * prime_numbers[(i + 3) % 16]) / 1000.0;
			double random_float3 = (prime_numbers[(i + 4) % 16] * prime_numbers[(i + 5) % 16]) / 1000.0;

			double nonlinear_value = std::sqrt(prime_numbers[i] * random_float1) * std::exp(prime_numbers[(i + 1) % 16] * random_float2) * std::sin(prime_numbers[(i + 2) % 16] * random_float3);
			uint64_t bits_representation = *reinterpret_cast<uint64_t*>(&nonlinear_value);

			for (int j = 0; j < 64 && co_prime_pairs.size() < 64; ++j) {
				int a = (bits_representation >> (2 * j)) & 0x3F;
				int b = (bits_representation >> (2 * j + 1)) & 0x3F;

				if (std::gcd(a, b) == 1) {
					co_prime_pairs.insert({a, b});
				}
			}
		}

		for (const auto& pair : co_prime_pairs) {
			std::cout << "Co-prime pair: (" << pair.first << ", " << pair.second << ")\n";
		}

		return 0;
	}

	*/
	
	uint64_t BitRotateLeft(uint64_t number, int bits)
	{
		return ((number >> (64 - bits)) | (number << bits));
	}

	uint64_t BitRotateRight(uint64_t number, int bits)
	{
		return ((number << (64 - bits)) | (number >> bits));
	}

	void custom_box_with_bitwise_original(uint64_t& x0, uint64_t& x1, uint64_t& x2, uint64_t& x3, uint64_t& x4, uint64_t& x5, uint64_t& x6, uint64_t& x7)
	{
		// Step 1: Initial XOR Shuffle Mixing
		x0 ^= x4;
		x4 ^= x3;
		x2 ^= x1;
		x5 ^= x7;
		x6 ^= x5;
		x7 ^= x2;
	
		// Step 2: Bitwise NOT and AND Operations for Nonlinearity function
		uint64_t t0 = ~(x0 & x1);
		uint64_t t1 = ~(x1 & x2);
		uint64_t t2 = ~(x2 & x3);
		uint64_t t3 = ~(x3 & x4);
		uint64_t t4 = ~(x4 & x0);
		uint64_t t5 = ~(x5 & x6);
		uint64_t t6 = ~(x6 & x7);
		uint64_t t7 = ~(x7 & x5);

		// Step 3: Use Nonlinear function
		x0 ^= t1;
		x1 ^= t2;
		x2 ^= t3;
		x3 ^= t4;
		x4 ^= t5;
		x5 ^= t6;
		x6 ^= t7;
		x7 ^= t0;

		//Step 3-1: Finally XOR Shuffle Mixinng and 
		x1 ^= x0;
		x0 ^= x4;
		x3 ^= x2;
		x2 = ~x2;
		x5 ^= x7;
		x6 ^= x5;
		x7 ^= x3;

		// Optional Step 4: Rotation and Modulo Addition Operations
		//x0 = (x0 << 13) | (x0 >> (64 - 13)); // Rotate
		//x1 = (x1 << 17) | (x1 >> (64 - 17)); // Rotate
		//x2 += x3; x3 += x4; x4 += x5; x5 += x6; x6 += x7; x7 += x0; // Addition
	}

	//Designed by Twilight-Dream
	//代换盒子的设计使用了逻辑电路图。
	//The design of the substitution box uses logic circuit diagrams.
	void custom_box_with_bitwise(uint64_t& x0, uint64_t& x1, uint64_t& x2, uint64_t& x3, uint64_t& x4, uint64_t& x5, uint64_t& x6, uint64_t& x7)
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

	void Permutation(std::array<std::uint64_t, 17>& State)
	{
		std::array<std::uint64_t, 17> TransformedState = State;

		constexpr std::array<uint64_t, 17> RoundConstants
		{
			//Constants generate from Reference code 1
			0xe80ac7166cf28f32,0x8c4eea5619edf50e,0x9c59c393f8bc9f31,0x0bdf57c08b5d36ef,
			0xb890d67c306b27c7,0x01a54cd116627b85,0x401e07294ce8c8bf,0x61d65684a349baae,
			0x6a578466d51d25ee,0xaf10cfa7c6ac0dfa,0x178aaa79a01623fe,0xe0d6a6f52e58afab,
			0x0543d0ed6e9b4d29,

			//Concatenation of Fibonacci numbers., π, φ, e
			0x01B70C8E97AD5F98,0x243F6A8885A308D3,0x9E3779B97F4A7C15,0xB7E151628AED2A6A
		};

		for (uint32_t round = 0; round < 18; ++round) {
			/* XOR Constants */
			TransformedState[8] ^= RoundConstants[round % RoundConstants.size()];

			/* Nonlinear Substitution like a Uniform Permutation (Based bitwise) (Confusion) */
			custom_box_with_bitwise(TransformedState[0], TransformedState[1], TransformedState[2], TransformedState[3], TransformedState[4], TransformedState[5], TransformedState[6], TransformedState[7]);

			/* Linear Permutation (Diffusion) */
			// Bit rotates generate from Reference code 2
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

	void GeneratePermutationTest()
	{
		std::array<std::uint64_t, 17> State
		{
			//SHA3-256Bit(FASTFIRECIPHER01)
			0xe43ad043ebf17448, 0xa3de870f2d8d2081, 0xde4dc5b75ea0d95c, 0x581ebaaace163621,
			0, 0, 0, 0,
			0, 0, 0, 0,
			0, 0, 0
		};

		for(uint32_t round = 0; round < 128; ++round)
		{
			Permutation(State);
		}

		std::ofstream file("PermutationTest.bin", std::ios::binary);

		for (int i = 0; i < 964; ++i) { // 960 iterations to generate 128 KB
			Permutation(State);
			if(i == 963)
			{
				for (size_t j = 0; j < 13; j++)
				{
					file.write(reinterpret_cast<const char*>(&State[j]), sizeof(State[j]));
				}
				break;
			}

			for (size_t j = 0; j < 17; j++)
			{
				file.write(reinterpret_cast<const char*>(&State[j]), sizeof(State[j]));
			}
		}

		file.close();
	}

	void GenerateNonlinearSubstitutionTest()
	{
		uint64_t x0 = 0x123456789ABCDEF0;
		uint64_t x1 = 0xFEDCBA9876543210;
		uint64_t x2 = 0xAABBCCDDEEFF0011;
		uint64_t x3 = 0x1122334455667788;
		uint64_t x4 = 0x99AABBCCDDEEFF00;
		uint64_t x5 = 0x0011223344556677;
		uint64_t x6 = 0x8899AABBCCDDEEFF;
		uint64_t x7 = 0xFFEEDDCCBBAA9988;

		std::ofstream file("NonlinearSubstitutionTest.bin", std::ios::binary);

		for (int i = 0; i < 2048; ++i) { // 2048 iterations to generate 128 KB
			custom_box_with_bitwise(x0, x1, x2, x3, x4, x5, x6, x7);
			file.write(reinterpret_cast<const char*>(&x0), sizeof(x0));
			file.write(reinterpret_cast<const char*>(&x1), sizeof(x1));
			file.write(reinterpret_cast<const char*>(&x2), sizeof(x2));
			file.write(reinterpret_cast<const char*>(&x3), sizeof(x3));
			file.write(reinterpret_cast<const char*>(&x4), sizeof(x4));
			file.write(reinterpret_cast<const char*>(&x5), sizeof(x5));
			file.write(reinterpret_cast<const char*>(&x6), sizeof(x6));
			file.write(reinterpret_cast<const char*>(&x7), sizeof(x7));
		}

		file.close();
	}
}