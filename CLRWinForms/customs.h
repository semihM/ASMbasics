#pragma once

class Average
{
public:
	double comps[3];

	void Zero()
	{
		comps[0] = 0;
		comps[1] = 0;
		comps[2] = 0;
	}

	void Add(Average* avg)
	{
		this->comps[0] += avg->comps[0];
		this->comps[1] += avg->comps[1];
		this->comps[2] += avg->comps[2];
	}

	void Add(int x, int y, unsigned char* bmporg, int w, int h)
	{
		if (x < 0 || y < 0 || x >= w || y >= h)
		{
			//comps[0] += 255;
			//comps[1] += 255;
			//comps[2] += 255;
			return;
		}

		x += y * w;
		x *= 3;
		comps[0] += bmporg[x + 0];
		comps[1] += bmporg[x + 1];
		comps[2] += bmporg[x + 2];
	}

	void Sub(int x, int y, unsigned char* bmporg, int w, int h)
	{
		if (x < 0 || y < 0 || x >= w || y >= h)
		{
			//comps[0] -= 255;
			//comps[1] -= 255;
			//comps[2] -= 255;
			return;
		}

		x += y * w;
		x *= 3;
		comps[0] -= bmporg[x + 0];
		comps[1] -= bmporg[x + 1];
		comps[2] -= bmporg[x + 2];
	}

	void Multiply(double m)
	{
		comps[0] *= m;
		comps[1] *= m;
		comps[2] *= m;
	}

	void WritePixel(int a, int b, unsigned char* bmpDest, int w, int h)
	{
		if (a < 0 || b < 0 || a >= w || b >= h)
		{
			return;
		}
		a += b * w;
		a *= 3;

		bmpDest[a + 0] = (unsigned char)comps[0];
		bmpDest[a + 1] = (unsigned char)comps[1];
		bmpDest[a + 2] = (unsigned char)comps[2];
	}
};

template <typename T> class Offset1DArray
{
	long size = 0;
	T* arr = NULL;
	T* empty = new T[0];
	long lowerlim = 0;
	long upperlim = 0;

	void alloc(long _size)
	{
		if (_size < 1) { arr = NULL; }
		else if (_size % 2 == 0) { arr = NULL; }
		else
		{
			arr = new T[_size];
			size = _size;
			upperlim = (_size - 1) / 2;
			lowerlim = (1 - _size) / 2;
		}
	}

public:
	Offset1DArray() { alloc(0); }
	Offset1DArray(long arrsize) { alloc(arrsize); }

	~Offset1DArray() { if (arr != NULL) { delete[] arr; arr = NULL; } }

	T& operator() (long ind)
	{
		if (ind < lowerlim || ind > upperlim) { return empty[0]; }
		else { return arr[ind + upperlim]; }
	}

	void RandomFill(int max)
	{
		if (arr == NULL) { return; }
		Random rand;
		for (int i = 0; i < size; i++)
		{
			arr[i] = rand.Next(max);
		}
	}
};

template <typename T> class Offset2DArray
{
	long row = 0, col = 0;
	T* arr = NULL;
	T* empty = new T[0];

	long rowlowerlim = 0, rowupperlim = 0, collowerlim = 0, colupperlim = 0;

	void alloc(long _row, long _col)
	{
		if (_row < 1 || _col < 1) { arr = NULL; }
		else if (_row % 2 == 0 || _col % 2 == 0) { arr = NULL; }
		else
		{
			arr = new T[_row * _col];
			row = _row;
			col = _col;
			rowupperlim = (_row - 1) / 2;
			rowlowerlim = (1 - _row) / 2;
			colupperlim = (_col - 1) / 2;
			collowerlim = (1 - _col) / 2;
		}
	}

public:
	Offset2DArray() { alloc(0, 0); }
	Offset2DArray(long rowsize, long colsize) { alloc(rowsize, colsize); }

	~Offset2DArray() { if (arr != NULL) { delete[] arr; arr = NULL; } }

	T& operator() (long r, long c)
	{
		if (r < rowlowerlim || r > rowupperlim || c < collowerlim || c > colupperlim) { return empty[0]; }
		else { return arr[(r + rowupperlim) * col + (c + colupperlim)]; }
	}

	void RandomFill(int max)
	{
		if (arr == NULL) { return; }
		Random rand;
		for (int i = 0; i < row * col; i++)
		{
			arr[i] = (T)rand.Next(max);
		}
	}
};