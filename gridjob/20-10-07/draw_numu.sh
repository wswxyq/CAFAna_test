
MODE_VALUES=( 0 1 2 3 10 )
PDG_VALUES=( 111 211 2212 2112 11 13 15 -13)


for mode in "${MODE_VALUES[@]}"
do
    for pdg in "${PDG_VALUES[@]}"
    do
        cafe -qb draw_spectra_numuE_select.C ${mode} ${pdg}
    done
done
