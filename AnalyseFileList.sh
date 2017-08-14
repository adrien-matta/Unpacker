#! /bin/bash

for file in $(cat $3)
do
	run='run'
	file='run'$file
	echo ------------------------------ Files: $1$file

	for i in $1$file*.mid
	do
		DATANAME=${i/$1/$2}
		DATANAME=${DATANAME/run/data}
		DATANAME=${DATANAME/mid/root}
		#SIZE=$(du $i | awk '{ print $1 }')
		if [ "$(uname)" == "Darwin" ]; then
		    SIZE=$(stat -f%z "$i")       
		elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
            SIZE=$(stat -c%s "$i")
		fi
		echo ------------------------------ Working on $i, size: $SIZE

	if [ -f $DATANAME ]
		then echo ------------------------------ $i already treated, ignored
	else 
		if [ "$SIZE" -lt 100000000 ]; then
			echo ------------------------------ too small size: $SIZE, ignored
		else
		echo treating $i
		#./unpacker --verbose 0  $i --user-output-tree-name rawdata    --user-output-path $2  
		./unpacker  --verbose 0  $i --user-output-tree-name rawdata    --user-output-path $2   --bank-output-tree-name bankdata   --bank-output-path  $2 
		#./unpacker  -v 0    $i   -ut rawdata    -up $2    -bt bankdata    -bp $2    -il 1000000 -ml 1000000
		fi
	fi
	done
done


# Examples run on terminal:
# ./AnalyseList.sh /Users/moukaddam/Work/TRIUMF/S1719/MidasData/   /Users/moukaddam/Work/TRIUMF/S1719/RawData/   listofAllRuns.txt
# ./AnalyseList.sh /Volumes/ne23dp/MidasData/   /Volumes/ne23dp/RawData/   listofAllRuns.txt
# nohup sh AnalyseList.sh /Volumes/ne23dp/MidasData/   /Volumes/ne23dp/RawData/  listofAllRuns.txt >/dev/null 2>&1    &
# nohup sh AnalyseList.sh /Volumes/ne23dp/MidasData/   /Volumes/ne23dp/RawData/  listofAllRuns.txt   &

