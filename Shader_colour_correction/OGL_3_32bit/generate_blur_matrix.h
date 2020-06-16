#pragma once
#define _USE_MATH_DEFINES
#include "math.h"
float sum_of_array(float* arr, int n);
void normalize_array(float *arr, int n);

void genenate_blur_vector_square(float* blurVector, int n) {
	float val = 1.f / n;
	for (int i = 0; i < n; i++)
		blurVector[i] = val;
}

void genenate_blur_vector_parabola(float* blurVector, int n, float power) {
	int i; float value, inorm;
	for (i = 0; i < n; i++) {
		inorm = (float)i / (n-1) * 2.f - 1.f; //приведення до діапазону [-1; 1]
		value = -pow(inorm, power)+1.f;
		if (value >= 0.f) blurVector[i] = value;
		else blurVector[i] = 0.f;
	}
	normalize_array(blurVector, n);
}
void genenate_gaussian_blur_vector(float* blurVector, int n) {
	int i; float value, inorm;
	for (i = 0; i < n; i++) {
		inorm = (float)i / (n-1) * 4.f - 2.f; //приведення до діапазону [-2; 2]
		value = exp(-(inorm*inorm)); //exp(-x^2);
		blurVector[i] = value;
	}
	normalize_array(blurVector, n);
}

void generate_sharpness_matrix(float* blurVector, float val) {
	const int n = 9;
	float val2 = (val - 1.f) / -8.f;
	int i;
	for (i = 0; i < 4; ++i)
		blurVector[i] = val2;
	blurVector[4] = val;
	for (i = 5; i < 9; ++i)
		blurVector[i] = val2;
}

void genenate_glur_zeroes(float* blurVector, int n) {
	for (int i = 0; i < n; i++)
		blurVector[i] = 0.0;
}

float sum_of_array(float* arr, int n) {
	float sum = 0.f;
	for (int i = 0; i < n; i++) sum += arr[i];
	return sum;
}

void normalize_array(float *arr, int n) {
	float sum = sum_of_array(arr, n);
	for (int i = 0; i < n; i++)
		arr[i] /= sum;
}
//====================================================================
class Matrix {
public:
	float* values;
	int nrows, ncols;
	Matrix(float* values, int nrows, int ncols) : values(values), nrows(nrows), ncols(ncols) {}
	float get(int row, int column) {
		if (row >= 0 && row < nrows && column >= 0 && column < ncols) return values[row*ncols + column];
		else return 0;
	}
	void set(int row, int column, float value) {
		if (row >= 0 && row < nrows && column >= 0 && column < ncols) values[row*ncols + column] = value;
	}
	float* getPtr(int row, int column) {
		return &values[row*ncols + column];
	}
};

int check_line(float *line, int start, int end) {
	int i;
	bool flag = false;
	for (i = start; i < end; ++i)
		if (line[i] == 1) {
			flag = true; break;
		}
	if (flag) return i;
	else return start;
}
void fill_convex_polygon(Matrix *mtrx) {
	int x, y, end;

	for (y = 0; y < mtrx->nrows; ++y)
	{
		x = 0;
		while (x < mtrx->ncols)
		{
			if (mtrx->get(y, x) == 0 && mtrx->get(y, x - 1) == 1)
			{
				end = check_line(mtrx->getPtr(y, 0), x, mtrx->ncols);
				for (int i = x; i < end; ++i)
					mtrx->set(y, i, 1);
				break;
			}
			++x;
		}
	}
}

void draw_line(Matrix* mtrx, float x1, float y1, float x2, float y2, float val) {
	float x = x2 - x1, y = y2 - y1;
	float len = sqrtf(x*x + y * y);
	x /= len; y /= len;  //нормалізація вектора

	//len = ceilf(len);
	//int len = lenf;
	float posX, posY;
	for (float i = 0; i <= len; ++i) {
		posX = x1 + x * i;
		posY = y1 + y * i;
		mtrx->set((int)roundf(posY), (int)roundf(posX), val);
	}

}

float maaxx(float a, float b) { return (a > b) ? a : b; }
void draw_regular_polygon(Matrix* mtrx, int nVertices, float val = 1) {

	float angle = 0.f, angleStep = M_PI * 2 / nVertices;
	float lenght = maaxx(mtrx->nrows, mtrx->ncols) * sinf(angleStep / 2.f) - 1;
	float posX = (mtrx->ncols - 1) / 2.f - lenght / 2.f, posY = 0, posXnew, posYnew;

	for (int i = 0; i < nVertices; ++i) {
		posXnew = posX + cosf(angle)*lenght;
		posYnew = posY + sinf(angle)*lenght;

		draw_line(mtrx, round(posX), round(posY), round(posXnew), round(posYnew), val);

		angle += angleStep;
		posX = posXnew;
		posY = posYnew;
	}
}

void generate_regular_polygon_matrix(float* values, int nrows, int ncols, int nVertices) {
	Matrix mtrx(values, nrows, ncols);
	draw_regular_polygon(&mtrx, nVertices);
	fill_convex_polygon(&mtrx);
	normalize_array(values, nrows*ncols);
}




