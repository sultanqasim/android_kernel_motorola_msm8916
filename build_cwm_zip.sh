# Run this after doing make modules_install INSTALL_MOD_PATH=squid_install
#!bash
mkdir -p cwm_flash_zip/system/lib/modules/pronto
find squid_install/ -name '*.ko' -type f -exec cp '{}' cwm_flash_zip/system/lib/modules/ \;
mv cwm_flash_zip/system/lib/modules/wlan.ko cwm_flash_zip/system/lib/modules/pronto/pronto_wlan.ko
cp arch/arm/boot/boot.img cwm_flash_zip/
rm -f arch/arm/boot/squid_kernel.zip
cd cwm_flash_zip
zip -r ../arch/arm/boot/squid_kernel.zip ./
