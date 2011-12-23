Summary: This program adds tabs to Skype™ for Linux 
%define version 0.4.8
Group:  Applications/Internet
Name: skypetab-ng 
License: LGPL
Prefix: /
Provides: skypetab-ng 
Release: 1 
Source: skypetab-ng.tar.gz 
URL: http://keks-n.net/skypetab
Version: %{version} 
Buildroot: /tmp/skypetab-ng 
%if 0%{?suse_version}
BuildRequires: xorg-x11-libX11-devel
%define QMAKE qmake
%else
BuildRequires: libX11-devel
%define QMAKE /usr/lib/qt4/bin/qmake-qt4
%endif
BuildRequires: qt-devel
BuildRequires: gcc-c++
AutoReq:        no
BuildArch:      noarch
%global _binaries_in_noarch_packages_terminate_build 0
%global __os_install_post %(echo '%{__os_install_post}' | sed -e 's!/usr/lib[^[:space:]]*/brp-desktop[[:space:]].*$!!g')
%description
This program adds tabs to Skype™ for Linux

%prep 
%setup -q
%build
%{QMAKE} -spec linux-g++-32
make

%install 
rm -fr $RPM_BUILD_ROOT
make install INSTALL_ROOT=$RPM_BUILD_ROOT

%clean 
rm -rf $RPM_BUILD_ROOT

%files 
%defattr(-,root,root)
/usr/bin/skypetab-ng
/usr/lib/libskypetab-ng.*
/usr/share/applications/skypetab-ng.desktop
