# .bashrc

# User specific aliases and functions

# Source global definitions
if [ -f /etc/bashrc ]; then
	. /etc/bashrc
fi

JAVA_HOME=/usr/jdk1.6.0_30/
PATH=$JAVA_HOME/bin:$PATH
CLASSPATH=$JAVA_HOME/lib/tools.jar:$JAVA_HOME/lib/dt.jar:$CLASSPATH

PATH=$PATH:/opt/buildroot-2011.11/arm920t/usr/bin
PS1='[\u@\h \W]\$ '
export LANG=en_US.UTF8
set -o vi
alias gitco='git checkout'
alias gitci='git commit'
alias vt100='export TERM=vt100'
alias linux='export TERM=linux'
alias maek='make'
#alias rmsvn='find -name .svn -exec rm -rf {} \;'
#alias rmgdb='find -iname "*.gdb" -exec rm -rf {} \;'
#alias rmobj='find -iname "*.o" -exec rm -rf {} \;'
#alias rmdep='find -name .depend -exec rm -rf {} \;'
alias myvalgrind='valgrind --track-fds=yes --leak-check=full --leak-resolution=high --track-origins=yes -v '
alias myindent='indent -npro -kr -i4 -ts4 -bls -bl -bli0 -cli2 -ss -bap -sc -sob -l100 -ncs -nce -nut'
alias tag='cscope -Rbq && ctags --c-kinds=+defglmnstuvx --langmap=c:.c.h.ho.hem.het.hec.hev.him.hit.hic.hiv -R .'
alias tagclean='rm -f cscope.* tags'
alias nocvim='mv ~/.vimrc ~/.vimrcb'
alias cvim='mv ~/.vimrcb ~/.vimrc'
alias ssh2board='ssh root@192.168.1.78'

# Description:  This function used to remove some specified files in current folder
#      Author:  Guo Wenxue(guowenxue@gmail.com)
#     Version:  1.0.0 (Release by guowenxue on 26th Fri. 2011)

function erm ()
{
   if [ -z $1 ]; then
      echo "This function used to remove some specified files in current folder"
      echo "Usage: $FUNCNAME [express]"
      echo "Example: $FUNCNAME *.gdb"
      return;
   fi

   express=$1
   find -iname "$express" -exec rm -rf {} \;
}
alias rmsvn='erm .svn'

# Description:  This function use samba filesystem to mount windows folder
#      Author:  Guo Wenxue(guowenxue@gmail.com)
#     Version:  1.0.0 (Release by guowenxue on 26th Fri. 2011)

function mntwinxp ()
{
#  if [ -z $1 ] ; then
#     echo "This function used to mount the windows XP folder to Linux, and it use sudo to excute"
#     echo "Usage: $FUNCNAME [password]"
#     return;
#  fi

  sudo mount -t vfat /dev/sda6  /mnt/

  #password="password"
  #if [ ! -z $1 ] ; then
  #   password=$1
  #fi

  #sudo mount -t cifs -o username="guowenxue",password="password" //192.168.8.74/linux /home/guowenxue/winxp/
}

# Description:  This function used to uncompress the packets according to the packet name
#      Author:  Guo Wenxue(guowenxue@gmail.com)
#     Version:  1.0.0 (Release by guowenxue on 26th Fri. 2011)

function ex () 
{
  if [ -f $1 ] ; then
    case $1 in
      *.tar.bz2)   tar xjf $1        ;;
      *.tar.gz)    tar xzf $1     ;;
      *.bz2)       bunzip2 $1       ;;
      *.rar)       rar x $1     ;;
      *.gz)        gunzip $1     ;;
      *.tar)       tar xf $1        ;;
      *.tbz2)      tar xjf $1      ;;
      *.tgz)       tar xzf $1       ;;
      *.zip)       unzip $1     ;;
      *.Z)         uncompress $1  ;;
      *.7z)        7z x $1    ;;
      *)           echo "'$1' cannot be extracted via extract()" ;;
    esac
  else
    echo "'$1' is not a valid file"
  fi
}

