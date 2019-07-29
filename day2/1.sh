#!/bin/bash
read dir
mkdir ${dir}
cp -rf /etc/*.conf ${dir}
ls -l /etc/*|grep ^d|wc -l >etcdir.txt
