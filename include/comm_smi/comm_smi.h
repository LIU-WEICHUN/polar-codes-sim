#include <stdio.h>
#include <math.h>

double mean(double* arr, int sample);
void linspace(double* x ,double init, double fin, int N) ;
void dB2Linear(double* dB, double* linear, int arr_size);

int HardDecisionWithLLR(double llr);
void PrintLLR(double* llr, int channel_size);
void fprintPyArray(FILE* f, char* arr_name ,double* arr, int size);
void setAllCodewordTo(int var, int* code, int code_size);