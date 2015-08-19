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

KERNEL_IMAGE := arch/arm/boot/zImage
DEVTREE := arch/arm/boot/dt.img
BOARD_KERNEL_PAGESIZE := 2048

$(KERNEL_IMAGE): zImage

.PHONY: dtimage
dtimage: $(DEVTREE)

$(DEVTREE): dtbs $(DTBTOOL)
	$(call pretty,"Target dt image: $(DEVTREE)")
	$(DTBTOOL) -o $@ -s $(BOARD_KERNEL_PAGESIZE) -p scripts/dtc/ arch/arm/boot/dts/
	chmod a+r $@
