#!/bin/sh
VERSION=`grep VERSION skypetab-ng.pro|sed "s/.*= *//"`
dpkg-buildpackage -S
sed -i "s/$VERSION-1) oneiric/$VERSION-1natty) natty/" debian/changelog
dpkg-buildpackage -S
sed -i "s/natty/maverick/g" debian/changelog
dpkg-buildpackage -S
sed -i "s/maverick/lucid/g" debian/changelog
dpkg-buildpackage -S
