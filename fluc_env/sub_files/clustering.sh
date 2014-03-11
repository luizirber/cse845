#!/bin/bash -exu

CUTOFF=151467

HISTORY_FILE=$1
DETAIL_FILE=$2
SIMULATION=$3
REPLICATE=$4
GENERATION=$5
OUTPUT_DIR=$6
OUTPUT_HISTORY_PRE=$7
OUTPUT_DETAIL_PRE=$8
OUTPUT_FILE=$9

ANALYSIS_SCRIPTS=${10}
WORKDIR=${11}
JOB_LOG=${12}

mkdir -p $OUTPUT_DIR
mkdir -p $WORKDIR/{outputs,sub}

cat <<EOT1> $WORKDIR/sub/$SIMULATION.$REPLICATE.$GENERATION
#!/bin/bash -login
#PBS -l walltime=00:10:00,nodes=1:ppn=1
#PBS -l mem=40mb
#PBS -j oe
#PBS -o $JOB_LOG
#PBS -N $SIMULATION.$REPLICATE.$GENERATION
#PBS -A ged-intel11
#PBS -W umask=027

set -exu

python $ANALYSIS_SCRIPTS/preprocess.py $HISTORY_FILE $OUTPUT_HISTORY_PRE
python $ANALYSIS_SCRIPTS/preprocess.py $DETAIL_FILE $OUTPUT_DETAIL_PRE
$ANALYSIS_SCRIPTS/treeCS $OUTPUT_DETAIL_PRE $OUTPUT_HISTORY_PRE $OUTPUT_FILE $CUTOFF
rm $OUTPUT_DETAIL_PRE $OUTPUT_HISTORY_PRE

cat \${PBS_NODEFILE}       # Output Contents of the PBS NODEFILE

env | grep PBS            # Print out values of the current jobs PBS environment variables

qstat -f \${PBS_JOBID}     # Print out final statistics about resource uses before job exits
set +exu

echo "Job ended without problems"
EOT1

qsub $WORKDIR/sub/$SIMULATION.$REPLICATE.$GENERATION
