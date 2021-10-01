#!/bin/bash

# Different apps
declare -a basic_targets=("circuitdojo_feather_nrf9160ns" "particle_xenon" "nrf52840dk_nrf52840")
declare -a ble_targets=("particle_xenon" "nrf52840dk_nrf52840")
declare -a golioth_targets=("circuitdojo_feather_nrf9160ns")

# Applications
declare -a apps=("basic" "ble" "golioth")

# Get Git tags
git fetch --prune --tags
version=$(git describe --tags --long)

# Make output dir
mkdir -p _out

# For each target
for app in "${apps[@]}"
do

# Map the targets to the sample
if [ $app == "basic" ]
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

# Build the target
west build -b $target -s ${app} -p

# Copy the target files over
mkdir -p _out/${version}/${app}
cp build/zephyr/app_update.bin _out/${version}/${app}/${app}_${target}_${version}_update.bin
cp build/zephyr/merged.hex _out/${version}/${app}/${app}_${target}_${version}_merged.hex

done
done