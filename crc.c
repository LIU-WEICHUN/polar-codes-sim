#include<stdio.h>
#include<assert.h>

void divise(int* dividend, int dend_size, int* divisor, int sor_size, int* remainder){
    int div[dend_size];
    for (int i = 0; i < dend_size; i++)
    {
        div[i] = dividend[i];
    }
    for (int i = 0; i < dend_size - sor_size +1; i++)
    {  
        if (div[i] == 0)
        {
            continue;
        }
        
        for (int j = 0; j < sor_size; j++)
        {
            div[i+j] = (div[i+j] + divisor[j])%2;
        }
        assert(div[i] == 0);
    }
    for (int i = 0; i < sor_size; i++)
    {
        remainder[i] = div[i+dend_size-sor_size];
    }
    
    return ;    
}
void crcEncoder(int* code, int code_size, int* encode){

    // int crc24a[] = {24, 23, 18, 17, 14, 11, 10, 7, 6, 5, 4, 3, 1, 0};
    // int crc24b[] = {24, 23, 6, 5, 1, 0};
    // int crc24c[] = {24, 23, 21, 20, 17, 15, 13, 12, 8, 4, 2, 1, 0};
    // int crc16[] = {16, 12, 5, 0};
    // int crc11[] = {11, 10, 9, 5, 0};
    // int crc6[] = {6, 5, 0};
    // int crc[] = {24, 23, 21, 20, 17, 15, 13, 12, 8, 4, 2, 1, 0};
    int crc[] = {3,1,0};
    
    int crc_size = 3;
    
    int sor_size = crc[0]+1;
    int tmp[code_size];
    for (int i = 0; i < code_size; i++)
    {
        tmp[i] = code[i];
        encode[i] = code[i];
    }
    
    for (int i = 0; i < code_size - sor_size +1; i++)
    {  
        if (tmp[i] == 0)
        {
            continue;
        }
        
        for (int j = 0; j < crc_size; j++)
        {
            tmp[i+sor_size-crc[j]-1] = ( tmp[i+sor_size-crc[j]-1] + 1)%2;
        }
        assert(tmp[i] == 0);
        printf("\n");
        for (int j = 0; j < code_size; j++)
        {
            printf("%d", tmp[j]);
        }
        
    }
    for (int i = 0; i < sor_size; i++)
    {
        encode[i+code_size-sor_size] += tmp[i+code_size-sor_size];
    }
    
    return ;    
}

int main(int argc, char const *argv[])
{
    // int crc24a[] = {24, 23, 18, 17, 14, 11, 10, 7, 6, 5, 4, 3, 1, 0};
    // int crc24b[] = {24, 23, 6, 5, 1, 0};
    // int crc24c[] = {24, 23, 21, 20, 17, 15, 13, 12, 8, 4, 2, 1, 0};
    // int crc16[] = {16, 12, 5, 0};
    // int crc11[] = {11, 10, 9, 5, 0};
    // int crc6[] = {6, 5, 0};

    int code[] = {1,1,0,1,0,0,1,1,1,0,1,1,0,0,1,0,0};
    int divisor[] = {1,0,1,1};
    int remainder[4];
    int encode[17];
    crcEncoder(code, 17, encode);
    for (int i = 0; i < 17; i++)
    {
        printf("%d", encode[i]);
    }
    
    

    return 0;
}
