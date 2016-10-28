#include <algorithm>
#include <fstream>
#include <iostream>

#include "bitstream.h"
#include "alphabet.h"
#include "code.h"

void test()
{
	alphabet a(10);
	std::vector<char> data;

	for (int i = 0; i < 10000; ++i)
		data.push_back(2000 / (i + 185));

	a.histogram(data.begin(), data.end());
	std::cout << "H = " << a.entropy() << "\n";
	for (int i = 0; i < a.size; ++i)
	{
		std::cout << "p[" << std::dec << i << "] = " << a[i].p << "\n";
	}

	std::vector<symbol> syms;
	for (int i = 0; i < a.size; ++i)
		syms.push_back(a[i]);

	std::sort(syms.begin(), syms.end());

	codebook cbook;

	shannon_fanno(a, cbook);

	float avg_len = 0;
	for (int i = 0; i < a.size; ++i)
	{
		 avg_len += a[i].p * cbook[a[i].sym].second;
	}

	std::cout << "Average length = " << avg_len << "\n";

	cbook = codebook();

	huffman(a, cbook);
	avg_len = 0;
	for (int i = 0; i < a.size; ++i)
	{
		 avg_len += a[i].p * cbook[a[i].sym].second;
	}

	std::cout << "Average length = " << avg_len << "\n";
}

int main(int argc, char **argv)
{
	std::fstream f("hamlet.txt", std::ios::in | std::ios::binary | std::ios::ate);
	int size = f.tellg();
	std::cout << "File size: " << std::dec << size << " bytes\n";
	f.seekg(0, std::ios::beg);

	std::vector<char> buf(size);
	f.read(buf.data(), size);

	alphabet a(256);
	a.histogram(buf.begin(), buf.end());

	std::cout << "H = " << a.entropy() << "\n";

	codebook cbook;
	huffman(a, cbook);

	float expected_len = 0;
	for (int i = 0; i < a.size; ++i)
	{
		expected_len += a[i].p * cbook[a[i].sym].second;
	}

	std::cout << "Average length: " << expected_len << "\n";

	int output_len_bits = 0;
	{
		std::fstream of("hamlet.cz", std::ios::out | std::ios::binary);
		bitstream bs(of);

		for (int i = 0; i < (int)buf.size(); ++i)
		{
			codeword cw = cbook[buf[i]];
			bs.put(cw.first, cw.second);
			output_len_bits += cw.second;
		}

		std::cout << "output_len_bits: " << output_len_bits << "\n";
	}

	std::fstream ifile("hamlet.cz", std::ios::in | std::ios::binary);
	std::fstream ofile("hamlet_decoded.txt", std::ios::out | std::ios::binary);
	bitstream bs(ifile);
	std::vector<char> decoded_text;
	pfc_decode(cbook, bs, output_len_bits, decoded_text);
	for (int i = 0; i < (int)decoded_text.size(); ++i)
		ofile.write(&decoded_text[i], 1);

	return 0;
}