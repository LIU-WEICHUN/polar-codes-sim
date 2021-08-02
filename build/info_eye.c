#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include <string.h>
#include "channel_coding.h"

double LLRUpperHardwareFriendly(double llr_upper, double llr_lower);
double LLRUpper(double llr_upper, double llr_lower);
double LLRUpperMaxStar(double llr_upper, double llr_lower);
double LLRLower(double llr_upper, double llr_lower, int u);
void recursive_caluelation(int code_size,double* channel_llr, int* decoded_code, int* encode_assume, double* decode_llr);
void SC_decoder(int code_size, int* decoded_code,double* channel_llr, int* frozen_index,int frozen_size, int* frozen_value, int* encode_assume, double* decode_llr);
void PolarEncoder(int* code, int code_size, int* encoded);
void SetFrozenBits2Code(int* decoded, int code_size, int* frozen_index, int frozen_size, int* frozen_value);
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



int main(int argc, char const *argv[])
{   
    // int code_size = atoi(argv[1]);
    int code_size = 2;
    //code
    int encode[code_size];
    int code[code_size];
    int decoded[code_size];
    int encode_assume[code_size];

    double channel_llr[code_size];
    int frozen_index[code_size];
    int frozen_size;
    int frozen_value[code_size];
    int sample_snr = 100;
    int sample_llr = 10000;
    double x_dB[sample_snr];
    double x[sample_snr];
    double decode_llr[code_size];
    double entropy_decode[code_size];
    double entropy_channel[code_size];
    double capacity_channel[code_size];
    double capacity_decode[code_size];
    double capacity_upper[sample_snr];
    double capacity_lower[sample_snr];
    FILE *f = fopen("eye0703.py", "w");
    gsl_rng * r;

    const gsl_rng_type * T;
  /* create a generator chosen by the
     environment variable GSL_RNG_TYPE */
    gsl_rng_env_setup();
    T = gsl_rng_default;
    r = gsl_rng_alloc (T);
    gsl_rng_set(r, time(0));
    // setRandom(r);
    printf("set");
    if (f == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }

    setZeroCodeword(code, code_size);
    setZeroCodeword(decoded, code_size);
    PolarEncoder(code, code_size, encode);

    linspace(x_dB, -20, 15, sample_snr);
    dB2Linear(x_dB, x, sample_snr);
    fprintPyArray(f, "SNRvec_db", x_dB, sample_snr);
    for (int i = 0; i < sample_snr; i++)
    {
        printf("%d", i);
        for (int k = 0; k < code_size; k++)
        {
            entropy_channel[k] = 0;
            entropy_decode[k] = 0;
        }
        for (int j = 0; j < sample_llr; j++)
        {
            Code2LLRWithSNR(encode, channel_llr, code_size, x[i], r);
            PrintLLR(channel_llr, code_size);
            SC_decoder(code_size, decoded, channel_llr, frozen_index, frozen_size, frozen_value, encode_assume, decode_llr);
            for (int k = 0; k < code_size; k++)
            {
                entropy_decode[k] += log2(1+exp(-decode_llr[k]))/sample_llr;
                entropy_channel[k] += log2(1+exp(-channel_llr[k]))/sample_llr;
            }
        }
        
        capacity_upper[i] = 1 - entropy_decode[0];
        capacity_lower[i] = 1 - entropy_channel[1];
        
    }
    fprintPyArray(f, "upper_capacity", capacity_upper, sample_snr);
    fprintPyArray(f, "lower_capacity", capacity_lower, sample_snr);
       
    fclose(f);
    return 0;
}

void PrintLLR(double* llr, int channel_size){
    printf("the llr is:\n");
    for (int i = 0; i < channel_size; i++)
    {
        printf("%lf ", llr[i]);
    }
    printf("\n");
}

double LLRUpper(double llr_upper, double llr_lower){
    return log((exp(llr_upper+llr_lower)+1)/(exp(llr_upper)+exp(llr_lower)));
}

