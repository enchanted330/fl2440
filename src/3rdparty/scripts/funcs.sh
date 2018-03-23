#!/bin/sh

CROSS=
ARCH=

function set_crosstool()
{
    CROSS=$1
    export CC=${CROSS}gcc
    export LD=${CROSS}ld
    export AS=${CROSS}as
    export CXX=${CROSS}g++
    export AR=${CROSS}ar
    export NM=${CROSS}nm
    export STRIP=${CROSS}strip
    export OBJCOPY=${CROSS}objcopy
    export OBJDUMP=${CROSS}objdump
    export RANLIB=${CROSS}ranlib
}

function set_env()
{
    export APPS_PATH=/apps/${ARCH}/

    if [ $ARCH == "x86" -o $ARCH == "X86" ] ; then
       return ;
    fi

    export ROOTFS_PATH=`pwd`/../../rootfs/rootfs
    export CONFIG_CROSS=--host=arm-linux

    if [ ! -d $APPS_PATH ] ; then
        mkdir -p $APPS_PATH
    fi
}

sup_arch=("" "arm926t" "arm920t" "arm1176jzfs" "x86" )
function clear_crossenv()
{
    unset CC
    unset LD
    unset AS
    unset AR
    unset NM
    unset CXX
    unset STRIP
    unset OBJCOPY
    unset OBJDUMP

    unset ARCH
    unset CPU
    unset CROSS
    unset INST_PATH
    unset C_INCLUDE_PATH
    unset LDFLAGS
    unset CFLAGS
}

function select_arch()
{
    if [ -n "$ARCH" ] ; then 
        return 0
    fi

    echo "Current support ARCH: " 
    i=1
    
    len=${#sup_arch[*]} 
    
    while [ $i -lt $len ]; do
        echo "$i: ${sup_arch[$i]}"
        let i++;
    done

    echo "Please select: "
    index=
    read index
    ARCH=${sup_arch[$index]} 
}


function decompress_packet()
(          
    if [ $# != 2 ] ; then
        echo "Usage: $1 [APP_NAME] [PACK_SUFIX]"
        return 1;
    fi
    dir_name=$1
    pack_sufix=$2
    pack_name=$1.$2

    echo "+---------------------------------------------+"
    echo "|  Remove and decompress $pack_name now"  
    echo "+---------------------------------------------+"

    rm -rf $dir_name

    case "$pack_sufix" in
        "zip")
            unzip $pack_name ;; 
        "tar") 
            tar -xf "$pack_name" ;;
        "gz") 
            gzip -d "$pack_name" ;;
        "tar.gz")
            tar -xzf "$pack_name" ;;
        "tgz")
            tar -xzf "$pack_name" ;;
        "bz2") 
            bunzip2 "$pack_name" ;;
        "tar.bz2")
            tar -xjf "$pack_name" ;;
        "tbz2")
            tar -xjf "$pack_name" ;;
        *)
            echo "$pack_name is unknow compress format" ;;
        esac
)

function download()
{
    dl_addr=$1
    file_name=`echo ${dl_addr} | awk -F "/" '{print $NF}'`

    if [ ! -f $file_name ] ; then 
        echo "+---------------------------------------------+"
        echo "|  Download $file_name now"  
        echo "+---------------------------------------------+"
        wget --no-check-certificat ${dl_addr}
    fi

    if [ ! -f $file_name ] ; then
        echo "ERROR: Download $file_name failure, exit now..."
        exit -1;
    fi
}

