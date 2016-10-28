#ifndef _ALPHABET_H_CAMZIP_
#define _ALPHABET_H_CAMZIP_

#include <vector>

struct symbol
{
	int sym;
	float p;
	symbol *a, *b;
	symbol(): sym(0), p(0), a(0), b(0) {}
	symbol(int _sym, float _p, symbol *_a, symbol *_b): sym(_sym), p(_p), a(_a), b(_b) {}
	// Implement copy constructor and operator< so that we can duplicate and sort lists
	symbol(const symbol &rhs): sym(rhs.sym), p(rhs.p), a(rhs.a), b(rhs.b) {}
	bool operator<(const symbol &rhs) const {return p < rhs.p;}
	// Summed symbol constructor for huffman trees etc
	symbol(symbol *_a, symbol *_b)
	{
		p = _a->p + _b->p;
		sym = -1;
		a = _a;
		b = _b;
	}
};

class alphabet
{
	typedef std::vector<char>::iterator iter;
	symbol *syms;
public:
	const int size;
	alphabet(int _size): size(_size)
	{
		syms = new symbol[size];
		for (int i = 0; i < size; ++i)
			syms[i].sym = i;
	}

	~alphabet()
	{
		delete[] syms;
	}

	void histogram(iter start, iter end);
	float entropy();
	const symbol& operator[](int index) const;
};

#endif // _ALPHABET_H_CAMZIP_