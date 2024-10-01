#include "revert_string.h"
#include "string.h"

void RevertString(char *str)
{
	int len = strlen(str);
	int i = 0;
	int j = len - 1;
	char temp;

	for (i = 0; i < j; i++, j--) {
		temp = str[i];
		str[i] = str[j];
		str[j] = temp;
	}
}

