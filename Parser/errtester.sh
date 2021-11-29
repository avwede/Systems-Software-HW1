#!/bin/bash

make
compiled=$?
if [[ $compiled != 0 ]]; then
	echo "does not compile"
	exit 1
fi

echo "Compiles"

for i in {1..19}
do
	echo -n "Error $i : "

	filename="error$i.txt"
	outcheck="outerror$i.txt"
	./a.out testcases/${filename} > output.txt
	executed=$?
	if [[ $executed !=  0 ]]; then
		echo ":'("
	else
		diff -w -B output.txt testcases/${outcheck} &> /dev/null
		correct=$?
		if [[ $correct != 0 ]]; then
			echo ":'("
		else
			echo "───==≡≡ΣΣ((( つºل͜º)つ"
		fi
	fi
done
