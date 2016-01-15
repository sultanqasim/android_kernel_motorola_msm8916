#!/bin/bash
set -e
rm -f arch/arm/boot/dts/*.dtb
rm -f arch/arm/boot/dt.img
rm -f cwm_flash_zip/boot.img
make -j10 zImage
make -j10 dtimage
make -j10 modules
rm -rf squid_install
mkdir -p squid_install
make -j10 modules_install INSTALL_MOD_PATH=squid_install INSTALL_MOD_STRIP=1
mkdir -p cwm_flash_zip/system/lib/modules/pronto
find squid_install/ -name '*.ko' -type f -exec cp '{}' cwm_flash_zip/system/lib/modules/ \;
mv cwm_flash_zip/system/lib/modules/wlan.ko cwm_flash_zip/system/lib/modules/pronto/pronto_wlan.ko
cp arch/arm/boot/zImage cwm_flash_zip/tools/
cp arch/arm/boot/dt.img cwm_flash_zip/tools/
rm -f arch/arm/boot/squid_kernel.zip
cd cwm_flash_zip
zip -r ../arch/arm/boot/squid_kernel.zip ./
