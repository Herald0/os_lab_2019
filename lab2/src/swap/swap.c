#include "swap.h"

void Swap(char *left, char *right)
{
	int temp = *left;
	*left = *right;
	*right = temp;
}
