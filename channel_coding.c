#include "channel_coding.h"

void setIndexArr(int* arr, int start, int end)
{
    for (int i = start; i < end+1; i++)
    {
        arr[i] = i;
    }
}

void setZeroCodeword(int* code, int code_size)
{
    for (int i = 0; i < code_size; i++) 
    {
        code[i] = 0;
    }
    return;
}


int mapping(int* encode ,int code_size,double* tx){
    for (int i = 0; i < code_size; i++)
    {
        switch (encode[i])
        {
            case 0:
                tx[i] = 1.0;
                break;
            case 1:
                tx[i] = -1.0;
                break;
            default:
                printf("error");
                break;
        }
    }
    return code_size;
}
void AWGNC(double* tx, int tx_size, double* rx, double snr){
    const gsl_rng_type * T;
    gsl_rng * r;
  /* create a generator chosen by the
     environment variable GSL_RNG_TYPE */
    gsl_rng_env_setup();
    T = gsl_rng_default;
    r = gsl_rng_alloc (T);
    gsl_rng_set(r, time(0));
    double sigma = sqrt(1/snr);
    for (int i = 0; i < tx_size; i++)
    {
        rx[i] = tx[i] + gsl_ran_gaussian(r, sigma);
    }
    return;
    
}
void rx2LlrForBpsk(double* rx, int rx_size, double* channel_llr, double snr){
    for (int i = 0; i < rx_size; i++)
    {
        channel_llr[i] = 2*rx[i]*snr;
    }
    return;
    
}

void Code2LLRWithSNR(int* encode, double* channel_llr, int code_size, double snr){
    double tx[code_size];
    int tx_size;
    double rx[code_size];

    if(snr < 0){
        for (int i = 0; i < code_size; i++)
        {  
            switch (encode[i])
            {
            case 0:
                channel_llr[i] = 10;
                break;
            case 1:
                channel_llr[i] = -10;
                break;
            default:
                printf("error");
                break;
            }   
        }
    }else
    {
        tx_size = mapping(encode , code_size, tx);
        AWGNC(tx, tx_size, rx, snr);
        rx2LlrForBpsk(rx, code_size, channel_llr, snr);
    }
    
}
void PrintCode(int* code, int code_size){
    printf("\nthe %d bits code is:\n", code_size);
    for (int i = 0; i < code_size; i++)
    {
        printf("%d ",code[i]);
    }
    printf("\n");
    return;
}

/* Arrange the N elements of ARRAY in random order.
   Only effective if N is much smaller than RAND_MAX;
   if this may not be the case, use a better random
   number generator. */
void shuffle(int *array, int n)
{
    if (n > 1) 
    {
        int i;
        for (i = 0; i < n - 1; i++) 
        {
          int j = i + rand() / (RAND_MAX / (n - i) + 1);
          int t = array[j];
          array[j] = array[i];
          array[i] = t;
        }
    }
}