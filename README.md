# polar-codes-sim

## TODO
* scl_snr.c
    * looks alright, but didn't know why other PM are the same.

### code for generate llr exp :
```matlab
sigma = sqrt(0.5 * (n/k) * 10^(-SNR_Eb_N0(snrIdx)/10));
a = zeros(1, n);
tx = (1 - 2 * a);
y = tx + sigma * randn(1, n);
llr = 2* y / (sigma^2);
```

### relation between llr and information
mutual information(capacity?) = 1- expection(log(1+exp(-L)) )  
where L is llr.  
if it is `capacity`,  expection(log(1+exp(-L)) ) would be `entropy`. (according digicomm p362, 6.5-29).  
Because the `entropy` is mean of information, `log(1+exp(-L) )` might be `informaion`.  



## referance
[1] M. Chiu, "Interleaved Polar (I-Polar) Codes," in IEEE Transactions on Information Theory, vol. 66, no. 4, pp. 2430-2442, April 2020, doi: 10.1109/TIT.2020.2969155.