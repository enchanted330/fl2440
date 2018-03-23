说明：   交叉编译器可以自己制作，也可以直接使用我们做好的bin文件安装。
   
1, 直接使用bin文件安装： 
   百度网盘  凌云嵌入式共享 > Part04-1. 嵌入式开发工具 >  buildroot-2012.08-LingYunEmbLab-v1.2.0.bin
    [lingyun@centos6 crosstool]$ sh buildroot-2012.08-LingYunEmbLab-v1.2.0.bin 
	+-------------------------------------------------------------------------------------------------------+
	|  Lingyun Embedded System Laboratory<www.emblinux.com> buildroot-2012.08 packet installer for CentOS6  |
	|  Please report bugs or feature suggestions to <Email: guowenxue@gmail.com QQ: 281143292>              |
	+-------------------------------------------------------------------------------------------------------+

	Current support ARCH: 
	1: arm920t      <CPU: s3c2410,s3c2440,at91sam9200...>
	2: arm926t      <CPU: s3c2416,at91sam9260/9g20,at91sam9x35...>
	3: arm1176jzfs  <CPU: s3c6410...>
	Please select:  <FL2440输入1, OK6410输入3>
	
	buildroot-2012.08 will be compiled for ARCH [arm920t]

	Please input buildroot-2012.08 install path, or ENTER for default path [/opt]:  <直接回车默认选择/opt，或指定你的交叉编译器路径>

	
2, 参考config/buildroot-2011.11.HowTo自己制作,建议使用buildroot-2012.08。config文件夹下有各种交叉编译器制作工具的相应版本配置；

