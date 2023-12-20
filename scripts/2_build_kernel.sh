#!/bin/bash

set -exuo pipefail

my_dir="$(cd $(dirname $0) && pwd)"
function copy_source_code {
    mkdir -p $linux_dir/phonebook
    mkdir -p $linux_dir/keyboardPressCounter
    cp $source_dir1/phoneBook.* $linux_dir/phonebook
    cp $source_dir2/keyboardPressCounter.c $linux_dir/keyboardPressCounter
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
source_dir1=$work_dir/../task1
source_dir2=$work_dir/../task2

mkdir -p $work_dir $output_dir

copy_source_code
build_linux
copy_resultant_kernel
