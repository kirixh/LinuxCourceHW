.DEFAULT_GOAL=all


build_initramfs:
	bash ./1_build_initramfs.sh
build_kernel:
	bash ./2_build_kernel.sh
run_qemu:
	bash ./3_run_qemu.sh
do_clean:
	bash ./4_clean.sh

all: build_initramfs build_kernel
run: run_qemu
clean: do_clean
