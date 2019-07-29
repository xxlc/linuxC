#!/bin/bash

num=4
if test "$num" == 3;then
	echo "num is 3."
else
	echo "num is not 3."
fi

a=4
if test $a -ge 3; then
	echo "$a >= 3."
fi
