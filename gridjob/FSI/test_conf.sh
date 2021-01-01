#!/bin/bash

RESULT_DIR="./results/"
RESULT_FILE="spectra.root"

SCRIPT=( exec_FSI.C )
NJOBS=50

JOB_OPTIONS=(
    --rel     "S20-10-30"
    --testrel "/nova/app/users/wus/fake_release_10-30"
)

OUTDIR="/pnfs/nova/scratch/users/wus/2020-10-07_wu_test"

MODE_VALUES=( 100000 )
FSI_VALUES=( 0 1 2 3 )
PM_VALUES=( 1 -1 2 -2 )

declare -A CONFIGS

for mode in "${MODE_VALUES[@]}"
do
    for fsi in "${FSI_VALUES[@]}"
    do
        for pm in "${PM_VALUES[@]}"
        do
            CONFIGS["${mode}_${fsi}_${pm}"]="${mode} ${fsi} ${pm}"
        done
    done
done

