#!/usr/bin/env bash

trap ctrl_c INT

function ctrl_c()
{
	exit
}

rm -Rf out.txt

xres=1000
yres=1000

rm -Rf out.txt
echo $xres $yres >> out.txt
for i in {1..10}
do
for thread in {1..5}
	do
		/usr/bin/time --append -o out.txt -f "$thread %e" ./raytracer -w $xres -h $yres -t $thread
	done
done
