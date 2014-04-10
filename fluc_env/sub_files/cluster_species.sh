#!/bin/bash -login

AVIDA_CONFIG=$(readlink -e ../config_files)
FLUC_TYPE=sync
INFLOW_RATE=1

# sync: 1, 10, 100, 1000, 10000
# stag: 1, 10, 100, 1000, 10000

mkdir -p workdir_cs/analysis_files
mkdir -p workdir_cs/sub_files

for i in $(seq 1 30)
do
  for j in $(seq 100000 100000 400000)
  do
    CLUSTERS=$(ls -1 ../data_${FLUC_TYPE}_${INFLOW_RATE}/clustering_history/replicate_${i}/detail-${j}_c*.spop|wc -l)

    cat <<EOT1> workdir_cs/analysis_files/cluster_species_${FLUC_TYPE}_${INFLOW_RATE}_u${j}_r${i}.cfg
  FORRANGE c 0 $((CLUSTERS - 1))
    PURGE_BATCH
    LOAD ../data_${FLUC_TYPE}_${INFLOW_RATE}/clustering_history/replicate_${i}/detail-${j}_c\$c.spop
    RECALCULATE
    FILTER num_cpus > 0
    PRINT_PHENOTYPES clustering_history/replicate_${i}/phenotype-${j}_c\$c.dat
    DETAIL clustering_history/replicate_${i}/genotype-${j}_c\$c.dat id num_cpus task_list
    DETAIL clustering_history/replicate_${i}/task_nums-${j}_c\$c.data task.0 task.1 task.2 task.3 task.4 task.5 task.6 task.7 task.8
  END
EOT1

    cat <<EOT2> workdir_cs/sub_files/cluster_species_${FLUC_TYPE}_${INFLOW_RATE}_u${j}_r${i}.sub
#!/bin/bash -login

#PBS -l walltime=00:10:00,nodes=1:ppn=1,mem=100mb
#PBS -j oe

set -exu

### load necessary modules, e.g.
module load avida

### change to the working directory where your code is located
cd ${AVIDA_CONFIG}

### call your executable
avida -a -set DATA_DIR ../data_${FLUC_TYPE}_${INFLOW_RATE} -set EVENT_FILE events_${FLUC_TYPE}_${INFLOW_RATE}.cfg -set ANALYZE_FILE ../sub_files/workdir_cs/analysis_files/cluster_species_${FLUC_TYPE}_${INFLOW_RATE}_u${j}_r${i}.cfg


cat \${PBS_NODEFILE}       # Output Contents of the PBS NODEFILE

env | grep PBS            # Print out values of the current jobs PBS environment variables

qstat -f \${PBS_JOBID}     # Print out final statistics about resource uses before job exits
EOT2

    qsub workdir_cs/sub_files/cluster_species_${FLUC_TYPE}_${INFLOW_RATE}_u${j}_r${i}.sub
  done
done
