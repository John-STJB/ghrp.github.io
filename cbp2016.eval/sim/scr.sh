./predictor ../traces/LONG_SERVER-4.bt9.trace.gz LRU &
./predictor ../traces/LONG_SERVER-4.bt9.trace.gz MRU &
./predictor ../traces/LONG_SERVER-4.bt9.trace.gz GHRP &
./predictor ../traces/LONG_SERVER-4.bt9.trace.gz RANDOM &
./predictor ../traces/LONG_SERVER-4.bt9.trace.gz RRIP &

for pid in $(jobs -p)
do
    wait $pid
done
