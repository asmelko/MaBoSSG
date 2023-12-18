#!/bin/bash

root_dir=$(dirname $(pwd))
out_file=gpu_out_real
data_dir=../data

rm -f ${out_file}

function runonce {
  for r in {1..5}
  do
    printf "$2;$3;$4;" >> ${out_file}
    ${root_dir}/build/MaBoSSG ${1}.bnd ${1}.cfg  2>&1 | grep "main>" | tail -n3 | cut -c52-62 | awk '{$1=$1;print}' | tr '\n' ';' >> ${out_file}
    echo >> ${out_file}
  done
}

runonce ${data_dir}/cellcycle cellcycle 10 1000000
runonce ${data_dir}/metastasis metastasis 32 1000
runonce ${data_dir}/Montagud2022_Prostate_Cancer Montagud 133 1000000
runonce ${data_dir}/sizek sizek 87 1000000
