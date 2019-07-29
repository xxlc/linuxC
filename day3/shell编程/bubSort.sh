#!/bin/bash

a=(3 10 6 5 9 2 8 1 4 7)

for ((i=9; i>0; i--))
do
	for ((j=0; j<i; j++))
	do
		if [ ${a[j]} -gt ${a[j+1]} ];then
			temp=${a[j]}
			a[j]=${a[j+1]}
			a[j+1]=$temp
		fi
	done
done

for ((i=0; i<10; i++))
do
	echo ${a[i]}
done

