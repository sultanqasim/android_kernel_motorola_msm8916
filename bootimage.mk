# Commands to build boot.img
# They assume that the device tree image and initrd are in their places

MKBOOTIMG := mkbootimg/mkbootimg
$(MKBOOTIMG):
	make -C mkbootimg

BOOT_IMAGE_OUT := arch/arm/boot/boot.img
KERNEL_IMAGE := arch/arm/boot/zImage
RAMDISK := boot/ramdisk/initrd.gz
DEVTREE := boot/dt.img
KERNEL_BASE := 0x80000000
KERNEL_CMDL := 'console=ttyHSL0,115200,n8 androidboot.console=ttyHSL0 androidboot.hardware=qcom msm_rtb.filter=0x3F ehci-hcd.park=3 vmalloc=400M androidboot.bootdevice=7824900.sdhci utags.blkdev=/dev/block/bootdevice/by-name/utags utags.backup=/dev/block/bootdevice/by-name/utagsBackup movablecore=160M'

$(KERNEL_IMAGE): zImage

MKBOOTIMG_ARGS := --kernel ${KERNEL_IMAGE} --ramdisk ${RAMDISK} --dt ${DEVTREE} \
    --base ${KERNEL_BASE} --cmdline ${KERNEL_CMDL} 

.PHONY: bootimage
bootimage: $(BOOT_IMAGE_OUT)

$(BOOT_IMAGE_OUT): $(KERNEL_IMAGE) $(RAMDISK) $(DEVTREE) $(MKBOOTIMG)
	$(MKBOOTIMG) $(MKBOOTIMG_ARGS) -o $@
