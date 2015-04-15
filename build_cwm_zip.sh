#!/bin/bash
rm -f arch/arm/boot/dts/*.dtb
rm -f arch/arm/boot/dt.img
make -j10 bootimage
make -j10 modules
rm -rf squid_install
mkdir -p squid_install
make -j10 modules_install INSTALL_MOD_PATH=squid_install INSTALL_MOD_STRIP=1
mkdir -p cwm_flash_zip/system/lib/modules/pronto
find squid_install/ -name '*.ko' -type f -exec cp '{}' cwm_flash_zip/system/lib/modules/ \;
mv cwm_flash_zip/system/lib/modules/wlan.ko cwm_flash_zip/system/lib/modules/pronto/pronto_wlan.ko
cp arch/arm/boot/boot.img cwm_flash_zip/
rm -f arch/arm/boot/squid_kernel.zip
cd cwm_flash_zip
zip -r ../arch/arm/boot/squid_kernel.zip ./
