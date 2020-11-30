#!/bin/bash

die ()
{
    echo "${*}"
    exit 1
}

usage ()
{
    cat <<EOF
Usage: submit CONF_FILE ACTION

where ACTION is either
    submit              submit job on the grid
    join                join job output files
    test                locally test configs
EOF
    exit 1
}

submit_jobs ()
{
    [[ -z "${SCRIPT}" ]] && die "No \${SCRIPT} variable set"

    CONFIG=(
        --njobs "${NJOBS:-100}"
        --print_jobsub
        --node-features=ssse3,sse4_1
        --drain
        "${JOB_OPTIONS[@]}"
    )

    for config_name in "${!CONFIGS[@]}"
    do
        outdir="${OUTDIR}/${config_name}"

        if [[ ${NJOBS} -eq 1 ]] && [[ -f "${outdir}/${RESULT_FILE}" ]]
        then
            continue
        fi

        mkdir -p "${outdir}" || die "Failed to make output directory"
        chmod g+w "${outdir}"

        submit_cafana.py "${CONFIG[@]}" --outdir "${outdir}" "${SCRIPT[@]}" \
            ${CONFIGS[${config_name}]}

        [[ -n "${SUBMIT_DELAY}" ]] && sleep "${SUBMIT_DELAY}"
    done
}

print_and_exec_cmd ()
{
    echo "Executing: ${*}"
    "${@}"
}

test_jobs ()
{
    [[ -z "${SCRIPT}" ]] && die "No \${SCRIPT} variable set"

    for config_name in "${!CONFIGS[@]}"
    do
        if [[ "${N_SKIP}" -gt 0 ]]
        then
            N_SKIP=$(( N_SKIP - 1 ))
            continue
        fi

        echo "Testing config: ${config_name}"
        print_and_exec_cmd cafe "${@}" -qb --limit 1 \
            "${SCRIPT[@]}" "${CONFIG[@]}" ${CONFIGS[${config_name}]}

        break
    done
}

join_results ()
{
    [[ -z "${RESULT_DIR}" ]]  && die "No \${RESULT_DIR} variable set"
    [[ -z "${RESULT_FILE}" ]] && die "No \${RESULT_FILE} variable set"
    [[ -z "${OUTDIR}" ]]      && die "No \${OUTDIR} variable set"

    local result_basename="${RESULT_FILE%.*}"
    local result_ext="${RESULT_FILE#${result_basename}}"

    local args_list=()

    for config_name in "${!CONFIGS[@]}"
    do
        echo "Joining files for '${config_name}'"

        local outdir="${OUTDIR}/${config_name}"
        local resultdir="${RESULT_DIR}/${config_name}"

        mkdir -p "${resultdir}"

        local files_to_join=(
            "${outdir}/${result_basename}."*_of_"${NJOBS}${result_ext}"
        )
        if [[ ${NJOBS} -eq 1 ]]
        then
            files_to_join="${outdir}/${RESULT_FILE}"
        fi

        local nfiles="${#files_to_join[@]}"

        if [[ "${nfiles}" -ne "${NJOBS}" ]]
        then
            echo "Number of files '${nfiles}' is not equal to" \
                 "the number of jobs '${NJOBS}'. Skipping..."
        else
            args_list+=( "${resultdir}/${RESULT_FILE}" "${files_to_join[@]}" )
        fi
    done

    local nargs="$(( NJOBS + 1 ))"

    printf "%s\0" "${args_list[@]}" \
        | xargs -t -P "${N_JOBS:-5}" -0 -n "${nargs}"  \
            hadd_cafana > /dev/null
}

conf_file="${1}"
action="${2}"

shift 2

[[ -z "${conf_file}" ]] && usage
[[ -z "${action}" ]]    && usage

. "${conf_file}" || die "Failed to imoprt config file"

[[ -z "${OUTDIR}" ]]          && die "OUTDIR is not defined in config file"
[[ "${#CONFIGS[@]}" -eq 0 ]]  && die "CONFIGS is not defined in config file"


case "${action}" in
    "submit")
        submit_jobs
        ;;
    "join")
        join_results
        ;;
    "test")
        test_jobs "${@}"
        ;;
    *)
        die "Unknown action"
        ;;
esac