# Description:  This function used to format all the source code in current forlder
#               and convert source code file format from windows to linux
#      Author:  Guo Wenxue(guowenxue@gmail.com)
#     Version:  1.0.0 (Release by guowenxue on 18th Fri. 2010)

function format_src ()
{

  find -iname "*.c" -exec dos2unix {} \;
  find -iname "*.h" -exec dos2unix {} \;
  find -iname "makefile" -exec dos2unix {} \;
  find -iname "Makefile" -exec dos2unix {} \;

  # -npro   不要读取indent的配置文件.indent.pro
  # -kr     使用Kernighan&Ritchie的格式
  # -i4     设置缩排的格数为4
  # -di28   将声明区段的变量置于指定的栏位(28) 
  # -ts4    设置tab的长度为4 
  # -bls    定义结构，"struct"和"{"分行
  # -bl     if(或是else,for等等)与后面执行区段的”{“不同行，且”}”自成一行。
  # -bli0   设置{ }缩排的格数为0 
  # -cli2   使用case时，switch缩排的格数
  # -ss     若for或whiile区段只有一行时，在分号前加上空格
  # -bad    在声明区段后加上空白行
  # -bbb    块注释前加空行
  # -bap    函数结束后加空行
  # -sc     在每行注释左侧加上星号(*)。
  # -bc     在声明区段中，若出现逗号即换行。
  # -sob    删除多余的空白行
  # -l100   非注释行最长100
  # -ncs    不要在类型转换后面加空格
  # -nce    不要将else置于”}”之后
  # -nut    不要使用tab来缩进

  INDET_FORMAT="-npro -kr -i4 -ts4 -bls -bl -bli0 -cli2 -ss -bap -sc -sob -l100 -ncs -nce -nut"

  find -iname "*.c" -exec indent $INDET_FORMAT {} \;
  find -iname "*.h" -exec indent $INDET_FORMAT {} \;

  find -iname "*.h~" | xargs rm -rf {} \;
  find -iname "*.c~" | xargs rm -rf {} \;
}

# Description:  This function used to generate uImage for ARM linux.
#      Author:  GuoWenxue<guowenxue@gmail.com>
#     Version:  1.0.0 (Release by guowenxue on 04th Feb. 2010)
# 
function mkuimage ()
{
  ZIMAGE=arch/arm/boot/zImage
  if [ ! -f $ZIMAGE ] ; then
     echo "ERROR: $ZIMAGE doesn't exist"
     return;
  fi

  cp $ZIMAGE zImage --reply=yes
  mkimage -A arm -O linux -n kernel -C NONE -a 0x20008000 -e 0x20008000 -d zImage uImage.gz
  rm -f zImage
}


# Description:  This function used to grep the "key" in all the files in current folder.
#      Author:  GuoWenxue<guowenxue@gmail.com>
#     Version:  1.0.0 (Release by guowenxue on 04th Feb. 2010)
# 

