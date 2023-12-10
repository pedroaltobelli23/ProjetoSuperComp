#!/bin/bash
#SBATCH --output=outputs/Serial-%j.out
#SBATCH --job-name=job
#SBATCH --nodes=1
#SBATCH --partition=normal
#SBATCH --mem=1G

echo Output do job $SLURM_JOB_ID
./executavel_serial