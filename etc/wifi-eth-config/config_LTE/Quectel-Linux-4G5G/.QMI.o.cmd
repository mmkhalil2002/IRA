cmd_/home/ubuntu/wifi-eth-config-08-21-21/Quectel-Linux-4G5G/QMI.o := gcc -Wp,-MD,/home/ubuntu/wifi-eth-config-08-21-21/Quectel-Linux-4G5G/.QMI.o.d  -nostdinc -isystem /usr/lib/gcc/arm-linux-gnueabihf/9/include  -I./arch/armv7l/include -I./arch/armv7l/include/generated  -I./include -I./arch/armv7l/include/uapi -I./arch/armv7l/include/generated/uapi -I./include/uapi -I./include/generated/uapi -include ./include/linux/kconfig.h -Iubuntu/include  -include ./include/linux/compiler_types.h -D__KERNEL__ -mlittle-endian -Wall -Wundef -Werror=strict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -fshort-wchar -fno-PIE -Werror=implicit-function-declaration -Werror=implicit-int -Werror=return-type -Wno-format-security -std=gnu89 -fno-dwarf2-cfi-asm -fno-omit-frame-pointer -mapcs -mno-sched-prolog -fno-ipa-sra -mabi=aapcs-linux -mfpu=vfp -marm -Wa,-mno-warn-deprecated -D__LINUX_ARM_ARCH__=7 -march=armv7-a -msoft-float -Uarm -fno-delete-null-pointer-checks -Wno-frame-address -Wno-format-truncation -Wno-format-overflow -Wno-address-of-packed-member -O2 --param=allow-store-data-races=0 -Wframe-larger-than=1024 -fstack-protector-strong -Wimplicit-fallthrough -Wno-unused-but-set-variable -Wno-unused-const-variable -fno-omit-frame-pointer -fno-optimize-sibling-calls -fno-var-tracking-assignments -pg -Wdeclaration-after-statement -Wvla -Wno-pointer-sign -Wno-stringop-truncation -Wno-array-bounds -Wno-stringop-overflow -Wno-restrict -Wno-maybe-uninitialized -fno-strict-overflow -fno-merge-all-constants -fmerge-constants -fno-stack-check -fconserve-stack -Werror=date-time -Werror=incompatible-pointer-types -Werror=designated-init -fmacro-prefix-map=./= -Wno-packed-not-aligned  -DMODULE  -DKBUILD_BASENAME='"QMI"' -DKBUILD_MODNAME='"GobiNet"' -c -o /home/ubuntu/wifi-eth-config-08-21-21/Quectel-Linux-4G5G/QMI.o /home/ubuntu/wifi-eth-config-08-21-21/Quectel-Linux-4G5G/QMI.c

source_/home/ubuntu/wifi-eth-config-08-21-21/Quectel-Linux-4G5G/QMI.o := /home/ubuntu/wifi-eth-config-08-21-21/Quectel-Linux-4G5G/QMI.c

deps_/home/ubuntu/wifi-eth-config-08-21-21/Quectel-Linux-4G5G/QMI.o := \
  include/linux/kconfig.h \
    $(wildcard include/config/cpu/big/endian.h) \
    $(wildcard include/config/booger.h) \
    $(wildcard include/config/foo.h) \
  include/linux/compiler_types.h \
    $(wildcard include/config/have/arch/compiler/h.h) \
    $(wildcard include/config/enable/must/check.h) \
    $(wildcard include/config/optimize/inlining.h) \
    $(wildcard include/config/cc/has/asm/inline.h) \
  include/linux/compiler_attributes.h \
  include/linux/compiler-gcc.h \
    $(wildcard include/config/arm64.h) \
    $(wildcard include/config/retpoline.h) \
    $(wildcard include/config/arch/use/builtin/bswap.h) \

/home/ubuntu/wifi-eth-config-08-21-21/Quectel-Linux-4G5G/QMI.o: $(deps_/home/ubuntu/wifi-eth-config-08-21-21/Quectel-Linux-4G5G/QMI.o)

$(deps_/home/ubuntu/wifi-eth-config-08-21-21/Quectel-Linux-4G5G/QMI.o):
