#!/bin/sh
rm -rf .git
VERSION=`grep VERSION skypetab-ng.pro|sed "s/.*= *//"`

dpkg-buildpackage -S

sed -i "s/$VERSION-1) quantal/$VERSION-1precise) precise/" debian/changelog

dpkg-buildpackage -S

sed -i "s/precise/oneiric/g" debian/changelog
dpkg-buildpackage -S

#This fix is required because of the broken GTK theme loader
sed -i "s/sh$/sh\nunset GNOME_DISPLAY_SESSION_ID/" skypetab-ng
sed -i "s/sh$/sh\nunset DESKTOP_SESSION/" skypetab-ng

sed -i "s/oneiric/natty/g" debian/changelog
dpkg-buildpackage -S
sed -i "s/natty/maverick/g" debian/changelog
dpkg-buildpackage -S
sed -i "s/maverick/lucid/g" debian/changelog
dpkg-buildpackage -S
