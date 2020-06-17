#pragma once
#include <time.h>

float scale(float v, float a, float b, float x, float y) {
	/*приведення значення v,
	яке знахадтться в діапазоні [a,b]
	до діапазону [x,y]*/
	return (v - a) / (b - a)*(y - x) + x;
}

int clip(int val, int minVal, int maxVal) {
	if (val < minVal) return minVal;
	else if (val > maxVal) return maxVal;
	else return val;
}

void fill_array_randomly(float* arr, int n, float minVal, float maxVal) {
	srand(time(NULL));
	for (int i = 0; i < n; ++i)
		arr[i] = scale(rand(), 0.f, (float)RAND_MAX, minVal, maxVal);
}

void print_array(float* arr, int n, int m)
{
	for (int i = 0; i < n; ++i)
	{
		for (int j = 0; j < m; ++j)
			std::cout << arr[i * m + j] << "  ";
		std::cout << std::endl;
	}
}
void print_array(float* arr, int n) {
	print_array(arr, 1, n);
}

void print_array(unsigned char* arr, int n)
{
	for (int i = 0; i < n; ++i)
		std::cout << (int)arr[i] << "  ";
}

void linspace(float* arr, float start, float end, uint32_t n) {
	float current = start;
	float step = (end - start) / ((float)n - 1);
	for (uint32_t i = 0; i < n; ++i) {
		arr[i] = current; current += step;

	}
}

void transpose_matrix(float* dest, float* src, int nrows, int ncols) {
	//nrows, ncols for source marix
	int i, j;
	for (i = 0; i < nrows; ++i)
		for (j = 0; j < ncols; ++j)
			dest[j*nrows + i] = src[i*ncols + j];
}
