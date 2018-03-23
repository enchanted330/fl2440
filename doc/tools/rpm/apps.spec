#
# Discription:  This is a sample SPEC file to build a RPM for our ARM embedded linux system
#   CopyRight:
#      Author:  GuoWenxue<guowenxue@gmail.com>
#       Usage:  [guowenxue@centos6 ~]$  rpmbuild -bb --target armv5tejl apps.spec 
#   ChangeLog:  
#            1, Initialize 1.0.0 on 2011.12.30 by guowenxue 


%define _topdir	%{getenv:PWD}/rpmbuild
%define __os_install_post   %{nil}

Name:		apps
Version:	1.0.0
Release:    1
Vendor:		GuoWenxue
Summary:	Wireless Router based on s3c2440 application upgrade packet 

Group:		System
License:	GPLV2
URL:		http://hi.baidu.com/kkernel
Source0:	%{name}-%{version}.tar.gz
BuildRoot:	%(mktemp -ud %{_tmppath}/%{name}-%{version}-XXXXXX)
Packager:	GuoWenxue <guowenxue@gmail.com>

%description
		This packet include some common tools and system configure file.

%prep
%setup -c

%install
		rm -rf $RPM_BUILD_ROOT/*
        #mkdir -p $RPM_BUILD_ROOT/apps/{etc/{init.d,network},lib,tools}
        mkdir -p $RPM_BUILD_ROOT/apps
        cp -af apps/* $RPM_BUILD_ROOT/apps/
                                
%clean
		rm -rf $RPM_BUILD_ROOT
		rm -rf %{_topdir}/BUILD/*

%files
%defattr(0755,root,root) 
/apps/tools/*
/apps/lib/*

%config
%defattr(0666,root,root) 
/apps/etc/*
%attr(0755,root,root) /apps/etc/init.d/*

%changelog

