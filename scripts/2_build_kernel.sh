#!/bin/bash

set -exuo pipefail

my_dir="$(cd $(dirname $0) && pwd)"
function copy_source_code {
    cp $source_dir/phoneBook.c $source_dir/phoneBook.h $linux_dir/phonebook
}
function build_linux {
    (cd $linux_dir; 
        make defconfig; 
        make -j $(nproc))
}
function copy_resultant_kernel {
    cp $kernel_path $output_dir
}


work_dir=$my_dir
misc_dir=$my_dir/misc
output_dir=$my_dir/output
linux_dir=$work_dir/../linux-6.5.2-custom
kernel_path=$linux_dir/arch/x86/boot/bzImage
source_dir=$work_dir/../task1

mkdir -p $work_dir $output_dir

copy_source_code
build_linux
copy_resultant_kernel
