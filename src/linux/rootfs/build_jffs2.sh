#!/bin/sh
#+--------------------------------------------------------------------------------------------
#|Description: This shell script is used to generate a JFFS2 rootfs for K9F2G08 nandflash
#|     Author:  GuoWenxue <guowenxue@gmail.com> QQ: 281143292
#|  ChangeLog:
#|           1, Initialize 1.0.0 on 2012.04.18
#|  Reference: 
#|       http://www.linux-mtd.infradead.org/faq/ubifs.html
#|       http://blog.sina.com.cn/s/blog_5b9ea9840100apqc.html
#+--------------------------------------------------------------------------------------------

VERSION=1.0.0

ROOTFS_DIR=./rootfs_tree
INSTALL_PATH=../../../bin/

#===================================================================
#  Linux kenrel mount rootfs partition information for reference +
#===================================================================
#  ~ >: cat /proc/mtd 
#  dev:    size   erasesize  name
# mtd0: 00100000 00020000 "mtdblock0 u-boot 1MB"
# mtd1: 00f00000 00020000 "mtdblock1 kernel 15MB"
# mtd2: 04000000 00020000 "mtdblock2 rootfs 64MB"
# mtd3: 05000000 00020000 "mtdblock3 apps 80MB"
# mtd4: 03000000 00020000 "mtdblock4 data 48MB"
# mtd5: 03000000 00020000 "mtdblock5 info 48MB"

#===========================================
#  u-boot setup bootargs env               +
#===========================================
# set bjffs2 'tftp 30800000 rootfs-jffs2.bin;nand erase 1000000 4000000;nand write 30800000 1000000 $filesize'
# set bootargs 'console=ttyS0,115200 root=/dev/mtdblock2 rootfstype=jffs2 init=/linuxrc mem=64M rw noinitrd loglevel=7' 

#===========================================
#  Shell script body start here            +
#===========================================

IMAGE_NAME=rootfs-jffs2.bin
partition_sizeM=`expr 20 \- 1` #Left 1M for bad block

page_size=2048
pages_per_block=64
block_size_in_bytes=`expr $page_size \* $pages_per_block`

partition_size=`expr $partition_sizeM \* 1024 \* 1024`

echo "Update rootfs version..."
SVNVER=`svn up rootfs.tar.bz2 | grep "At revision" | awk '{print $3}' | cut -d. -f1`
echo "version=V${VERSION} r$SVNVER `date +"%Y-%m-%d"`" > fs.conf

if [ ! -d $ROOTFS_DIR ] ; then
   sudo tar -xjf $ROOTFS_DIR.tar.bz2 
fi 

sudo mv fs.conf $ROOTFS_DIR/etc/fs.conf 
sudo chown root.root $ROOTFS_DIR/etc/fs.conf 

echo ""
echo "Generating $IMAGE_NAME file by mkfs.jffs2..."

set -x
sudo mkfs.jffs2 -n -s $page_size -e $block_size_in_bytes -d $ROOTFS_DIR -o $IMAGE_NAME --pad=$partition_size
set +x
sudo chmod a+x $IMAGE_NAME

cp $IMAGE_NAME $INSTALL_PATH

