#!/bin/bash

total=0
count=0

while IFS= read -r line; do
   total=$((total + line))
   count=$((count + 1))
done

if [ $count -ne 0 ]; then
   average=$((total / count))
   echo "Количество аргументов: $count"
   echo "Среднее значение: $average"
else
   echo "Нет данных для обработки."
fi
 