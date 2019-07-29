#!/bin/bash
#this is my first shell project
read dir
mkdir ${dir}
cp -rf /etc/*.conf ${dir}
ls -l /etc/*|grep ^d|wc -l >etcdir.txt
