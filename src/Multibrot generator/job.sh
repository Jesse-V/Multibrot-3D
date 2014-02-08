#!/bin/bash
# The #PBS directives are the equivalent of passing the same argument on
# the qsub command line.

#PBS -N multibrot
#PBS -l nodes=1:ppn=1
#PBS -l walltime=06:00:00
#PBS -M jvictors@jessevictors.com

#PBS -j oe
#PBS -m abe

. /rc/tools/utils/dkinit
# use OpenMPI-GCC-4.8

#LD_LIBRARY_PATH=/home/A01514050:$LD_LIBRARY_PATH
#export LD_LIBRARY_PATH

multibrot/multibrot
