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
AutoReq:        no
BuildArch:      noarch
%global _binaries_in_noarch_packages_terminate_build 0
%description
This program adds tabs to Skype™ for Linux

%prep 
%setup -q
%build
qmake -spec linux-g++-32
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
