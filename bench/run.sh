#!/bin/bash

root_dir=$(dirname $(pwd))
out_file=gpu_out
data_dir=data

rm -f ${out_file}

function runonce {
  for r in {1..5}
  do
    printf "$2;$3;$4;$5;" >> ${out_file}
    ${root_dir}/build/MaBoSSG ${1}.bnd ${1}.cfg  2>&1 | grep "main>" | tail -n3 | cut -c52-62 | awk '{$1=$1;print}' | tr '\n' ';' >> ${out_file}
    echo >> ${out_file}
  done
}

for i in {10..100..10}
do 
  runonce ${data_dir}/synth-100t-${i}n-4f-1000000s 100 $i 4 1000000
done

for i in {200..1000..100}
do 
  runonce ${data_dir}/synth-100t-${i}n-4f-1000000s 100 $i 4 1000000
done

for i in {1000000..10000000..1000000}
do 
  runonce ${data_dir}/synth-100t-100n-4f-${i}s 100 100 4 $i
done

for i in {100..1000..100}
do 
  runonce ${data_dir}/synth-${i}t-100n-4f-1000000s $i 100 4 1000000
done

for i in {4..49..5}
do 
  runonce ${data_dir}/synth-100t-100n-${i}f-1000000s 100 100 $i 1000000
done