#!/bin/bash
#SBATCH --output=outputs/MPI-%j.out
#SBATCH --job-name=job
#SBATCH --nodes=2
#SBATCH --ntasks-per-node=3
#SBATCH --partition=normal
#SBATCH --mem=1G

echo Output do job $SLURM_JOB_ID
mpirun -np 6 --allow-run-as-root ./executavel
