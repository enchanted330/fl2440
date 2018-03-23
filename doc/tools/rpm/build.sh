#!/bin/sh

mkdir -p rpmbuild/{BUILD,BUILDROOT,RPMS,SOURCES,SPECS,SRPMS}
cp apps-1.0.0.tar.gz rpmbuild/SOURCES
rpmbuild -bb --clean --target armv4tl apps.spec
mv rpmbuild/RPMS/armv4tl/apps-1.0.0-1.armv4tl.rpm s3c24x0-apps-1.0.0-1.rpm
rm -rf rpmbuild ~/rpmbuild/
