#!/bin/bash
rm -f arch/arm/boot/dts/*.dtb
rm -f arch/arm/boot/dt.img
rm -f cwm_flash_zip/boot.img
make -j10 zImage
make -j10 dtimage
make -j10 modules
rm -rf squid_install
mkdir -p squid_install
cp arch/arm/boot/zImage cwm_flash_zip/tools/
cp arch/arm/boot/dt.img cwm_flash_zip/tools/
rm -f arch/arm/boot/squid_kernel.zip
cd cwm_flash_zip
zip -r ../arch/arm/boot/squid_kernel.zip ./
