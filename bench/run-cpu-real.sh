#!/bin/bash

root_dir=$(dirname $(pwd))
out_file=cpu_out_real
data_dir=../data

c=`ls | wc -l`
cp ${out_file} ${out_file}$c
rm -f ${out_file}

function runonce {
  for t in 32 64
  do
    for r in {1..5}
    do
        printf "$2;$3;$4;$t;" >> ${out_file}
        ./MaBoSS_1100n ${1}.bnd -c ${1}.cfg -e thread_count=$t -o res | cut -f2 -d' ' | tr '\n' ';' >> ${out_file}
        echo >> ${out_file}
    done
  done
}

runonce ${data_dir}/cellcycle cellcycle 10 1000000
runonce ${data_dir}/metastasis metastasis 32 1000
runonce ${data_dir}/Montagud2022_Prostate_Cancer Montagud 133 1000000
runonce ${data_dir}/sizek sizek 87 1000000
