#include <stdio.h>
#include <stdlib.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <time.h>


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

int main(int argc, char const *argv[])
{
    double a, b;
    const gsl_rng_type * T;
    gsl_rng * r;
  /* create a generator chosen by the
     environment variable GSL_RNG_TYPE */
    gsl_rng_env_setup();
    T = gsl_rng_default;
    r = gsl_rng_alloc (T);
    gsl_rng_set(r, time(0));

    
    for(int i=0; i < 10 ; i++)
    {   
        a = gsl_ran_gaussian(r, 2);
        b = gsl_ran_gaussian(r, 2);   
        printf("%lf, %lf = %lf\n",a, b, LLRUpper(a, b));
    }
    
    return 0;
}
