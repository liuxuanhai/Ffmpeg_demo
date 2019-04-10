#!/bin/bash
make clean
export NDK=/test2/ndk/android-ndk-r14b
export SYSROOT=$NDK/platforms/android-9/arch-arm/
export TOOLCHAIN=$NDK/toolchains/arm-linux-androideabi-4.9/prebuilt/linux-x86_64
export CPU=armeabi-v7a
export PREFIX=$(pwd)/android/$CPU
export ADDI_CFLAGS="-march=armv7-a -mfloat-abi=softfp -mfpu=neon"
export ADDI_LDFLAGS="-Wl,--fix-cortex-a8"

./configure --target-os=android \
--prefix=$PREFIX --arch=arm \
--enable-shared \
--disable-static \
--disable-yasm \
--disable-symver \
--enable-gpl \
--enable-ffmpeg \
--disable-ffplay \
--disable-ffprobe \
--disable-doc \
--disable-symver \
--cross-prefix=$TOOLCHAIN/bin/arm-linux-androideabi- \
--enable-cross-compile \
--sysroot=$SYSROOT \
--extra-cflags="-Os -fpic $ADDI_CFLAGS" \
--extra-ldflags="$ADDI_LDFLAGS" \
$ADDITIONAL_CONFIGURE_FLAG
make clean
make
make install

