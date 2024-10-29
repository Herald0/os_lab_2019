#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>

struct MinMax {
  int min;
  int max;
};

void GenerateArray(int *array, unsigned int array_size, unsigned int seed);

#endif
