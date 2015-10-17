#!/bin/bash

#Define Paths
dir=${HOME}/android/kernel/BOA-butterfly3/
dest=${HOME}/Desktop/modules/

#Kernel build START
VER="BOA_Kernel_1"

DATE_START=$(date +"%s")
export LOCALVERSION="-"`echo $VER`
export ARCH=arm64
export SUBARCH=arm64
export PATH=$PATH:~/android/kernel/toolchains/aarch64-linux-android-4.9/bin/
export CROSS_COMPILE=aarch64-linux-android-
export KBUILD_BUILD_USER=BOA

make "b3uhl_defconfig"

make -j8

DATE_END=$(date +"%s")
DIFF=$(($DATE_END - $DATE_START))

echo
if (( $(($DIFF / 60)) == 0 )); then
	echo "  Build completed in $(($DIFF % 60)) seconds."
elif (( $(($DIFF / 60)) == 1 )); then
	echo "  Build completed in $(($DIFF / 60)) minute and $(($DIFF % 60)) seconds."
else
	echo "  Build completed in $(($DIFF / 60)) minutes and $(($DIFF % 60)) seconds."
fi
echo "  Finish time: $(date +"%r")"
echo
#Kernel build END


#Generate Device Tree Blobs
./dtbToolCM -o ~/android/kernel/dt_files/dt.img -s 4096 -2 -p ./scripts/dtc/ ./arch/arm/boot/dts/

#Find modules and extract them to Desktop/modules
mkdir ${HOME}/Desktop/modules/
find "$dir" -name "*.ko" -exec cp -v {} $dest \; -printf "Found .ko Module\n"

#Remove any .rej files leftover from previous
find "$dir" -type f -name "*.rej" -exec rm -f {} \; -printf "Deleted .rej file\n"
