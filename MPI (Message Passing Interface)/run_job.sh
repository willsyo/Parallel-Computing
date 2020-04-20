#!/bin/bash
#PBS -l nodes=4:ppn=4:physical
#PBS -l walltime=00:10:00
#PBS -o /home/Students/twillis/Assignment#6_Willis/job_out
#PBS -j oe
#PBS -N lab3

date
export PROGRAM="/home/Students/twillis/Assignment#6_Willis/lab3"
mpirun -machinefile $PBS_NODEFILE $PROGRAM 1024
date
exit 0