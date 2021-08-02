#include<stdio.h>
#include<assert.h>

void divise(int* dividend, int dend_size, int* divisor, int sor_size, int* remainder);
void crcEncoder(int* code, int code_size, int* encode);

// 1 error
// 0 vaild
int crcChecker(int* encode, int code_size);
