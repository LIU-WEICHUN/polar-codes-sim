#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include <string.h>
#include "channel_coding.h"
#include "polar.h"
#include "comm_smi.h"


void recursive_caluelation(int code_size,double* channel_llr, int* decoded_code, int* encode_assume, double* decode_llr);
void SC_decoder(int code_size, int* decoded_code,double* channel_llr, int* frozen_index,int frozen_size, int* frozen_value, int* encode_assume, double* decode_llr);

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
    int sample_snr = 100;
    int sample_llr = 10000;
    // sample_llr = 1;
    double x_dB[sample_snr];
    double x[sample_snr];
    double decode_llr[code_size];
    double entropy_decode[code_size];
    double entropy_channel[code_size];
    double capacity_channel[code_size];
    double capacity_decode[code_size];
    FILE *f = fopen("polar_snr0622.py", "w");
    if (f == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }

    setZeroCodeword(code, code_size);
    setZeroCodeword(decoded, code_size);
    PolarEncoder(code, code_size, encode);
    gsl_rng * r;

    const gsl_rng_type * T;
  /* create a generator chosen by the
     environment variable GSL_RNG_TYPE */
    gsl_rng_env_setup();
    T = gsl_rng_default;
    r = gsl_rng_alloc (T);
    gsl_rng_set(r, time(0));
    // setIndexArr(frozen_index ,0, code_size-1);
    // shuffle(frozen_index, code_size);
    // frozen_size = 0;
    // setZeroCodeword(frozen_value, code_size);
    // SetFrozenBits2Code(code, code_size, frozen_index, frozen_size, frozen_value);

    linspace(x_dB, -20, 10, sample_snr);
    dB2Linear(x_dB, x, sample_snr);
    fprintPyArray(f, "SNRvec", x, sample_snr);
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
            SC_decoder(code_size, decoded, channel_llr, frozen_index, frozen_size, frozen_value, encode_assume, decode_llr);
            for (int k = 0; k < code_size; k++)
            {
                entropy_decode[k] += log2(1+exp(-decode_llr[k]))/sample_llr;
                entropy_channel[k] += log2(1+exp(-channel_llr[k]))/sample_llr;
            }
        }
        for (int k = 0; k < code_size; k++)
        {
            capacity_decode[k] = 1 - entropy_decode[k];
            capacity_channel[k] = 1 - entropy_channel[k];
        }
        fprintf(f, "# sample number is: %d\n", sample_llr);
        fprintf(f, "# snr is: %lf\n", x[i]);
        fprintf(f, "a%d", i);
        fprintPyArray(f, "thSNR_capacity_channel", capacity_channel, code_size);
        fprintf(f, "a%d", i);
        fprintPyArray(f, "thSNR_capacity_decode", capacity_decode, code_size);
    }
    fclose(f);
    return 0;
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


