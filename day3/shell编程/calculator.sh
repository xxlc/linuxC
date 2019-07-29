#!/bin/bash

echo "please input the first number."
read num1
echo "please input the second number."
read num2
echo "please input your operator"
read ope

result=0

if [ "+" == $ope ];then
	result=$(($num1 + $num2))
elif [ "-" == $ope ];then
	result=$(($num1 - $num2))
elif [ "*" == $ope ];then
	result=$(($num1 * $num2))
elif [ "/" == $ope ];then
	result=$(($num1 / $num2))
else
	result=$(($num1 % $num2))
fi

echo "$num1 $ope $num2 = $result"
