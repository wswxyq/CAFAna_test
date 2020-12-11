#!/bin/bash

RESULT_DIR="./results/"
RESULT_FILE="spectra.root"

SCRIPT=( Resolution.C )
NJOBS=50

JOB_OPTIONS=(
    --rel     "S20-10-30"
    --testrel "/nova/app/users/wus/fake_release_10-30"
)

OUTDIR="/pnfs/nova/scratch/users/wus/2020-12-11-resolution_MCNP"

MODE_VALUES=( 100000 )


declare -A CONFIGS

for mode in "${MODE_VALUES[@]}"
do
    CONFIGS["${mode}"]="${mode}"
done