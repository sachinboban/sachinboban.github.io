
/*
 * Demonstrate a simple buffer overflow attack
 */
#include <stdio.h>
#include <stdlib.h>

/*
 * A function that is never called!
 */
void not_used(void)
{
    printf("I am never here!\n");
    exit(1);
}

/*
 * Function to read an array and return the product of
 * all the elements in the array.
 */
long serial_mult(void)
{
    long a[5];
    int size;
    int i;
    long ret;

    printf("Enter array size\n");
    scanf("%d", &size);

    printf("Enter array the array\n");
    for (i=0; i < size; ++i) {
        scanf("%ld", &a[i]);
    }

    ret = 1;
    for (i=0; i < size; ++i) {
        ret *= a[i];
    }
    return ret;
}

int main()
{
    long prod;

    prod = serial_mult();
    printf("Product: %ld\n", prod);
    return 0;
}
