#!/bin/bash

# Precisa ser feito dessa maneira porque da problema de segmentation fault
#Esse arquivo vao criar 
var=45

python3 graph_generation.py $var
cp grafo.txt grafos/grafo$var.txt
python3 findclickCorrect.py > outputs/saidacorreta$var.txt

# sbatch findclickMPI.slurm
# sbatch findclickSerial.slurm