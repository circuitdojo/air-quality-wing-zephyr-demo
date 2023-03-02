#!/bin/bash

# Different apps
declare -a basic_targets=("circuitdojo_feather_nrf9160_ns" "particle_xenon" "nrf52840dk_nrf52840")
declare -a basic_with_display_targets=("circuitdojo_feather_nrf9160_ns" "particle_xenon")
declare -a ble_targets=("particle_xenon" "nrf52840dk_nrf52840")
declare -a golioth_targets=("circuitdojo_feather_nrf9160_ns")

# Applications
declare -a apps=("basic" "ble" "basic_with_display")

# Get Git tags
git fetch --prune --tags
version=$(git describe --tags --long)

# Make output dir
mkdir -p _out

# Stop on error
set -e

# For each target
for app in "${apps[@]}"
do

# Map the targets to the sample
if [ $app == "basic_with_display" ]
then
targets=("${basic_with_display_targets[@]}")
elif [ $app == "basic" ]
then
targets=("${basic_targets[@]}")
elif [ $app == "ble" ]
then 
targets=("${ble_targets[@]}")
elif [ $app == "golioth" ]
then
targets=("${golioth_targets[@]}")
fi

# For each target
for target in "${targets[@]}"
do

# Echo version
echo "Building ${app} (ver: ${version}) for ${target}".

# Change directory
cd ${app}

# Build the target
west build -b $target -p

# Go back
cd ..

# Copy the target files over
mkdir -p _out/${version}/${app}
cp ${app}/build/zephyr/app_update.bin _out/${version}/${app}/${app}_${target}_${version}_update.bin || true
cp ${app}/build/zephyr/merged.hex _out/${version}/${app}/${app}_${target}_${version}_merged.hex || true
cp ${app}/build/zephyr/zephyr.hex _out/${version}/${app}/${app}_${target}_${version}_zephyr.hex || true


done
done