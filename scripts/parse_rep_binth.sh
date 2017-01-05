#!/bin/bash

DIR="/home/mrp/NetBeansProjects/RuleProgramming/REP_BINTH_LOG"

if [[ -z "$1" ]]; then
    echo "Please enter wr"
    exit
fi

WR="$1"

if [ ! -d "$DIR/$WR" ]; then
    echo "$DIR/$WR does not exist."
    exit
fi

cd "$DIR/$WR"

files=`ls .`

for file in $files
do
    echo $file
    cat "$file" | grep "REP_BINTH" | cut -d ',' -f 2
    echo ""
    echo ""
    read -n1 x
done