#!/bin/bash
# gen part types convertion function from PartitionTypes.txt file

if [ $# -eq 1 ]; then
    partfile=$1
else
    partfile=PartitionTypes.txt
fi

awk '$1 ~ /[0-9a-fA-F]h/ { print $0 } ' $partfile
