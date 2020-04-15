#include "other_functions.h"
#define _USE_MATH_DEFINES
#include <math.h>

void print_array(float* arr, int n) {
	for (int i = 0; i < n; ++i) {
		std::cout << "arr[" << i << "] = " << arr[i] << std::endl;
	}
}