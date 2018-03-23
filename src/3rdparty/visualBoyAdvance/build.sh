#!/bin/bash  
#+--------------------------------------------------------------------------------------------
#|Description:  This shell script used to download VisualBoyAdvance and compile it, which is
#|              a game box can runs on FL2440.
#|     Author:  GuoWenxue <guowenxue@gmail.com>
#|  ChangeLog:
#|           1, Initialize 1.0.0 on 20125.04.23 by WeiShusheng
#|           2, Modify to static compile by guowenxue
#+--------------------------------------------------------------------------------------------

PRJ_PATH=`pwd`
PREFIX_PATH=$PRJ_PATH/install

PNG_NAME="libpng-1.4.3"
PNG_SUFIX="tar.bz2"
PNG_ADDR="http://down1.chinaunix.net/distfiles/libpng-1.4.3.tar.bz2"

SDL_NAME="SDL-1.2.13"
SDL_SUFIX="tar.gz"
SDL_ADDR="http://down1.chinaunix.net/distfiles/SDL-1.2.13.tar.gz"

VBOY_NAME="VisualBoyAdvance"
VBOY_SRC="-src-1.7.2"
VBOY_NOSRC="-1.7.2"
VBOY_SUFIX="tar.gz"
VBOY_ADDR="http://downloads.sourceforge.net/project/vba/VisualBoyAdvance/1.7.2/VisualBoyAdvance-src-1.7.2.tar.gz?r=http%3A%2F%2Fsourceforge.net%2Fprojects%2Fvba%2Ffiles%2FVisualBoyAdvance%2F1.7.2%2F&ts=1429342329&use_mirror=ncu
"

if [ ! -d $PREFIX_PATH ] ; then
    mkdir -p $PREFIX_PATH
fi

CROSS=/opt/buildroot-2012.08/arm920t/usr/bin/arm-linux-
export CC=${CROSS}gcc
export CXX=${CROSS}g++
export CPP=${CROSS}cpp
export AS=${CROSS}as
export LD=${CROSS}ld
export AR=${CROSS}ar
export RANLIB=${CROSS}ranlib
export STRIP=${CROSS}strip

cd ${PRJ_PATH}/
if [ ! -f $PREFIX_PATH/lib/libpng.a ] ; then 
    # Download libpng source code packet 
    if [ ! -s $PNG_NAME.$PNG_SUFIX ] ; then 
        echo "+------------------------------------------------------------------+" 
        echo "|  Download $PNG_NAME.$PNG_SUFIX now "  
        echo "+------------------------------------------------------------------+" 
        wget $PNG_ADDR 
    fi 

    #unzip libpng
    if [ ! -d $PNG_NAME ];then 
        tar -xjf $PNG_NAME.$PNG_SUFIX 

        if [ $? != 0 ] ; then
            echo "Decompress $PNG_NAME.$PNG_SUFIX failure and exit now..."
            exit 1;
        else 
            echo "Decompress $PNG_NAME.$PNG_SUFIX and compile it..."
        fi
    fi

    cd ${PRJ_PATH}/$PNG_NAME
    ./configure  --host=arm-linux --prefix=$PREFIX_PATH --enable-static --disable-shared
    make && make install
    if [ $? != 0 ] ; then
        echo "Compress $PNG_NAME failure, exit now..."
        exit 1;
    fi
fi

cd ${PRJ_PATH}/
if [ ! -f $PREFIX_PATH/lib/libSDL.a ] ; then 
    # Download SDL source code packet 
    if [ ! -s $SDL_NAME.$SDL_SUFIX ] ; then 
        cd ${PRJ_PATH}/
        echo "+------------------------------------------------------------------+" 
        echo "|  Download $SDL_NAME.$SDL_SUFIX now "  
        echo "+------------------------------------------------------------------+" 
        wget $SDL_ADDR 
    fi 


    #unzip SDL 
    if [ ! -d $SDL_NAME ];then 
        tar -xzf $SDL_NAME.$SDL_SUFIX 

        if [ $? != 0 ] ; then
            echo "Decompress $SDL_NAME.$SDL_SUFIX failure and exit now..."
            exit 1; 
        else
            echo "Decompress $SDL_NAME.$SDL_SUFIX and compile it..."
        fi
    fi 

    cd ${PRJ_PATH}/$SDL_NAME 
    ./configure  --host=arm-linux --prefix=$PREFIX_PATH --enable-static --disable-shared
    make && make install

    if [ $? != 0 ] ; then
        echo "Compress $SDL_NAME failure, exit now..."
        exit 1;
    fi
fi


cd ${PRJ_PATH}/
if [ ! -f VisualBoyAdvance ] ; then 
    # Download VBOY source code packet 
    if [ ! -s $VBOY_NAME$VBOY_SRC.$VBOY_SUFIX ] ; then 
        echo "+------------------------------------------------------------------+" 
        echo "|  Download $VBOY_NAME$VBOY_SRC.$VBOY_SUFIX now " 
        echo "+------------------------------------------------------------------+" 
        wget $VBOY_ADDR 
    fi 

    #unzip VBOY 
    if [ ! -d $VBOY_NAME$VBOY_NOSRC ];then 
        tar -xzf $VBOY_NAME$VBOY_SRC.$VBOY_SUFIX 

        if [ $? != 0 ] ; then
            echo "Decompress $VBOY_NAME$VBOY_SRC.$VBOY_SUFIX failure and exit now..."
            exit 1; 
        else
            echo "Decompress $VBOY_NAME$VBOY_SRC.$VBOY_SUFIX and compile it..."
        fi
    fi 
    
    cd ${PRJ_PATH}/$VBOY_NAME$VBOY_NOSRC 
    sed -i 's/utilGzWriteFunc = (int (\*)(void \*,void \* const, unsigned int))gzwrite;/utilGzWriteFunc = (int (\*)(gzFile_s\*,void \* const, unsigned int))gzwrite;/' src/Util.cpp 

    export CFLAGS+="-I$PREFIX_PATH/include -I$PREFIX_PATH/include/SDL"
    export CXXFLAGS+="-I$PREFIX_PATH/include -I$PREFIX_PATH/include/SDL"
    export LDFLAGS="-L$PREFIX_PATH/lib" 
    export SDL_CONFIG=$PREFIX_PATH/bin
    export LIBS+=-lSDL
    if [ -f Makefile ] ; then 
        make distclean 
    fi
    ./configure --host=arm-linux --prefix=$PREFIX_PATH --with-sdl-exec-prefix=$PREFIX_PATH --with-sdl-exec-prefix=$PREFIX_PATH
    make && make install
    ${STRIP} $PREFIX_PATH/bin/VisualBoyAdvance
fi

