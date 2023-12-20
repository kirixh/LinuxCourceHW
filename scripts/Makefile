kernel=output/bzImage
initramfs=output/initramfs.igz

.DEFAULT_GOAL=all


$(initramfs):
	bash ./0_build_initramfs.sh
$(kernel):
	bash ./2_build_kernel.sh
run_qemu: all
	bash ./3_run_qemu.sh
do_clean:
	bash ./4_clean.sh

all: $(initramfs) $(kernel)
run: run_qemu
clean: do_clean
