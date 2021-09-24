#!/bin/bash
if ! [[ $1 =~ ^-?[0-9]+$ ]] || ! [[ $2 =~ ^-?[0-9]+$ ]] ; then
    echo "Please check your arguemt. You must pass argument which is both INTEGER."
    exit 127
fi


if [ $1 -lt 1 ] || [ $2 -lt 1 ] ; then
    echo "Please check your argument. You must pass argument which is both bigger than 0."
    exit 127
fi

for (( a=1; a<=$1; a++ ))
do
	for (( b=1; b<=$2; b++ ))
    do
        echo -n "$a * $b = `expr $a \* $b` "
    done
    echo
done


