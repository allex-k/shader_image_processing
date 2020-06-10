#pragma once
#include <iostream>

void print_array(float* arr, int n) {
	for (int i = 0; i < n; ++i) {
		std::cout << "arr[" << i << "] = " << arr[i] << std::endl;
	}
}

void print_array(uint8_t* arr, int n) {
	for (int i = 0; i < n; ++i) {
		std::cout << "arr[" << i << "] = " << arr[i] << std::endl;
	}
}