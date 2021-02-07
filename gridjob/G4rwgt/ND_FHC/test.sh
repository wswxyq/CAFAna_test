#!/bin/bash

RESULT_DIR="./results/"
RESULT_FILE="spectra.root"

SCRIPT=( LSTM_macro.C )
NJOBS=200

JOB_OPTIONS=(
    --rel     "development"
    --memory 3900
    --testrel "/nova/app/users/wus/fake_release_test_ND_FHC"
)

OUTDIR="/pnfs/nova/scratch/users/wus/2021-01-23_wu_test"

declare -A CONFIGS

CONFIGS["100000"]=""


