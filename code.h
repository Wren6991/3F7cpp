#ifndef _CODE_H_CAMZIP_
#define _CODE_H_CAMZIP_

#include "alphabet.h"
#include "bitstream.h"

#include <vector>
#include <map>

typedef std::pair<int, int> codeword; // (bits, number of bits)
typedef std::map<int, codeword> codebook;

void shannon_fanno(const alphabet &a, codebook &cbook);
void huffman(const alphabet &a, codebook &cbook);
void pfc_decode(const codebook &cbook, bitstream bits, int count, std::vector<char> &out);

#endif // _CODE_H_CAMZIP_