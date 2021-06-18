# polar-codes-sim

* the oupout fig didn't fit the known capacity
    * the expection is out of the log (DONE)
    * ask boss if it correct

### code for generate llr exp :
```matlab
sigma = sqrt(0.5 * (n/k) * 10^(-SNR_Eb_N0(snrIdx)/10));
a = zeros(1, n);
tx = (1 - 2 * a);
y = tx + sigma * randn(1, n);
llr = 2* y / (sigma^2);
```