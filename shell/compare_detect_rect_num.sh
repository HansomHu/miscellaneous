#!/bin/bash

dir1=./flame_detect
# dir2=./smoke_self_test_result/txt
dir2=../../datasets/fire_smoking_glove/fire/TESTSET_wgh/EvalLabels

label_list=$(cd ${dir1};ls)

for file in ${label_list[@]}; do
    file1=${dir1}/${file}
    file2=${dir2}/${file}
    if [ ! -e ${file2} ]; then
        echo "ERROR: no file: ${file2}"
	continue
    fi
    rect_num1=$(cat ${file1} | wc -l)
    rect_num2=$(cat ${file2} | wc -l)
    # echo "${rect_num1}:${rect_num2}"
    if [ ${rect_num1} -ne ${rect_num2} ]; then
	echo -e "\033[31m ERROR: rect diff (${rect_num1} : ${rect_num2}) ${file}\033[0m"
    fi
done
