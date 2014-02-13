#!/bin/bash
# The #PBS directives are the equivalent of passing the same argument on
# the qsub command line.

#PBS -N converter
#PBS -l nodes=1:ppn=1
#PBS -l walltime=48:00:00
#PBS -M jvictors@jessevictors.com

#PBS -j oe
#PBS -m abe

. /rc/tools/utils/dkinit

multibrot/converter/converter
