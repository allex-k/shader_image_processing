#pragma once
#include "math.h"
float sum_of_array(float* arr, int n);
void normalize_array(float *arr, int n);
void genenate_blur_vector_square(float* blurVector, int n) {
	for (int i = 0; i < n; i++)
		blurVector[i] = 1.f;
	normalize_array(blurVector, n);
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

void genenate_glur_zeroes(float* blurVector, int n) {
	for (int i = 0; i < n; i++)
		blurVector[i] = 0.0;
}