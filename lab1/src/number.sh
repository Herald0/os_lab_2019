#!/bin/bash

count=150

for i in $(seq 1 $count); do
    echo $((RANDOM % 101)) >> numbers.txt
done
