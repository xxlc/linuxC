#!/bin/bash

for x in one two three four
do
	echo number $x
done

echo "---------------------------"

for myfile in /home/chenlei/*
do
	if [ -d "$myfile" ];then
		echo "$myfile is dir"
	else
		echo "$myfile is file"
	fi
done

echo "---------------------------"

for x in /var/log/*
do
	echo "basename $x is a file living in /var/log"
done
