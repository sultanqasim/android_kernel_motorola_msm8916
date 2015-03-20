# Generate device tree image (dt.img)

BOARD_KERNEL_PAGESIZE := 2048
DTIMAGE_TARGET := $(PRODUCT_OUT)/dt.img

$(DTIMAGE_TARGET): zImage $(DTBTOOL)
    $(call pretty,"Target dt image: $(DTIMAGE_TARGET)")
    $(hide) $(DTBTOOL) -o $@ -s $(BOARD_KERNEL_PAGESIZE) -p scripts/dtc/ arch/arm/boot/dts/
    $(hide) chmod a+r $@
