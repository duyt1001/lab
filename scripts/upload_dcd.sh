#!/usr/bin/env bash

LOCAL_RESULTS_DIR=/Volumes/Anna/2023/charmm/results
S3_DCD_DIR=s3://annadu/charmm/dcd

# look for dcd files "all-*.dcd" in the local results directory
cd "$LOCAL_RESULTS_DIR"
for dcd in */charmm_openmm/all-*.dcd; do
	newdcd=${dcd//\//__}
	echo "$dcd -> $S3_DCD_DIR/$newdcd"
	aws --profile anna s3 cp "$dcd" "$S3_DCD_DIR/$newdcd"
done
