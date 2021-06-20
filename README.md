# polar-codes-sim



### code for generate llr exp :
```matlab
sigma = sqrt(0.5 * (n/k) * 10^(-SNR_Eb_N0(snrIdx)/10));
a = zeros(1, n);
tx = (1 - 2 * a);
y = tx + sigma * randn(1, n);
llr = 2* y / (sigma^2);
```
