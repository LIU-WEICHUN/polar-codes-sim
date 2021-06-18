#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include <string.h>
#include "channel_coding.h"

double LLRUpperHardwareFriendly(double llr_upper, double llr_lower);
double LLRUpper(double llr_upper, double llr_lower);
double LLRUpperMaxStar(double llr_upper, double llr_lower);
double LLRLower(double llr_upper, double llr_lower, int u);
void recursive_caluelation(int code_size,double* channel_llr, int* decoded_code, int* encode_assume);
void SC_decoder(int code_size, int* decoded_code,double* channel_llr, int* frozen_index,int frozen_size, int* frozen_value, int* encode_assume);
void PolarEncoder(int* code, int code_size, int* encoded);
void SetFrozenBits2Code(int* decoded, int code_size, int* frozen_index, int frozen_size, int* frozen_value);

int main(int argc, char const *argv[])
{   
    int code_size = atoi(argv[1]);
    int encode[code_size];
    int code[code_size];
    int decoded[code_size];
    int encode_assume[code_size];
    double channel_llr[code_size];
    int show = 0;
    int frozen_index[code_size];
    int frozen_size;
    int frozen_value[code_size];

    setZeroCodeword(code, code_size);
    PolarEncoder(code, code_size, encode);
    setIndexArr(frozen_index ,0, code_size-1);
    shuffle(frozen_index, code_size);
    frozen_size = code_size/2;
    setZeroCodeword(frozen_value, code_size);
    SetFrozenBits2Code(code, code_size, frozen_index, frozen_size, frozen_value);

    Code2LLRWithSNR(encode, channel_llr, code_size, -1);

    SC_decoder(code_size, decoded, channel_llr, frozen_index, frozen_size, frozen_value, encode_assume);
    
    //output result
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


void recursive_caluelation(int code_size, double* channel_llr, int* decoded_code, int* encode_assume){
    int sub_channel_size = code_size/2;
    if(code_size == 1)
    { 
        if (decoded_code[0] == -1) //unfrozen bit
        {
            decoded_code[0] = HardDecisionWithLLR(channel_llr[0]);
        }
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
        llr_subchannel[i] = LLRUpperMaxStar(channel_llr[i], channel_llr[i+sub_channel_size]);
        // llr_subchannel[i] = LLRUpper(channel_llr[i], channel_llr[i+sub_channel_size]);
        // llr_subchannel[i] = LLRUpperHardwareFriendly(channel_llr[i], channel_llr[i+sub_channel_size]);
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

void SC_decoder(int code_size, int* decoded_code,double* channel_llr, int* frozen_index,int frozen_size, int* frozen_value, int* encode_assume){
    SetFrozenBits2Code(decoded_code, code_size, frozen_index, frozen_size, frozen_value);
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
