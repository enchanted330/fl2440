#!/bin/sh

#+--------------------------------------------------------------------------------------------
#|Description:  This shell script used to download lrzsz source code and cross compile it.
#|     Author:  GuoWenxue <guowenxue@gmail.com>
#|  ChangeLog:
#|           1, Initialize 1.0.0 on 2011.12.23
#+--------------------------------------------------------------------------------------------
. ../scripts/funcs.sh
clear_crossenv

ARCH=arm920t
select_arch
if [ "x86" != "$ARCH" ] ; then
    CROSS="/opt/buildroot-2012.08/${ARCH}/usr/bin/arm-linux-"
    set_crosstool $CROSS
    set_env
fi

APP_NAME=appweb
#DIR_NAME="appweb-3.3.4"
DIR_NAME="appweb-3.4.2"
PACK_SUFIX="-0-src.tgz"
DL_ADDR="https://embedthis.com/software/$DIR_NAME$PACK_SUFIX"
PROJ_PATH=`pwd`

PREFIX_PATH=${APPS_PATH}/${APP_NAME}
PHP_LIB_PATH=${APPS_PATH}/php/
PHP_PATH=`pwd`/../php
PHP_DIR=`cat $PHP_PATH/build.sh | grep "^DIR_NAME=" | cut -d= -f 2`
PHP_SRC_PATH=$PHP_PATH/$PHP_DIR

if [ ! -d "$PHP_SRC_PATH" ] ; then
    cd $PHP_PATH
    sh build.sh
    cd -
fi

select_arch
if [ "x86" != "$ARCH" ] ; then
    CROSS="/opt/buildroot-2012.08/${ARCH}/usr/bin/arm-linux-"
    set_crosstool $CROSS
    set_env
fi

# Download and decompress source code packet 
download $DL_ADDR
rm -rf $DIR_NAME
tar -xzf $DIR_NAME$PACK_SUFIX

if [ "$LINK_STATIC" == "YES" ] ; then
    export CFLAGS=--static
    export LDFLAGS=-static
    CONFIG_LD_STATUS='--enable-static --disable-shared'
fi

echo "+------------------------------------------------------------------+"
echo "|  Platform:  $DIR_NAME for $ARCH  "
echo "|  Compiler:  ${CROSS}gcc  "
echo "+------------------------------------------------------------------+"
mkdir -p $PREFIX_PATH/{bin,modules}

cd $DIR_NAME
   set -x

   #--type=RELEASE --shared --port=80 --enable-assert --enable-complete-cross \
   ./configure $CONFIG_CROSS --prefix=$PREFIX_PATH \
   --type=RELEASE --static --port=80 --enable-assert --enable-complete-cross \
   --enable-legacy-api --enable-log --enable-multi-thread --enable-samples \
   --enable-test --enable-send --enable-upload --enable-file --enable-regex \
   --with-php=$PHP_SRC_PATH
   make TRACE=1 

   if [ $? != 0 ] ; then
       echo "Cross compile failure, exit now"
       exit
   fi
   
   $STRIP bin/arm-unknown-linux/appweb
   cp bin/arm-unknown-linux/appweb $PREFIX_PATH/bin
   cp modules/arm-unknown-linux/* $PREFIX_PATH/modules
   cp -rf test/{appweb.conf,groups.db,mime.types,users.db} $PREFIX_PATH

   cd ${PROJ_PATH}
   tar -xzf web.tar.gz -C $PREFIX_PATH
   sed -i -e "s|^Listen 4100|Listen 80|g" $PREFIX_PATH/appweb.conf
   sed -i -e 's|^LoadModulePath "../modules"|LoadModulePath "./modules"|g' $PREFIX_PATH/appweb.conf
   cp $PHP_LIB_PATH/lib/libphp5.so $PREFIX_PATH/modules
cat << EOF > run.sh
#!/bin/sh
APPWEB_PATH=/apps/appweb
export LD_LIBRARY_PATH=\$LD_LIBRARY_PATH:\$APPWEB_PATH/modules

cd \$APPWEB_PATH
./bin/appweb --config appweb.conf &
EOF
   chmod 755 run.sh
   mv run.sh $PREFIX_PATH/

   cd `dirname $PREFIX_PATH`
   tar -czf appweb_$ARCH.tar.gz `basename $PREFIX_PATH`
   mv appweb_$ARCH.tar.gz $PROJ_PATH

   echo "+------------------------------------------------------------------+"
   echo "|   Install Prefix: $PREFIX_PATH"
   echo "|   Install   Path: $INST_PATH"
   echo "+------------------------------------------------------------------+"
cd -

