#!/bin/sh

for file in $1; do
	n=$( echo $file | grep -P -o '\d+(?=.tgf.txt)' )
	m=$( echo $file | grep -P -o '(?<=_m)\d+' )
	r=$( echo $file | grep -P -o '\d+(?=remotes)' )
	f=$( grep "Failure rate:" $file | cut -f2 )
	s=$( grep "Route Stretch" $file | cut -f2 )
	echo -e "$n\t$m\t$r\t$f\t$s"
done
