#!/bin/bash
echo "size,processes,time" > matrix_results.csv

for size in 5000 7500 10000 12500 15000 17500 20000; do
  for proc in 1 2 4 8 10 12 14; do
    result=$(mpiexec --oversubscribe -n $proc ./main $size | tail -n 1)
    echo "$result" | awk '{print $1 "," $2 "," $3}' >> matrix_results.csv
  done
done

