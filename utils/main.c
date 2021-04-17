#include <stdio.h>
#include "utils/utils.h"

int main()
{
    printf("Hello, World!\n");
    int arr[] = {1, 2, 3, 4, 5, 6};
    printArray(arr, ARRAY_SIZE(arr));
    return 0;
}
