#include "polar.h"

double LLRUpper(double llr_upper, double llr_lower){
    return log((exp(llr_upper+llr_lower)+1)/(exp(llr_upper)+exp(llr_lower)));
}

double LLRUpperMaxStar(double llr_upper, double llr_lower){
    return fmax(0, llr_lower+llr_upper) - fmax(llr_upper, llr_lower) + log(1 + exp(-fabs(llr_lower + llr_upper))) - log(1 + exp(-fabs(llr_upper-llr_lower)));
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
        printf("error about u, u = %d\n", u);
        break;
    }
    return NAN;
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


void SetFrozenBits2Code(int* decoded, int code_size, int* unfrozen_index, int unfrozen_size, int* frozen_value){
    for (int i = 0; i < code_size; i++)
    {
        decoded[i] = frozen_value[i];
    }
    for (int i = 0; i < unfrozen_size; i++)
    {
        decoded[unfrozen_index[i]] = -1;
    }  
}

void setMessage2Code(int* message, int message_size, int* code,int code_size, int* unfrozen_index, int* frozen_value){
    for (int i = 0; i < code_size; i++)
    {
        code[i] = frozen_value[i];
    }
    for (int i = 0; i < message_size; i++)
    {
        code[unfrozen_index[i]] = message[i];
    } 
    return;
}