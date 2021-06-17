#include <stdio.h>
#include <stdlib.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <time.h>
#include <math.h>

void printDoubleArray(double* arr, int arr_size){
    printf("print array star:\n");
    for (int i = 0; i < arr_size; i++)
    {
        printf("%lf\n", arr[i]);
    }
    printf("print array end----\n");
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
    // printDoubleArray(rx, tx_size);
    return;
    
}
void tx2LlrForBpsk(double* rx, int rx_size, double* channel_llr, double snr){
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
        tx2LlrForBpsk(rx, code_size, channel_llr, snr);
        printf("snr = %lf, llr = %lf\n", snr, channel_llr[0]);
    }
    
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
double expectation(double* val_arr, int arr_size){
    double sum=0;
    for (int i = 0; i < arr_size; i++)
    {
        sum += val_arr[i]/arr_size;
    }
    return sum;
}

void outputPythonPlotFile(double* x, double* y, int size){
    FILE *f = fopen("plot.py", "w");
    if (f == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }

    /* print some text */
    fprintf(f, "SNRvec_dB = [");
    fprintf(f, "%lf ", x[0]);
    for (int i = 1; i < size; i++)
    {
        fprintf(f, ",%lf ", x[i]);
    }
    fprintf(f, "]\n");

    fprintf(f, "C = [");
    fprintf(f, "%lf ", y[0]);
    for (int i = 1; i < size; i++)
    {
        fprintf(f, ",%lf ", y[i]);
    }
    fprintf(f, "]\n");    
    
    fprintf(f, "# plot and save fig\n");
    fprintf(f, "import matplotlib.pyplot as plt\n");
    fprintf(f, "plt.grid()\n");
    fprintf(f, "plt.xlabel(\"SNR(dB)\")\n");
    fprintf(f, "plt.plot(SNRvec_dB, C, linewidth = 0.7, label = 'llr mutual info')\n");
    fprintf(f, "plt.legend()\n");
    fprintf(f, "plt.savefig(\"./fig.png\")\n");

    fclose(f);

}

int main(int argc, char const *argv[])
{
    int sample_snr = 1000;
    int sample_llr = 100000;
    double x_dB[sample_snr];
    double x[sample_snr];
    double channel_llr[sample_llr];
    int code[sample_llr];
    double information[sample_llr];
    double capacity[sample_snr];
    for (int i = 0; i < sample_llr; i++)
    {
        code[i] = 0;
    }

    linspace(x_dB, -20, 10, sample_snr);
    // printDoubleArray(x_dB, sample);
    dB2Linear(x_dB, x, sample_snr);
    for (int i = 0; i < sample_snr; i++)
    {
        Code2LLRWithSNR(code, channel_llr, sample_llr, x[i]);
        for (int j = 0; j < sample_llr; j++)
        {
            information[j] = log2(1+exp(-channel_llr[j]));
        }
        // printDoubleArray(information, 5);
        capacity[i] = 1 - expectation(information, sample_llr);
        // printf("x[i] = %lf, E[info] = %lf\n", x[i], expectation(information, sample_snr));
    }
    
    outputPythonPlotFile(x_dB, capacity, sample_snr);
    

    return 0;
}
