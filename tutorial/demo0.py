# Make a simple spectrum plot

import cafana

# Environment variables and wildcards work. Most commonly you want a SAM
# dataset. Pass -ss --limit 1 on the cafe command line to make this take a
# reasonable amount of time for demo purposes.
fname = 'prod_sumdecaf_R17-03-01-prod3reco.k_fd_genie_nonswap_fhc_nova_v08_full_nue_or_numu_or_nus_contain_v1'

loader = cafana.SpectrumLoader(fname)

bins = cafana.Binning.Simple(100, 0, 1000)

# Arbitrary code to extract value from StandardRecord. Must be in C++ like this
kTrackLen = cafana.CVar('''
if(sr.trk.kalman.ntracks == 0) return 0.0f;
return float(sr.trk.kalman.tracks[0].len);
''')

# Spectrum to be filled from the loader
length = cafana.Spectrum("Track length (cm)", bins, loader, kTrackLen, cafana.kIsNumuCC)

# Do it!
loader.Go()

# How to scale histograms
pot = 18e20

# We have histograms
length.ToTH1(pot).Draw('hist')