double LLRUpperMaxStar(double llr_upper, double llr_lower){
    return fmax(0, llr_lower+llr_upper) - fmax(llr_upper, llr_lower) + log(1 + exp(-fabs(llr_lower + llr_upper))) - log(1 + exp(-fabs(llr_upper-llr_lower)));
}

void SetFrozenBits2Code(int* decoded, int code_size, int* frozen_index, int frozen_size, int* frozen_value){
    for (int i = 0; i < code_size; i++)
    {
        decoded[i] = -1;
    }
    for (int i = 0; i < frozen_size; i++)
    {
        decoded[frozen_index[i]] = frozen_value[i];
    }  
}

double LLRUpperHardwareFriendly(double llr_upper, double llr_lower){
    int signa = (llr_upper > 0 ? 1 : -1);
    int signb = (llr_lower > 0 ? 1 : -1);
    if (signa == signb)
    {
        return signa > 0 ? (llr_upper < llr_lower ? llr_upper : llr_lower) : (llr_upper > llr_lower ? -llr_upper : -llr_lower);
    }else{
        return signa > 0 ? (llr_upper < -llr_lower ? -llr_upper : llr_lower) : (-llr_upper < llr_lower ? llr_upper : -llr_lower);
    }
}

double LLRLower(double llr_upper, double llr_lower, int u)
{
    switch (u)
    {
    case 0:
        return llr_upper + llr_lower;
        break;
    
    case 1:
        return llr_lower - llr_upper;
    
    default:
        printf("error about u");
        break;
    }
    return NAN;
}

int HardDecisionWithLLR(double llr){
    if(llr >= 0){
        return 0;
    }else
    {   
        return 1;
    }      
}


void recursive_caluelation(int code_size, double* channel_llr, int* decoded_code, int* encode_assume, double* decode_llr){
    int sub_channel_size = code_size/2;
    if(code_size == 1)
    { 
        if (decoded_code[0] == -1) //unfrozen bit
        {
            decoded_code[0] = HardDecisionWithLLR(channel_llr[0]);
        }
        encode_assume[0] = decoded_code[0];
        decode_llr[0] = channel_llr[0];
        return;
    }
    if(code_size%2 != 0){
        printf("the size is : %d\n", code_size);
        printf("code size unavaliable!!\n");
    }

    double llr_subchannel[sub_channel_size];
    for (int i = 0; i < sub_channel_size; i++)
    {
        llr_subchannel[i] = LLRUpperMaxStar(channel_llr[i], channel_llr[i+sub_channel_size]);
    }
    recursive_caluelation(sub_channel_size, llr_subchannel, decoded_code, encode_assume, decode_llr);

    for (int i = 0; i < sub_channel_size; i++)
    {
        llr_subchannel[i] = LLRLower(channel_llr[i], channel_llr[i+sub_channel_size], encode_assume[i]);
    }
    recursive_caluelation(sub_channel_size, llr_subchannel, decoded_code+sub_channel_size, encode_assume+sub_channel_size, decode_llr+sub_channel_size);
    
    for (int i = 0; i < sub_channel_size; i++)
    {
        encode_assume[i] = (encode_assume[i] + encode_assume[i+sub_channel_size])%2;
    }    
    
}

void SC_decoder(int code_size, int* decoded_code,double* channel_llr, int* frozen_index,int frozen_size, int* frozen_value, int* encode_assume, double* decode_llr){
    // SetFrozenBits2Code(decoded_code, code_size, frozen_index, frozen_size, frozen_value);
    recursive_caluelation(code_size, channel_llr, decoded_code, encode_assume, decode_llr);
    return;
}

void PolarEncoder(int* code, int code_size, int* encoded){
    if(code_size == 1){
        encoded[0] = code[0];
        return;
    }
    PolarEncoder(code+code_size/2, code_size/2, encoded+code_size/2);
    PolarEncoder(code, code_size/2, encoded);
    
    for (int i = 0; i < code_size/2; i++)
    {
        encoded[i] = (encoded[i] + encoded[i+code_size/2])%2;
    }    
    return;
}
