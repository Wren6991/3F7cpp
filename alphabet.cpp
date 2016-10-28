#include "alphabet.h"

#include <cmath>

void alphabet::histogram(iter start, iter end)
{
	int count = 0;
	for (int i = 0; i < size; ++i)
		syms[i].p = 0;
	while (start != end)
	{
		if (*start < size)
			syms[*start].p++;
		++start;
		++count;
	}
	for (int i = 0; i < size; ++i)
		syms[i].p /= count;
	
}

float alphabet::entropy()
{
	static float ln2_factor = 1 / log(2.f);

	float h = 0;
	for (int i = 0; i < size; ++i)
		if (syms[i].p)
			h -= syms[i].p * log(syms[i].p) * ln2_factor;
 	return h;
}

const symbol& alphabet::operator[](int index) const
{
	return syms[index];
}