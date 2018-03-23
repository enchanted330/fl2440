#!/bin/bash
# This shell script used to compile buildroot-2012.08 cross compiler
# Author: Guo Wenxue<guowenxue@gmail.com>

# Usage: 
#   I. Create an install bin file
#       [guowenxue@centos6 ~]$ tar -cjf buildroot-2012.08-packes.tar.bz2 buildroot-2012.08-packes
#       [guowenxue@centos6 ~]$ cat buildroot-install.sh buildroot-2012.08-packes.tar.bz2 > buildroot-2012.08-install.bin
#       [guowenxue@centos6 ~]$ chmod a+x buildroot-2012.08-install.bin


ARCH=
INST_PATH=
BUILDROOT=buildroot-2012.08
BUILDROOT_PACK_NAME=$BUILDROOT-packes

sup_arch=("" "arm920t" "arm926t" "arm1176jzfs" )
sup_cpu=("" "s3c2410,s3c2440,at91sam9200..." "s3c2416,at91sam9260/9g20,at91sam9x35..." "s3c6410..." )
function select_arch()
{
    if [ -n "$ARCH" ] ; then
        return 0
    fi

    echo "Current support ARCH: " 
    i=1

    len=${#sup_arch[*]}

    while [ $i -lt $len ]; do
        printf "%-15s %s\n" "$i: ${sup_arch[$i]}" "<CPU: ${sup_cpu[$i]}>"
        let i++;
    done

    echo "Please select: "
    index=
    read index
    ARCH=${sup_arch[$index]}

	if [ ! -n "$ARCH" ] ; then
		ARCH=arm920t
    fi

    echo "$BUILDROOT will be compiled for ARCH [$ARCH]"
    echo ""
}

function check_install_path()
{
    if [ ! -n "$INST_PATH" ] ; then
        echo -n "Please input $BUILDROOT install path, or ENTER for default path [/opt]:  "
        read INST_PATH
	fi
	
	if [ ! -n "$INST_PATH" ] ; then
		INST_PATH=/opt
    fi

    if [ ! -d $INST_PATH ] ; then
        echo "ERROR: Please create buildroot install path '$INST_PATH' first and make it writable by user '`whoami`'"
        exit 1;
    fi

    if [ ! -w $INST_PATH ] ; then
        echo "ERROR: Path '$INST_PATH' can not be written by user `whoami`, please give write privilege for user '`whoami`'"
        exit 2;
    fi
   
    echo "$BUILDROOT will be installed to [$INST_PATH]"
    echo ""
}

dep_tools=("" "automake" "autoconf" "bison" "gcc" "gcc-c++" "make" "ncurses" "zlib" "texinfo" "flex" )
function check_dep_tools()
{
    i=1
    status=1
    len=${#dep_tools[*]}

    echo "Check buildroot dependency system tools installed status..."

    while [ $i -lt $len ]; 
    do 
        printf "%-20s" "$i: ${dep_tools[$i]} "

        rpm -qa | grep ${dep_tools[$i]} > /dev/null 2>&1
        if [ $? == 0 ] ; then
           echo "[   OK   ]"
        else
            status=0
           echo "[ FAILED ]"
        fi

        let i++;
    done

    if [ $status == 0 ] ; then
        echo "ERROR:  please use yum install the miss system tools by root first!!!"
        exit 3;
    fi
}

function do_compile()
{
    cd $INST_PATH/$BUILDROOT 

    ln -s $BUILDROOT_PACK_NAME dl
    cp $BUILDROOT_PACK_NAME/config/$BUILDROOT-$ARCH.config .config

    echo "Compile $BUILDROOT for $ARCH..."
    make 
    echo "Compile $BUILDROOT successfully!!!"

    echo "Buildroot is already installed to '$INST_PATH/$BUILDROOT/$ARCH' on the system "
    echo "echo Do you wish to remove the compile temporary files? [y/n]"

    read clean_confirm 

    if [ "$clean_confirm" == "y" -o "$clean_confirm" == "Y" ]; then
        mv $ARCH $ARCH-
        make distclean
        mv $ARCH- $ARCH 
    fi 
}



echo ""
echo "+-------------------------------------------------------------------------------------------------------+"
echo "|  Lingyun Embedded System Laboratory<www.emblinux.com> $BUILDROOT packet installer for CentOS6  |"
echo "|  Please report bugs or feature suggestions to <Email: guowenxue@gmail.com QQ: 281143292>              |" 
echo "+-------------------------------------------------------------------------------------------------------+"
echo 

select_arch

check_install_path

if [ -d $INST_PATH/$BUILDROOT/$ARCH ] ; then
    echo "Buildroot is already installed to '$INST_PATH/$BUILDROOT/$ARCH' on the system, exit..."
    exit 0;
fi

if     [ -d $INST_PATH/$BUILDROOT/$BUILDROOT_PACK_NAME ] && 
       [ -f $INST_PATH/$BUILDROOT/$BUILDROOT_PACK_NAME/config/$BUILDROOT-$ARCH.config ] ; then 
    do_compile
    exit 0;
fi

check_dep_tools

echo ""
echo "Decompress buildroot crosstool packetes..."
sed -n '1,/^exit 0$/!p' $0 | tar -xj -C $INST_PATH > /dev/null 2>&1

if [ $? != 0 ] ; then
   echo "ERROR: Decompress buildroot crosstool packetes failure!!!"
   exit 4
fi

cd $INST_PATH

if [ ! -f $BUILDROOT_PACK_NAME/config/$BUILDROOT-$ARCH.config ] ; then
    echo "ERROR: Miss buildroot default configure file: '$BUILDROOT_PACK_NAME/config/$BUILDROOT-$ARCH.config'"
    exit 5;
fi

echo "Decompress $BUILDROOT packet..."
tar -xjf $BUILDROOT_PACK_NAME/$BUILDROOT.tar.bz2 > /dev/null 2>&1
if [ $? != 0 ] ; then
   echo "ERROR: Decompress $BUILDROOT.tar.bz2 failure!!!"
   exit 6
fi

mv $BUILDROOT_PACK_NAME $BUILDROOT

do_compile

#WARNNING: The last line must start and end by "exit 0" without any other character follwed!
exit 0
