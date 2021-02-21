#!/bin/bash

RESULT_DIR="./results/"
RESULT_FILE="spectra.root"

SCRIPT=( exec_rwgt.C )
NJOBS=200

JOB_OPTIONS=(
    --rel     "development"
    --memory 3900
    --testrel "/nova/app/users/wus/fake_release_test_ND_FHC"
)

OUTDIR="/pnfs/nova/scratch/users/wus/2021-02-20_rwgt"


RW_VALUES=( 0 1 2 3 4 5 )
PM_VALUES=( 40 -40 )

declare -A CONFIGS


for fsi in "${RW_VALUES[@]}"
do
    for pctg in "${PM_VALUES[@]}"
    do
        CONFIGS["${fsi}_${pctg}"]="${fsi} ${pctg}"
    done
done