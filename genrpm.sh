#!/bin/bash
VERSION=`grep VERSION skypetab-ng.pro|sed "s/.*= *//"`
echo Preparing environment
rm -rf tmprpm
mkdir -p tmprpm/rpmbuild/{BUILD,RPMS,SOURCES,SPECS,SRPMS}
mkdir -p tmprpm/skypetab-ng-$VERSION
cp -R * tmprpm/skypetab-ng-$VERSION
cd tmprpm
tar czf skypetab-ng.tar.gz skypetab-ng-$VERSION
mv skypetab-ng.tar.gz rpmbuild/SOURCES
cp ../skypetab-ng-rpmlintrc rpmbuild/SOURCES
cp ../skypetab-ng.spec rpmbuild/SPECS
echo Building
rpmbuild -bs ../skypetab-ng.spec --buildroot `pwd`/build/ --define='%_topdir %(echo `pwd`)/rpmbuild'
cp rpmbuild/SRPMS/* ../skypetab-ng.src.rpm
rm -rf tmprpm