function mygrep ( )
{
  if [ $# -lt 1 ] ; then
      echo "   Usage: $FUNCNAME key [-g] [suffix]"
      echo "Example1: mygrep haha"
      echo "Example2: mygrep haha -g"
      echo "Example3: mygrep haha *.c"
      echo "Example4: mygrep haha -g *.c"
      return
  fi

  key=$1         #The grep key

  if [ $# = 1 ] ; then
      grep -n "$key" -r *

  elif [ $# = 2 ] ; then
      if [ $2 = "-g" ] ; then
         grep -n "\<$key\>" -r *
      else
         suffix=$2
         find -iname "$suffix" | xargs grep -n "$key"
      fi
  else # Arguments more than 3

      if [ $2 = "-g" ] ; then
             suffix=$3

      elif [ $3 = "-g" ]; then
             suffix=$2
      fi

      find -iname "$suffix" | xargs grep -n "\<$key\>"
  fi
}


# Description:  This function used to generate a ramdisk block device file
#      Author:  GuoWenxue<guowenxue@gmail.com>
#     Version:  1.0.0 (Release by guowenxue on 24th Apr. 2011)
# 

function ramdisk_init ()
{
  RF_MNT=mnt

  if [ $# != 2 ] ; then
     echo "This function used to generate a ramdisk block device file"
     echo "Usage: $FUNCNAME [ramdisk_image_name] [size(MB)]"

     return;
  fi

  RF_NAME=$1
  SIZE=$2
  CMD_PREFIX=

  if [ root != `whoami` ] ; then 
     echo "WARNING: Not root user, use sudo to excute this function commands" 
     CMD_PREFIX=sudo
  fi

  dd if=/dev/zero of=$RF_NAME bs=1M count=$SIZE
  $CMD_PREFIX mke2fs -F -v -m0 $RF_NAME

  if [ ! -d $RF_MNT ] ; then
     mkdir $RF_MNT
  fi

  $CMD_PREFIX mount -o loop $RF_NAME $RF_MNT
  $CMD_PREFIX rm -rf $RF_MNT/lost+found
  $CMD_PREFIX mkdir -p $RF_MNT/{apps,bin,data,dev,info,proc,root,sbin,sys,tmp,var,etc/{,init.d,dropbear},mnt/{,usb,sdc,nfs,dev},usr/{,bin,sbin,lib,share},lib/{,modules/{,2.6.24,2.6.38}}} 

  $CMD_PREFIX mknod -m666 $RF_MNT/dev/null c 1 3 
  $CMD_PREFIX mknod -m666 $RF_MNT/dev/console c 5 1
  $CMD_PREFIX mknod -m666 $RF_MNT/dev/ttyS0 c 4 64

  $CMD_PREFIX ln -s /tmp $RF_MNT/var/lock
  $CMD_PREFIX ln -s /tmp $RF_MNT/var/log
  $CMD_PREFIX ln -s /tmp $RF_MNT/var/run
  $CMD_PREFIX ln -s /tmp $RF_MNT/var/tmp

  $CMD_PREFIX umount $RF_NAME
  $CMD_PREFIX tune2fs -i 0 -c 0 $RF_NAME

  rm -rf $RF_MNT
}

# Description:  This function used to uncomperss the rootfs and mount it to ./mnt
#      Author:  GuoWenxue<guowenxue@gmail.com>
#     Version:  1.0.0 (Release by guowenxue on 24th Apr. 2011)
# 

function ramdisk_mnt ()
{
  MNT_POINT=mnt

  if [ 1 != $# ] ; then
     echo "This function used to uncomperss the rootfs and mount it to ./mnt"
     echo "Usage: $FUNCNAME [rootfs]"
     return;
  fi

  #Rootfs packet name
  RF_PACK=$1
  CMD_PREFIX=

  if [ root != `whoami` ] ; then
     echo "WARNING: Not root user, use sudo to excute this function commands"
     CMD_PREFIX=sudo
  fi

  if [ ! -s $RF_PACK ] ; then
     echo "ERROR: Rootfs \"$RF_PACK\" doesn't exist"
     return;
  fi

  #Rootfs uncompressed name
  RF_NAME=`echo $RF_PACK | awk -F '.' '{print $1}'`
  ex $RF_PACK

  if [ ! -d $MNT_POINT ] ; then
     mkdir $MNT_POINT
  fi

  $CMD_PREFIX mountpoint $MNT_POINT
  if [ 0 == $? ] ; then
     echo "ERROR:  $RF_NAME not mounted"
     return;
  fi

  set -x
  $CMD_PREFIX mount -o loop $RF_NAME $MNT_POINT
  set +x
}


# Description:  This function used to umount ramdisk rootfs and compress it
#      Author:  GuoWenxue<guowenxue@gmail.com>
#     Version:  1.0.0 (Release by guowenxue on 24th Apr. 2011)
# 

function ramdisk_gen ()
{
  if [ 1 != $# ] ; then
     echo "This function used to umount ramdisk rootfs and compress it"
     echo "$FUNCNAME [rootfs]"
     return;
  fi

  #Rootfs name
  RF_NAME=$1
  CMD_PREFIX=

  if [ root != `whoami` ] ; then
     echo "WARNING: Not root user, use sudo to excute this function commands" 
     CMD_PREFIX=sudo
  fi

  file $RF_NAME | grep "filesystem" >> /dev/null 2>&1
  if [ 0 != $? ] ; then
     echo "$RF_NAME is not linux initrd file system" 
     return;
  fi

  file $RF_NAME | grep "mount" >> /dev/null 2>&1
  if [ 0 == $? ] ; then
     $CMD_PREFIX echo "version=file system Build `date +"%Y-%m-%d"`" > fs.fcfg 
     $CMD_PREFIX mv fs.fcfg mnt/etc/fs.fcfg && $CMD_PREFIX chown root.root mnt/etc/fs.fcfg
     $CMD_PREFIX umount mnt
     $CMD_PREFIX rm -rf mnt
  fi

  echo "Compress ramdisk now..."
  gzip $RF_NAME

  mkimage -A arm -O linux -T filesystem -C gzip -d $RF_NAME.gz $RF_NAME.gz-magic
}

# Description:  This function used to genrate the cramfs rootfs image from a rootfs tree 
#      Author:  GuoWenxue<guowenxue@gmail.com>
#     Version:  1.0.0 (Release by guowenxue on 23th Sep. 2012)
# 

function cramfs_gen ()
{
  MKCRAMFS_TOOL=/usr/local/bin/mkcramfs
  if [ 2 != $# ] ; then
     echo "This function used to genrate the cramfs rootfs image from a rootfs tree."
     echo "Usage: $FUNCNAME [rootfs dir] [rootfs image]"
     return;
  fi

  #Rootfs packet name
  ROOTFS_DIR=$1
  ROOTFS_IMG=$2
  
  CMD_PREFIX=

  if [ root != `whoami` ] ; then
     echo "WARNING: Not root user, use sudo to excute this function commands"
     CMD_PREFIX=sudo
  fi

  $CMD_PREFIX echo "version=file system Build `date +"%Y-%m-%d"`" > fs.fcfg && sudo mv fs.fcfg $ROOTFS_DIR/etc/fs.fcfg
  $CMD_PREFIX $MKCRAMFS_TOOL $ROOTFS_DIR $ROOTFS_IMG
}

# Description:  This function used to generate the APM(ARM Packet Manager) file
#      Author:  GuoWenxue<guowenxue@gmail.com>
#     Version:  1.0.0 (Release by guowenxue on 11st Jun. 2012)

function apm_gen ()
{
  path=`pwd`
  folder=`basename $path`
  apm_name=${folder}.apm

  if [ ! -s patch.sh ] ; then 
     echo "Miss patch.sh file, not APM file? exit now..."
     return;
  fi

  #first, compress these sub folders and name them as ${foldername}.apm
  for i in N??*; do
       tar -czf $i.apm $i
       rm -rf $i
  done

  #second, generate the final APM file
  cd ..
  tar -czf $apm_name ${folder}
  rm -rf $folder
}


# Description:  This function used to uncompress the APM(ARM Packet Manager) file
#      Author:  GuoWenxue<guowenxue@gmail.com>
#     Version:  1.0.0 (Release by guowenxue on 11st Jun. 2012)
# 

function apm_uncmp ()
{
  if [ $# -ne 1 ]; then
      echo "This function used to uncompress the APM file"
      echo "Usage:   $FUNCNAME \"apm_path\"             "
      return;
  fi

  apm_path=$1
  apm_name=`basename ${apm_path}`
  work_path=`echo ${apm_name}|awk -F '.' '{print   $1}'`


  # Uncompress the TOP APM file
  tar -xzf $apm_path 

  if [ ! -d  $work_path ];then
      echo "===================================================================="
      echo "*  ERROR: Uncompress failure, make sure the file is tar.gz format  *"
      echo "===================================================================="
      echo ""
      return;
  fi

  # Goes into the folder and uncompress the child NUP file one by one.
  cd $work_path

  for i in *.apm; do
      tar -xzf $i
      rm -f $i
  done
}

