#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <time.h>
#include<math.h>

void PrintLLR(double* llr, int channel_size);
void shuffle(int *array, int n);
void PrintCode(int* code, int code_size);
void Code2LLRWithSNR(int* encode, double* channel_llr, int code_size, double SNR);
void setZeroCodeword(int* code, int code_size);
void setIndexArr(int* arr, int start, int end);
int mapping(int* encode ,int code_size,double* tx);
void AWGNC(double* tx, int tx_size, double* rx, double snr);
void rx2LlrForBpsk(double* rx, int rx_size, double* channel_llr, double snr);
