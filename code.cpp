#include "code.h"

#include <algorithm>
#include <cmath>
#include <vector>

#include "heap.h"

// Function call graph is already a tree -- no need for explicit tree structure
void shannon_fanno_assign(std::vector<symbol> &syms, codebook &book, int cword = 0, int depth = 0)
{
	static float log2_factor = 1 / log(2.f);

	const symbol &last = syms[syms.size() - 1];
	// If we have run out of meaningful symbols, we're finished
	if (syms.size() == 0 || last.p == 0)
		return;
	if (depth >= -log(last.p) * log2_factor)
	{
		book[last.sym] = codeword(cword, depth);
		syms.pop_back();
		return;
	}
	shannon_fanno_assign(syms, book, cword << 1, depth + 1);
	shannon_fanno_assign(syms, book, (cword << 1) + 1, depth + 1);
}

void shannon_fanno(const alphabet &a, codebook &book)
{
	std::vector <symbol> syms;
	for (int i = 0; i < a.size; ++i)
		syms.push_back(a[i]);
	std::sort(syms.begin(), syms.end());
	shannon_fanno_assign(syms, book);
}

bool sym_lt(symbol* const& a, symbol* const& b)
{
	return !(*b < *a);
}

void huffman_assign_codewords(symbol *root, codebook &cbook, int cword = 0, int depth = 0)
{
	// Check if this is a leaf node (summed symbols have negative, i.e. invalid, symbol values)
	if (root->sym >= 0)
	{
		cbook[root->sym] = codeword(cword, depth);
		return;
	}
	// Otherwise we are still at an intermediate node, so continue walking to the leaves.
	// Left leaves get a 0 appended to the codeword, right leaves get a 1.
	huffman_assign_codewords(root->a, cbook, cword << 1, depth + 1);
	huffman_assign_codewords(root->b, cbook, (cword << 1) + 1, depth + 1);
}

void huffman(const alphabet &a, codebook &cbook)
{
	/*
	 * "syms" is a mutable copy of the alphabet.
	 * Having "syms" lets us put pointers in the heap instead of values -- this means pointers between symbols
	 * aren't disturbed when the heap moves its objects around to maintain order.
	 */
	std::vector<symbol> syms;
	heap<symbol*> hp(sym_lt);
    int sym_count;

	for (int i = 0; i < a.size; ++i)
    {
        if (a[i].p > 0.f)
            syms.push_back((a[i]));
    }
    sym_count = syms.size();
    // Ensure that syms doesn't get resized (i.e. moved) when we start using pointers to it
    syms.reserve(sym_count * 2 + 1);

	for (int i = 0; i < sym_count; ++i)
	{
		hp.push(&syms[i]);
	}

	for (int i = 0; i < sym_count - 1; ++i)
	{
		// Pop the two least-probable symbols from the heap
		symbol *a, *b;
		hp.pop(a);
		hp.pop(b);
		// Create a "super symbol" with summed probability, and put this in the heap.
		syms.push_back(symbol(a, b));
		hp.push(&syms[syms.size() - 1]);
	}
	// The final symbol we created must be the root, since it contains all subsymbols.
	// Walk the tree from here to find the codewords that specify the leaves.
	huffman_assign_codewords(&syms[syms.size() - 1], cbook);
}

void pfc_decode(const codebook &cbook, bitstream bits, int count, std::vector<char> &out)
{
	symbol *syms = new symbol[2 * cbook.size()];
	int symbol_count = 0;
	std::map<int, int> symbol_indices;

	// Enumerate all the symbols into "syms" -- these will be the leaf nodes. Make note of indices.
	for (codebook::const_iterator iter = cbook.begin(); iter != cbook.end(); ++iter)
	{
		syms[symbol_count] = symbol(iter->first, 0, 0, 0);
		symbol_indices[iter->first] = symbol_count++;
	}
	// symbol name of -1 indicates that this is not a leaf node.
	syms[symbol_count] = symbol(-1, 0, 0, 0);
	symbol *root = &syms[symbol_count++];
	// Now build the tree from the codebook.
	for (codebook::const_iterator iter = cbook.begin(); iter != cbook.end(); ++iter)
	{
		int sym_name = iter->first;
		codeword cword = iter->second;
		int bits_left = cword.second;
        symbol *current_node = root;

		while (--bits_left)
		{
			// Pointer to pointer to next location. Indirection avoids duplication of next block of code.
			symbol **next_ptr;
			if (cword.first & (0x1 << (bits_left)))
				next_ptr = &current_node->b;
			else
				next_ptr = &current_node->a;
			// Follow pointer if next node exists, else create the node and update pointer
			if (*next_ptr)
			{
				current_node = *next_ptr;
			}
			else
			{
				syms[symbol_count] = symbol(-1, 0, 0, 0);
				*next_ptr = &syms[symbol_count++];
				current_node = *next_ptr;
			}
		}
		// Final bit of codeword specifies the leaf node, so is handled separately
		symbol **leaf_ptr = cword.first & 0x1 ?
			&current_node->b : &current_node->a;
        int index = symbol_indices[sym_name];
		*leaf_ptr = &syms[index];
	}
	// Phew! Decoding is easy now because PFCs are magical.
	symbol *current_node = root;
	while (count--)
	{
		if (bits.get(1))
			current_node = current_node->b;
		else
			current_node = current_node->a;
		if (current_node->sym >= 0)
		{
			out.push_back(current_node->sym);
			current_node = root;
		}
	}
	delete syms;
}
