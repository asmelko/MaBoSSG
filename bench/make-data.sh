#!/bin/bash

root_dir=$(dirname $(pwd))
data_dir=data

mkdir $data_dir

for i in {10..100..10}
do 
  python3 ${root_dir}/data/generate-synth.py --signal_length 4 --nodes $i ${data_dir}/synth-100t-${i}n-4f-1000000s
done

for i in {200..1000..100}
do 
  python3 ${root_dir}/data/generate-synth.py --signal_length 4 --nodes $i ${data_dir}/synth-100t-${i}n-4f-1000000s
done

for i in {1000000..10000000..1000000}
do 
  python3 ${root_dir}/data/generate-synth.py --signal_length 4 --nodes 100 --sample_count $i ${data_dir}/synth-100t-100n-4f-${i}s
done

for i in {100..1000..100}
do 
  python3 ${root_dir}/data/generate-synth.py --signal_length 4 --nodes 100 --max_time $i ${data_dir}/synth-${i}t-100n-4f-1000000s
done

for i in {4..49..5}
do 
  python3 ${root_dir}/data/generate-synth.py --signal_length $i --nodes 100 ${data_dir}/synth-100t-100n-${i}f-1000000s
done

for i in {200..1000..200}
do 
  for j in {2000000..10000000..2000000}
  do
    for k in 4 24 49
    do
      python3 ${root_dir}/data/generate-synth.py --signal_length $k --nodes $i --sample_count $j ${data_dir}/synth-100t-${i}n-${k}f-${j}s
    done
  done
done