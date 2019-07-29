#!/bin/bash

read file
if [ -f $file -a -r $file -a -w $file ];then #判断文件是否可读可写
	cat $file
elif [ -d $file ];then	#判断文件是否是目录文件
	cd $file
	if [ -e ls.sh ];then	#判断ls.sh文件是否存在
		chmod +x ls.sh
		./ls.sh
	else
		touch ls.sh
		echo "#!/bin/bash" >>ls.sh
		echo "ls -li /etc >etc.list" >>ls.sh
		chmod +x ls.sh
		./ls.sh
	fi
fi
