#!/bin/sh
rm -rf .git
VERSION=`grep VERSION skypetab-ng.pro|sed "s/.*= *//"`
dpkg-buildpackage -S
sed -i "s/$VERSION-1) oneiric/$VERSION-1natty) natty/" debian/changelog
dpkg-buildpackage -S
sed -i "s/natty/maverick/" debian/changelog
dpkg-buildpackage -S
sed -i "s/maverick/lucid/" debian/changelog
dpkg-buildpackage -S
