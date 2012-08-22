#!/bin/sh

for file in $1; do
	n=$( grep "Nodes:"            $file | cut -f2 )
	m=$( grep "Neighbour degree:" $file | cut -f2 )
	r=$( grep "Remote degree:"    $file | cut -f2 )
	f=$( grep "Failure rate:"     $file | cut -f2 )
	s=$( grep "Route Stretch"     $file | cut -f2 )
	echo -e "$n\t$m\t$r\t$f\t$s"
done
