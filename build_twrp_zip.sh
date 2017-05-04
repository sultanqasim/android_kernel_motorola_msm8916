#!/bin/bash
set -e
rm -f arch/arm/boot/dts/*.dtb
rm -f arch/arm/boot/dt.img
rm -f twrp_flash_zip/boot.img
make -j10 zImage
make -j10 dtimage
make -j10 modules
rm -rf moon_install
mkdir -p moon_install
make -j10 modules_install INSTALL_MOD_PATH=moon_install INSTALL_MOD_STRIP=1
mkdir -p twrp_flash_zip/system/lib/modules/pronto
find moon_install/ -name '*.ko' -type f -exec cp '{}' twrp_flash_zip/system/lib/modules/ \;
mv twrp_flash_zip/system/lib/modules/wlan.ko twrp_flash_zip/system/lib/modules/pronto/pronto_wlan.ko
cp arch/arm/boot/zImage twrp_flash_zip/tools/
cp arch/arm/boot/dt.img twrp_flash_zip/tools/
rm -f arch/arm/boot/moon_kernel.zip
cd twrp_flash_zip
zip -r ../arch/arm/boot/moon_kernel.zip ./
