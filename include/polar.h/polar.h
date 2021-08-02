#include<math.h>
#include<stdio.h>
double LLRUpperHardwareFriendly(double llr_upper, double llr_lower);
double LLRUpper(double llr_upper, double llr_lower);
double LLRUpperMaxStar(double llr_upper, double llr_lower);
double LLRLower(double llr_upper, double llr_lower, int u);

void PolarEncoder(int* code, int code_size, int* encoded);
void SetFrozenBits2Code(int* decoded, int code_size, int* frozen_index, int unfrozen_size, int* frozen_value);
void setMessage2Code(int* message, int message_size, int* code,int code_size, int* unfrozen_index, int* frozen_value);