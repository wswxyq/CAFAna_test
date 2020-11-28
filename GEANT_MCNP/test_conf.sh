#!/bin/bash

RESULT_DIR="./results/"
RESULT_FILE="spectra.root"

SCRIPT=( prong_length_original.C )
NJOBS=20

JOB_OPTIONS=(
    --rel     "S20-04-03"
    --testrel "/nova/app/users/wus/fake_release"
    #--os "sl6"
    --singularity "/cvmfs/singularity.opensciencegrid.org/fermilab/fnal-wn-sl6:latest"
)

OUTDIR="/pnfs/nova/scratch/users/wus/2020-11-27_wu_test"

MODE_VALUES=( 0 )


declare -A CONFIGS

for mode in "${MODE_VALUES[@]}"
do
    CONFIGS["${mode}"]="${mode}"
done

