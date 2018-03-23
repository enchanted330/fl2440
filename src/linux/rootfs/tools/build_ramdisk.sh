#!/bin/sh
#+--------------------------------------------------------------------------------------------
#|Description: This shell script is used to generate a ramdisk rootfs for K9F2G08 nandflash
#|     Author:  GuoWenxue <guowenxue@gmail.com> QQ: 281143292
#|  ChangeLog:
#|           1, Initialize 1.0.0 on 2012.04.18
#|  Reference: 
#|       http://www.linux-mtd.infradead.org/faq/ubifs.html
#|       http://blog.sina.com.cn/s/blog_5b9ea9840100apqc.html
#+--------------------------------------------------------------------------------------------

#===================================================================
#  U-BOOT print the Rootfs partition UBI information for reference +
#===================================================================
#U-Boot> mtdparts default
#[ s3c2440@guowenxue ]# mtdparts
#
#device nand0 <nand0>, # parts = 8
##: name                size            offset          mask_flags
#0: uboot               0x00100000      0x00000000      0
#1: kernel              0x00f00000      0x00100000      0
#2: ramdisk             0x01400000      0x01000000      0
#3: cramfs              0x01400000      0x02400000      0
#4: jffs2               0x01400000      0x03800000      0
#5: yaffs2              0x01400000      0x04c00000      0
#6: ubifs               0x01400000      0x06000000      0
#7: users               0x08c00000      0x07400000      0
#
#active partition: nand0,0 - (uboot) 0x00100000 @ 0x00000000
#
#defaults:
#mtdids  : nand0=nand0
#mtdparts: mtdparts=nand0:1m(uboot),15m(kernel),20m(ramdisk),20m(cramfs),20m(jffs2),20m(yaffs2),20m(ubifs),-(users)
#

#===================================================================
#  Linux kenrel print nandflash partition information for reference +
#===================================================================
#Creating 9 MTD partitions on "NAND":
#0x000000000000-0x000000100000 : "mtdblock0 u-boot 1MB"
#0x000000100000-0x000001000000 : "mtdblock1 kernel 15MB"
#0x000001000000-0x000002400000 : "mtdblock2 ramdisk 20MB"
#0x000002400000-0x000003800000 : "mtdblock3 cramfs 20MB"
#0x000003800000-0x000006000000 : "mtdblock4 jffs2 40MB"
#0x000006000000-0x000008800000 : "mtdblock5 yaffs2 40MB"
#0x000008800000-0x00000b000000 : "mtdblock6 ubifs 40MB"
#0x00000b000000-0x00000d800000 : "mtdblock7 apps 40MB"
#0x00000d800000-0x000010000000 : "mtdblock8 data 40MB"


#===========================================
#  Shell script body start here            +
#===========================================

CPU=s3c2440
rootfs_dir=../rootfs_tree
image_name=ramdisk-${CPU}.rootfs
ramdisk_size=16 #Unit MB
image_tmp=ramdisk
MNT=mnt
CMD_PREFIX=

if [ ! -d $rootfs_dir ] ; then 
    echo "Miss rootfs source code tree \"$rootfs_dir\" exit..."
    exit;
fi

if [ root != `whoami` ] ; then 
    echo "WARNING: Not root user, use sudo to excute this shell script commands" 
    CMD_PREFIX=sudo
fi

if [ ! -d $MNT ] ; then
    mkdir $MNT
fi


set -x
dd if=/dev/zero of=$image_tmp bs=1M count=$ramdisk_size
$CMD_PREFIX mke2fs -F -v -m0 $image_tmp
$CMD_PREFIX mount -o loop $image_tmp $MNT
$CMD_PREFIX rm -rf $MNT/lost+found/

sudo cp -af $rootfs_dir/* $MNT

$CMD_PREFIX umount $image_tmp
gzip $image_tmp
mv $image_tmp.gz $image_name
rm -rf $MNT

