#!/bin/bash

QT_INST_PATH=/apps/qt5-everywhere
QT_PACK=qt-everywhere-opensource-src-5.9.1

CROSS_TOOLS=/opt/buildroot-2012.08/arm920t/usr/bin/arm-linux-
export PATH=`dirname $CROSS_TOOLS`:$PATH
CROSS_PREFIX=`basename $CROSS_TOOLS`


if [ ! -d $QT_PACK ] ; then
    if [ -f $QT_PACK.tar.gz ] ; then 
        echo "Start decompress $QT_PACK.tar.gz now ...."
        tar -xzf $QT_PACK.tar.gz
    else
        echo "ERROR: $QT_PACK.tar.gz not exist here"
        exit 1;
    fi

    if [ $? != 0 ] ; then
        echo "ERROR: Decompress $QT_PACK.tar.gz failure"
        exit 1;
    fi

    cd $QT_PACK
    sed -i "s/arm-linux-gnueabi-/$CROSS_PREFIX/" qtbase/mkspecs/linux-arm-gnueabi-g++/qmake.conf
else
    cd $QT_PACK
fi


./configure -opensource -confirm-license -release -shared -strip -no-c++11 -prefix ${QT_INST_PATH} \
-xplatform linux-arm-gnueabi-g++ -no-opengl -qt-freetype -no-avx  -make libs -optimized-qmake \
-qt-zlib -no-gif -qt-libpng  -qt-libjpeg -no-openssl -no-cups -no-dbus -linuxfb -pch -qt-sql-sqlite \
-no-sse2 -no-sse3 -no-ssse3 -no-sse4.1 -no-sse4.2 -no-rpath -no-glib -qreal float -no-xcb \
-no-xcursor -no-xfixes -no-xrandr -no-xrender -no-separate-debug-info -nomake tools -nomake examples \
-no-tslib 

make -j8 && make install

