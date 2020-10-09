# Used as temporary sync tool
# Do not use rsync to pull remote to local
# rsync will not delete files that do not exist in destination but source
rsync -chavzP --stats . wus@novagpvm05.fnal.gov:/nova/app/users/wus/CAFAna_test --exclude=.git