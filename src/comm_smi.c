#include "comm_smi.h"

double mean(double* arr, int sample){
    double sum = 0;
    for (int i = 0; i < sample; i++)
    {
        sum += arr[i]/sample;
    }
    return sum;
}

void linspace(double* x ,double init, double fin, int N) {
     double step = (fin - init) / (double)N;

     x[0] = init; 

     for (int i = 1; i < N; i++) {
         x[i] = x[i - 1] + step;
     }
     x[N - 1] = fin;
     return;
}

void dB2Linear(double* dB, double* linear, int arr_size){
    for (int i = 0; i < arr_size; i++)
    {
        linear[i] = pow(10, dB[i]/10);
    }
}

int HardDecisionWithLLR(double llr){
    if(llr >= 0){
        return 0;
    }else
    {   
        return 1;
    }      
}

void PrintLLR(double* llr, int channel_size){
    printf("the llr is:\n");
    for (int i = 0; i < channel_size; i++)
    {
        printf("%lf ", llr[i]);
    }
    printf("\n");
}

void fprintPyArray(FILE* f, char* arr_name ,double* arr, int size){
    fprintf(f, "%s ", arr_name);
    fprintf(f, "= [");
    fprintf(f, "%lf ", arr[0]);
    for (int i = 1; i < size; i++)
    {
        fprintf(f, ",%lf ", arr[i]);
    }
    fprintf(f, "]\n");   
}

void setAllCodewordTo(int var, int* code, int code_size)
{
    for (int i = 0; i < code_size; i++) 
    {
        code[i] = var;
    }
    return;
}

