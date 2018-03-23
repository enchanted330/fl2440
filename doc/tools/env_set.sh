#!/bin/sh

unset $ARCH
unset $BOARD

ARCH=$1
BOARD=$2

sup_arch=("" "arm920t" "arm926t")
function select_arch()
{
   echo "Current support ARCH:"
   i=1
   len=${#sup_arch[*]}

   while [ $i -lt $len ]; do
       echo "$i: ${sup_arch[$i]}"
       let i++;
   done

   if [ $len -eq 2 ] ; then
        ARCH=${sup_arch[1]}
        return;
   fi

   echo "Please select: "
   index=
   read index

   ARCH=${sup_arch[$index]} 
}

sup_boards=("" "fl2440" "sd2410" "at91sam9260" "at91sam9g20")
function select_board()
{
   echo "Current support boards:"
   i=1
   len=${#sup_boards[*]}

   while [ $i -lt $len ]; do
       echo "$i: ${sup_boards[$i]}"
       let i++;
   done

   if [ $len -eq 2 ] ; then
        BOARD=${sup_boards[1]}
        return;
   fi

   echo "Please select: "
   index=
   read index

   BOARD=${sup_boards[$index]}
}

# If not define ARCH, then let user choose a one
if [ -z $ARCH ] ; then 
    select_arch
fi

# If not define boards, then let user choose a one
if [ -z $BOARDS ] ; then 
    select_board
fi

CROSS="/opt/buildroot-2011.11/${ARCH}/usr/bin/arm-linux-"

export ARCH
export BOARD
export CROSS

echo ""
echo "|============================================================="
echo "| Choose setting as:"
echo "|     1,\$ARCH=$ARCH"
echo "|     2,\$BOARD=$BOARD"
echo "|     3,\$CROSS=$CROSS"
echo "|============================================================="
echo ""

bash

