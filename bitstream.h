#ifndef _BITSTREAM_H_
#define _BITSTREAM_H_

#include <fstream>
#include <iostream>
#include <iomanip>
#include <stdint.h>

#include "endian.h"

/*
 * Bit-aligned puts/gets to the bitstream
 * end up as 32 bit word-aligned reads/writes to the fstream
 */
class bitstream
{
	uint32_t word_i, word_o;
	int bitcount_i, bitcount_o;
	std::fstream &stream;

	inline uint32_t rmask(int width)
	{
		return ~((uint32_t)0) >> (32 - width);
	}

	inline uint32_t lmask(int width)
	{
		return ~((uint32_t)0) << (32 - width);
	}

	public:
	bitstream(std::fstream &_stream) : stream(_stream)
	{
		word_i = 0;
		bitcount_i = 0;
		word_o = 0;
		bitcount_o = 0;

	}

	~bitstream()
	{
		// Flush pending bits on close
		if (bitcount_o)
		{
			word_o <<= (32 - bitcount_o);
			word_o = cpu_to_be32(word_o);
			stream.write((char*)&word_o, sizeof(uint32_t));
		}
	}

	uint32_t get(int bits)
	{
		uint32_t tmp = 0;
		if (bits > bitcount_i)
		{
			tmp = (word_i & lmask(bitcount_i)) >> (32 - bits);
			bits -= bitcount_i;
			stream.read((char*)&word_i, sizeof(uint32_t));
			word_i = be32_to_cpu(word_i);
			bitcount_i = 32;
		}
		tmp |= (word_i & lmask(bits)) >> (32 - bits);
		word_i <<= bits;
		bitcount_i -= bits;
		return tmp;
	}

	void put(uint32_t x, int bits)
	{
		if(bits + bitcount_o > 32)
		{
			x &= rmask(bits);
			int space = 32 - bitcount_o;
			bits -= space;
			word_o <<= space;
			word_o |= x >> bits;
			word_o = cpu_to_be32(word_o);
			stream.write((char*)&word_o, sizeof(uint32_t));
			word_o = 0;
			bitcount_o = 0;
		}
		word_o = (word_o << bits) | (x & rmask(bits));
		bitcount_o += bits;
	}
};

#endif // _BITSTREAM_H_