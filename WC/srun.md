# the following command request a p100 node in Wilson CLuster 
## (ignore the warning: bash: /nashome/w/wus/.bashrc: Permission denied)

[Go to official website for a full list of the nodes](https://computing.fnal.gov/wilsoncluster/hardware/)

srun --pty --nodes=1 --partition gpu_gce --gres=gpu:1 --nodelist wcgpu02 bash

**or**

[recommanded config]
srun --unbuffered --pty -A nova --partition=gpu_gce \
     --time=08:00:00 \
     --nodes=1 --ntasks-per-node=1 --gres=gpu:1 \
     --nodelist wcgpu02 /bin/bash

module load singularity

export SINGULARITY_CACHEDIR=/scratch/.singularity/cache

cd /wclustre/nova/users/wus/LSTM/images

tar xf tensorflow_1_12_gpu.tar.bz2 --directory /scratch

mkdir /scratch/work

singularity shell --userns --nv \
    --workdir=/scratch/work \
    --home=/work1/nova/wus/ \
    /scratch/tensorflow_1_12_gpu/

# the mappped directory will store the change in container i.e. .bash_history, .bash_rc 