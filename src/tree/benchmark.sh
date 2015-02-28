#!/bin/bash

for dir in $(ls); do
	if [[ ! -d $dir ]]; then
		continue
	fi
	cd $dir
	echo $dir
	time make test > /dev/null
	make clean > /dev/null
	echo -------------------
	cd ..
done
