#ifndef _HEAP_CAMZIP_H_
#define _HEAP_CAMPZIP_H_

#include <vector>
#include <cassert>

#define HEAP_LEFT(ix) (2 * (ix) + 1)
#define HEAP_RIGHT(ix) (2 * (ix) + 2)
#define HEAP_PARENT(ix) ((ix) / 2)

template<typename T>
struct heap
{
public:
	typedef bool (*lessthan_t)(const T& a, const T& b);
private:
	lessthan_t lessthan;
	std::vector<T> data;
	void swap(T &a, T &b);
public:
	heap(lessthan_t _lessthan): lessthan(_lessthan) {}
	void peek(T &x);
	void push(const T &x);
	void pop(T &x);
};

template <typename T>
void heap<T>::swap(T &a, T &b)
{
	T tmp = a;
	a = b;
	b = tmp;
}

template <typename T>
void heap<T>::peek(T &x)
{
	assert(data.size() > 0);
	x = data[0];
}

template <typename T>
void heap<T>::push(const T &x)
{
	// Put it in the first available space -- will be a leaf node by definition
	int pos = data.size();
	data.push_back(x);
	// Sift it up whilst not root, and less than parent.
	while (pos && lessthan(x, data[HEAP_PARENT(pos)]))
	{
		swap(data[pos], data[HEAP_PARENT(pos)]);
		pos = HEAP_PARENT(pos);
	}
}

template <typename T>
void heap<T>::pop(T &x)
{
	// Give root element to user, and put bottom element in its place.
	x = data[0];
	data[0] = data[data.size() - 1];
	data.pop_back();
	// Now sift the new root into its correct position.
	// Exit loop if we have no child nodes
	int pos = 0;
	while (HEAP_LEFT(pos) < (int)data.size())
	{
		// If we have two children:
		if (HEAP_RIGHT(pos) < (int)data.size())
		{
			int smallest = lessthan(data[HEAP_LEFT(pos)], data[HEAP_RIGHT(pos)]) ?
				HEAP_LEFT(pos) : HEAP_RIGHT(pos);
			if (lessthan(data[smallest], data[pos]))
			{
				swap(data[smallest], data[pos]);
				pos = smallest;
			}
			else
			{
				break;
			}
		}
		else
		{
			if (lessthan(data[HEAP_LEFT(pos)], data[pos]))
			{
				swap(data[pos], data[HEAP_LEFT(pos)]);
				pos = HEAP_LEFT(pos);
			}
			else
			{
				break;
			}
		}
	}

}

#endif // _HEAP_CAMPZIP_H_