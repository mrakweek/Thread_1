#!/bin/bash

# Массив чисел процессов
procs=(1 2 4 6 8 10 12 14)
# Массив числа испытаний
trials=(100 1000 10000 100000 1000000)

# Файл для результатов
outfile="pi_results.csv"

# Заголовок CSV
echo "processes,trials,hits,pi,time" > $outfile

# Запуск экспериментов
for p in "${procs[@]}"; do
    for t in "${trials[@]}"; do
        mpiexec --oversubscribe -n $p ./main $t >> $outfile
    done
done

echo "All experiments done. Results saved in $outfile"

