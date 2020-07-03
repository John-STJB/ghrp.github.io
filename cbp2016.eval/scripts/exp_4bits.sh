/home/shyngys/cbp2016.eval/sim/predictor /home/shyngys/cbp2016.eval/traces/SHORT_MOBILE-91.bt9.trace.gz GHRP > /home/shyngys/cbp2016.eval/results/exp_4bits/SHORT_MOBILE-91.bt9.trace.gzGHRP.txt &
/home/shyngys/cbp2016.eval/sim/predictor /home/shyngys/cbp2016.eval/traces/SHORT_MOBILE-91.bt9.trace.gz RANDOM > /home/shyngys/cbp2016.eval/results/exp_4bits/SHORT_MOBILE-91.bt9.trace.gzRANDOM.txt &
/home/shyngys/cbp2016.eval/sim/predictor /home/shyngys/cbp2016.eval/traces/SHORT_MOBILE-91.bt9.trace.gz LRU > /home/shyngys/cbp2016.eval/results/exp_4bits/SHORT_MOBILE-91.bt9.trace.gzLRU.txt &
/home/shyngys/cbp2016.eval/sim/predictor /home/shyngys/cbp2016.eval/traces/SHORT_MOBILE-91.bt9.trace.gz RRIP > /home/shyngys/cbp2016.eval/results/exp_4bits/SHORT_MOBILE-91.bt9.trace.gzRRIP.txt &
/home/shyngys/cbp2016.eval/sim/predictor /home/shyngys/cbp2016.eval/traces/LONG_MOBILE-20.bt9.trace.gz GHRP > /home/shyngys/cbp2016.eval/results/exp_4bits/LONG_MOBILE-20.bt9.trace.gzGHRP.txt &
/home/shyngys/cbp2016.eval/sim/predictor /home/shyngys/cbp2016.eval/traces/LONG_MOBILE-20.bt9.trace.gz RANDOM > /home/shyngys/cbp2016.eval/results/exp_4bits/LONG_MOBILE-20.bt9.trace.gzRANDOM.txt &
/home/shyngys/cbp2016.eval/sim/predictor /home/shyngys/cbp2016.eval/traces/LONG_MOBILE-20.bt9.trace.gz LRU > /home/shyngys/cbp2016.eval/results/exp_4bits/LONG_MOBILE-20.bt9.trace.gzLRU.txt &
/home/shyngys/cbp2016.eval/sim/predictor /home/shyngys/cbp2016.eval/traces/LONG_MOBILE-20.bt9.trace.gz RRIP > /home/shyngys/cbp2016.eval/results/exp_4bits/LONG_MOBILE-20.bt9.trace.gzRRIP.txt &
for pid in $(jobs -p)
do
	wait $pid
done