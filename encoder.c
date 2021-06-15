#include<stdio.h>
#include<stdlib.h>

void PolarEncoder(int* code, int code_size, int* encoded){
    if(code_size == 1){
        encoded[0] = code[0];
        return;
    }
    int* upper;
    int* lower;
    PolarEncoder(code+code_size/2, code_size/2, encoded+code_size/2);
    PolarEncoder(code, code_size/2, encoded);
    
    for (int i = 0; i < code_size/2; i++)
    {
        encoded[i] = (encoded[i] + encoded[i+code_size/2])%2;
    }    
    return;
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



int main(int argc, char const *argv[])
{
    int code_size = atoi(argv[1]);
    int encode[code_size];
    int code[code_size];
    for (int i = 0; i < code_size; i++)
    {
        code[i] = atoi(argv[2+i]);
    }
    
    PrintCode(code, code_size);
    PolarEncoder(code, code_size, encode);
    PrintCode(encode, code_size);


    return 0;
}
