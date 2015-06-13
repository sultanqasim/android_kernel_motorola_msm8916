# Commands to build boot.img
# They assume that the device tree image and initrd are in their places

MKBOOTIMG := tools/mkbootimg/mkbootimg
$(MKBOOTIMG):
	make -C tools/mkbootimg

DTBTOOL := tools/dtbtool/dtbtool
$(DTBTOOL):
	make -C tools/dtbtool

MKBOOTFS := tools/mkbootfs/mkbootfs
$(MKBOOTFS):
	make -j1 -C tools/mkbootfs

BOOT_IMAGE_OUT := arch/arm/boot/boot.img
KERNEL_IMAGE := arch/arm/boot/zImage
RAMDISK := arch/arm/boot/initramfs.cpio.gz
DEVTREE := arch/arm/boot/dt.img
KERNEL_BASE := 0x80000000
KERNEL_CMDL := 'console=ttyHSL0,115200,n8 androidboot.console=ttyHSL0 androidboot.hardware=qcom msm_rtb.filter=0x3F ehci-hcd.park=3 vmalloc=400M androidboot.bootdevice=7824900.sdhci utags.blkdev=/dev/block/bootdevice/by-name/utags utags.backup=/dev/block/bootdevice/by-name/utagsBackup movablecore=160M'
BOARD_KERNEL_PAGESIZE := 2048

$(KERNEL_IMAGE): zImage

.PHONY: ramdisk FORCE_RDISK
ramdisk: $(RAMDISK)
FORCE_RDISK:

RAMDISK_ROOT = "boot/ramdisk_$(VARIANT)"

ifneq ($(shell test -d $(RAMDISK_ROOT); echo $$?),0)
$(error Variant $(VARIANT) not found)
endif

$(RAMDISK): $(MKBOOTFS) FORCE_RDISK
	$(MKBOOTFS) $(RAMDISK_ROOT) | gzip -9 -n >$@

.PHONY: dtimage
dtimage: $(DEVTREE)

$(DEVTREE): dtbs $(DTBTOOL)
	$(call pretty,"Target dt image: $(DEVTREE)")
	$(DTBTOOL) -o $@ -s $(BOARD_KERNEL_PAGESIZE) -p scripts/dtc/ arch/arm/boot/dts/
	chmod a+r $@

MKBOOTIMG_ARGS := --kernel ${KERNEL_IMAGE} --ramdisk ${RAMDISK} --dt ${DEVTREE} \
    --base ${KERNEL_BASE} --cmdline ${KERNEL_CMDL}

.PHONY: bootimage
bootimage: $(BOOT_IMAGE_OUT)

$(BOOT_IMAGE_OUT): $(KERNEL_IMAGE) $(RAMDISK) $(DEVTREE) $(MKBOOTIMG)
	$(MKBOOTIMG) $(MKBOOTIMG_ARGS) -o $@
