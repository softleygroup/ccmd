#!/bin/bash
PROGRAM=./bin/linux/ccmd_test 

WDS=(  "./eta_digital/ca_number_tau_0.226/ca_35")

i=0

while [ $i -lt ${#WDS[@]} ]
do
    runcount=`ps eax | grep ccmd_test | grep -v grep | wc -l`
    if [ 4 -gt ${runcount} ]
    then
        echo ${WDS[$i]}
	$PROGRAM ${WDS[$i]} &
        let "i++"

        # filebase=arbr32_BG_dr_${param[$i]}
        # sed 's/xxx/'${param[$i]}'/' < arbr32_BG_dr.in > ${filebase}.in
        # ${program} < ${filebase}.in >  ${filebase}.conv &
    else
        sleep 5
    fi
done

