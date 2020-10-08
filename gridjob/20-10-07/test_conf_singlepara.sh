#!/bin/bash

RESULT_DIR="./results/"
RESULT_FILE="spectra.root"

SCRIPT=( prong_length_original.C )
NJOBS=20

JOB_OPTIONS=(
    --rel     "S20-04-03"
    --testrel "/nova/app/users/wus/fake_release"
)

OUTDIR="/pnfs/nova/scratch/users/wus/2020-10-07_wu_test"

MODE_VALUES=( 0 1 2 3 10 )


declare -A CONFIGS

for mode in "${MODE_VALUES[@]}"
do
    CONFIGS["${mode}_${pdg}_${pm}"]="${mode}"
done

