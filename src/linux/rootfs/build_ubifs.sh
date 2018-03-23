#!/bin/sh
#+--------------------------------------------------------------------------------------------
#|Description: This shell script is used to generate a UBIFS rootfs for K9F2G08 nandflash
#|     Author:  GuoWenxue <guowenxue@gmail.com> QQ: 281143292
#|  ChangeLog:
#|           1, Initialize 1.0.0 on 2012.04.18
#|  Reference: 
#|       http://www.linux-mtd.infradead.org/faq/ubifs.html
#|       http://blog.sina.com.cn/s/blog_5b9ea9840100apqc.html
#+--------------------------------------------------------------------------------------------

VERSION=1.0.0
ROOTFS_DIR=rootfs_tree
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

#UBI: attaching mtd2 to ubi2
#UBI: physical eraseblock size:   131072 bytes (128 KiB)
#UBI: logical eraseblock size:    129024 bytes
#UBI: smallest flash I/O unit:    2048
#UBI: sub-page size:              512
#UBI: VID header offset:          512 (aligned 512)
#UBI: data offset:                2048
#UBI: max. sequence number:       2
#UBI: attached mtd2 to ubi2
#UBI: MTD device name:            "mtdblock2 rootfs 64MB"
#UBI: MTD device size:            64 MiB
#UBI: number of good PEBs:        510
#UBI: number of bad PEBs:         2
#UBI: number of corrupted PEBs:   0
#UBI: max. allowed volumes:       128
#UBI: wear-leveling threshold:    4096
#UBI: number of internal volumes: 1
#UBI: number of user volumes:     0
#UBI: available PEBs:             501
#UBI: total number of reserved PEBs: 9
#UBI: number of PEBs reserved for bad PEB handling: 5
#UBI: max/mean erase counter: 1/1
#UBI: image sequence number:  558504080
#UBI: background thread "ubi_bgt2d" started, PID 1200
#ubiattach -m 2 -d 2 /dev/ubi_ctrl
#mount -t ubifs -o rw,sync ubi2:rootfs /rootfs
#UBIFS: default file-system created
#UBIFS: mounted UBI device 2, volume 0, name "rootfs"
#UBIFS: file system size:   63479808 bytes (61992 KiB, 60 MiB, 492 LEBs)
#UBIFS: journal size:       3225600 bytes (3150 KiB, 3 MiB, 25 LEBs)
#UBIFS: media format:       w4/r0 (latest is w4/r0)
#UBIFS: default compressor: lzo
#UBIFS: reserved for root:  2998307 bytes (2928 KiB)


#===========================================
#  u-boot setup bootargs env               +
#===========================================
# set bubifs 'tftp 30800000 rootfs-ubifs.bin;nand erase 1000000 4000000;nand write 30800000 1000000 $filesize'
# set bootargs 'console=ttyS0,115200 mem=64M ubi.mtd=2 root=ubi0:rootfs rootwait rootfstype=ubifs rw'

#===========================================
#  Shell script body start here            +
#===========================================

IMAGE_NAME=rootfs-ubifs
config_file=rootfs_ubinize.cfg

#UBI: physical eraseblock size:   131072 bytes (128 KiB)
#UBI: logical eraseblock size:    129024 bytes
PEB_SIZE=131072  #128KiB
LEB_SIZE=129024


#One K9F2G08 device get 2048 block 
blocks_per_device=2048
partition_size_MB=40

#UBIFS: file system size:   63479808 bytes (61992 KiB, 60 MiB, 492 LEBs)
tmp=$(echo "$blocks_per_device * 0.025" | bc)
wear_level_reserved_blocks=`echo $((${tmp//.*/+1}))`

partition_size_bytes=`expr $partition_size_MB \* 1024 \* 1024` 
partition_physical_blocks=`expr $partition_size_bytes / $PEB_SIZE` 
LEB_COUNT=`expr $partition_physical_blocks - $wear_level_reserved_blocks`

VOL_SIZE=`expr $LEB_COUNT \* $LEB_SIZE`

# ubi2: min./max. I/O unit sizes: 2048/2048, sub-page size 2048

#UBI: smallest flash I/O unit:    2048
#UBI: sub-page size:              512
#UBI: VID header offset:          512 (aligned 512)
MIN_IO_SIZE=2048
SUB_PAGE_SIZE=512
VID_HDR_OFFSET=512


echo "Decompress rootfs packet and update rootfs version..."
SVNVER=`svn up rootfs.tar.bz2 | grep "At revision" | awk '{print $3}' | cut -d. -f1`
echo "version=V${VERSION} r$SVNVER `date +"%Y-%m-%d"`" > fs.conf

if [ ! -d $ROOTFS_DIR ] ; then
   sudo tar -xjf $ROOTFS_DIR.tar.bz2
fi
sudo mv fs.conf $ROOTFS_DIR/etc/fs.conf 
sudo chown root.root $ROOTFS_DIR/etc/fs.conf

echo ""
echo "Generating $IMAGE_NAME.bin file by mkfs.ubifs..."
set -x
sudo mkfs.ubifs -x lzo -m $MIN_IO_SIZE -e $LEB_SIZE -c $LEB_COUNT -r $ROOTFS_DIR -o $IMAGE_NAME.img
set +x

echo
echo "Generating configuration file..."
echo "[ubifs-volume]" > $config_file
echo "mode=ubi" >> $config_file
echo "image=$IMAGE_NAME.img" >> $config_file
echo "vol_id=0" >> $config_file
echo "vol_size=$VOL_SIZE" >> $config_file
echo "vol_type=dynamic" >> $config_file
echo "vol_name=rootfs" >> $config_file
echo "vol_flags=autoresize" >> $config_file
echo "vol_alignment=1" >> $config_file
echo

set -x
sudo ubinize -o $IMAGE_NAME.bin -m $MIN_IO_SIZE -p $PEB_SIZE -s $SUB_PAGE_SIZE -O $VID_HDR_OFFSET $config_file
sudo chmod a+x $IMAGE_NAME.bin
cp $IMAGE_NAME.bin $INSTALL_PATH
set +x
sudo rm -f $IMAGE_NAME.img $config_file

