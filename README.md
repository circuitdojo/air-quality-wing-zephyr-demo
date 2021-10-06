# Air Quality Wing Examples

This repository is contains examples of using the Air Quality Wing with Zephyr.

## Documentation

The documentation for this board and these samples is [located here.](https://docs.jaredwolff.com/air-quality-wing/zephyr-examples.html)

## Setup

Here is the minimal amount of steps to get an example working. This example is currently only configured for the nRF52 DK. 

### Hardware

If you do not have a PM2.5 sensor you can comment out `&hpma_sensor,` within `sensors[]` in `main.c` before building. 

### Virtual Environment

Set up a python virual environment:

```
virtualenv -p python3 env
```

Then enable it by running:

```
source env/bin/activate
```

### Install `west`

```
pip3 install west
```

### Init and update project

```
west init -m https://github.com/circuitdojo/air-quality-wing-zephyr-demo.git  --manifest-rev main
```

Or already cloned locally:

```
west init -l .
```

Then run

```
west update
```

### Install remaining python dependencies

```
pip3 install -r zephyr/scripts/requirements.txt
```

### Install toolchain

   * For **Mac** run the following: (it does require you install `wget`. `brew` is an easy way to do so: `brew install wget`)

     ```
     cd ~
     wget "https://developer.arm.com/-/media/Files/downloads/gnu-rm/9-2019q4/gcc-arm-none-eabi-9-2019-q4-major-mac.tar.bz2"
     tar xvfj gcc-arm-none-eabi-9-2019-q4-major-mac.tar.bz2
     rm gcc-arm-none-eabi-9-2019-q4-major-mac.tar.bz2
     ```

     **Note** for Catalina users you will get an error when running these utilities for the first time. You must allow them to be executed in your Security preferences.

     ![Error running ARM Toolchain](img/sdk-setup-mac/cannot-be-opened.jpeg)


   * For **Windows** you can download and install the toolchain with [this direct link.](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads/9-2019-q4-major)

### Golioth

You will need to edit `golioth.conf` with your credentials in order to connect to Golioth's backend. More instructions on setting up your credentials can be [found here.](https://docs.golioth.io/docs/guides/golioth-platform-getting-started/platform-manage-devices) You can also create devices and add credentials within the [Golioth Console.](https://console.golioth.io)


## Building

### nRF52840 DK

```
cd demo
west build -b nrf52840dk_nrf52840
```

Then flash using

```
nrfjprog --program build/merged.hex --chiperase --reset
```

or load via the bootloader

```
newtmgr -c serial image upload build/zephyr/app_update.bin && newtmgr -c serial reset
```
### Xenon

```
cd basic
west build -b particle_xenon
```

Then flash using

```
nrfjprog --program build/merged.hex --chiperase --reset
```

### nRF9160 Feather

```
cd basic
west build -b circuitdojo_feather_nrf9160_ns
```

Then flash using

```
nrfjprog --program build/merged.hex --chiperase --reset
```