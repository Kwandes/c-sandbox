#include <stdio.h>
#include "utils.h"

void printArray(int arr[], size_t arr_size)
{
    printf("[");
    for (int i = 0; i < arr_size; i++)
    {
        printf("%d", arr[i]);
        if (i != arr_size - 1) printf(",");
    }
    printf("]");
}

