#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <string.h>
#include <time.h>

double LLRUpper(double llr_upper, double llr_lower);
double LLRLower(double llr_upper, double llr_lower, int u);
void recursive_caluelation(int code_size,double* channel_llr, int* decoded_code, int* encode_assume);
void SC_decoder(int code_size, int* decoded_code,double* channel_llr, int* encode_assume);
void PolarEncoder(int* code, int code_size, int* encoded);
void Code2LLRWithSNR(int* encode, double* channel_llr, int code_size, double SNR);
void PrintCode(int* code, int code_size);
void PrintLLR(double* llr, int channel_size);

int main(int argc, char const *argv[])
{   
    int code_size = atoi(argv[1]);
    int encode[code_size];
    int code[code_size];
    int decoded[code_size];
    int encode_assume[code_size];
    double channel_llr[code_size];
    int show = 0;
    // for (int i = 0; i < code_size; i++)
    // {
    //     code[i] = atoi(argv[2+i]);
    // }
    for (int i = 0; i < code_size; i++)
    {
        code[i] = 0;
    }

    if (show == 1)
    {
        printf("code:");
        PrintCode(code, code_size);
    }
 
    PolarEncoder(code, code_size, encode);
    if (show == 1)
    {   
        printf("\n\nencode:");
        PrintCode(encode, code_size);
    }   

    Code2LLRWithSNR(encode, channel_llr, code_size, -1);
    PrintLLR(channel_llr, code_size);

    SC_decoder(code_size, decoded, channel_llr, encode_assume);
    
    printf("decoded:");
    PrintCode(decoded, code_size);

    printf("\n\nencode assume:");
    PrintCode(encode_assume, code_size);
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
/*
double LLRUpper(double llr_upper, double llr_lower){
    return log(exp(llr_upper+llr_lower)+1) - log(exp(llr_upper)+exp(llr_lower));
}*/
// double LLRUpper(double llr_upper, double llr_lower){
//     return log(exp((llr_upper+llr_lower)+1)/(exp(llr_upper)+exp(llr_lower)));
// }

double LLRUpper(double llr_upper, double llr_lower){
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


void recursive_caluelation(int code_size, double* channel_llr, int* decoded_code, int* encode_assume){
    int sub_channel_size = code_size/2;
    if(code_size == 1)
    { 
        decoded_code[0] = HardDecisionWithLLR(channel_llr[0]);
        encode_assume[0] = decoded_code[0];
        PrintLLR(channel_llr, code_size);
        return;
    }
    if(code_size%2 != 0){
        printf("the size is : %d\n", code_size);
        printf("code size unavaliable!!\n");
    }

    double llr_subchannel[sub_channel_size];
    for (int i = 0; i < sub_channel_size; i++)
    {
        llr_subchannel[i] = LLRUpper(channel_llr[i], channel_llr[i+sub_channel_size]);
    }
    // printf("Upper ");
    // PrintLLR(llr_subchannel, sub_channel_size);
    recursive_caluelation(sub_channel_size, llr_subchannel, decoded_code, encode_assume);

    for (int i = 0; i < sub_channel_size; i++)
    {
        llr_subchannel[i] = LLRLower(channel_llr[i], channel_llr[i+sub_channel_size], encode_assume[i]);
    }
    // printf("Lower ");
    // PrintLLR(llr_subchannel, sub_channel_size);
    recursive_caluelation(sub_channel_size, llr_subchannel, decoded_code+sub_channel_size, encode_assume+sub_channel_size);
    for (int i = 0; i < sub_channel_size; i++)
    {
        encode_assume[i] = (encode_assume[i] + encode_assume[i+sub_channel_size])%2;
    }    
    
}

void SC_decoder(int code_size, int* decoded_code,double* channel_llr, int* encode_assume){
    recursive_caluelation(code_size, channel_llr, decoded_code, encode_assume);
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
    double sigma = sqrt(0.5 / snr);
    for (int i = 0; i < tx_size; i++)
    {
        rx[i] = tx[i] + gsl_ran_gaussian(r, sigma);
    }
    return;
    
}
void tx2LlrForBpsk(double* rx, int rx_size, double* channel_llr, double snr){
    for (int i = 0; i < rx_size; i++)
    {
        channel_llr[i] = 4*rx[i]*snr;
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
        tx2LlrForBpsk(rx, code_size, channel_llr, snr);
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