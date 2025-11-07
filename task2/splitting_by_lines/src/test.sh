#!/bin/bash
echo "size,processes,time" > matrix_results.csv

for size in 100 200 400 800 1600 6400; do
  for proc in 1 2 4 8 10 12 14; do
    result=$(mpiexec --oversubscribe -n $proc ./main $size | tail -n 1)
    echo "$result" | awk '{print $1 "," $2 "," $3}' >> matrix_results.csv
  done
done

