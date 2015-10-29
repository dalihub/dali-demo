%bcond_with wayland

Name:       com.samsung.dali-demo
Summary:    The OpenGLES Canvas Core Demo
Version:    1.1.8
Release:    1
Group:      System/Libraries
License:    Apache-2.0
URL:        https://review.tizen.org/git/?p=platform/core/uifw/dali-demo.git;a=summary
Source0:    %{name}-%{version}.tar.gz

Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig
Requires: dali
Requires: dali-adaptor
Requires: dali-toolkit
BuildRequires:  cmake
BuildRequires:  pkgconfig
BuildRequires:  pkgconfig(capi-appfw-application)
BuildRequires:  pkgconfig(capi-media-player)
BuildRequires:  dali-toolkit-devel
BuildRequires:  dali-adaptor-devel
BuildRequires:  pkgconfig(dlog)
BuildRequires:  gettext-tools

%description
The OpenGLES Canvas Core Demo is a collection of examples and demonstrations
of the capability of the toolkit.

##############################
# Preparation
##############################
%prep
%setup -q
%define dali_app_ro_dir       /usr/apps/com.samsung.dali-demo/
%define dali_app_exe_dir      %{dali_app_ro_dir}/bin/
%define dali_xml_file_dir     /usr/share/packages/
%define dali_icon_dir         /usr/share/icons/
%define smack_rule_dir        /etc/smack/accesses2.d/
%define locale_dir            %{dali_app_ro_dir}/res/locale
%define local_style_dir       ../../resources/style/mobile

##############################
# Build
##############################
%build
PREFIX="/usr"
CXXFLAGS+=" -Wall -g -O2"
LDFLAGS+=" -Wl,--rpath=$PREFIX/lib -Wl,--as-needed -fPIC"

%ifarch %{arm}
CXXFLAGS+=" -D_ARCH_ARM_"
%endif

cd %{_builddir}/%{name}-%{version}/build/tizen && cmake -DDALI_APP_DIR=%{dali_app_ro_dir} -DLOCALE_DIR=%{locale_dir} -DLOCAL_STYLE_DIR=%{local_style_dir} .

make %{?jobs:-j%jobs}

##############################
# Installation
##############################
%install
rm -rf %{buildroot}
cd build/tizen
%make_install DALI_APP_DIR=%{dali_app_ro_dir}

mkdir -p %{buildroot}%{dali_xml_file_dir}
cp -f %{_builddir}/%{name}-%{version}/%{name}.xml %{buildroot}%{dali_xml_file_dir}

mkdir -p %{buildroot}%{dali_icon_dir}
mv %{buildroot}/%{dali_app_ro_dir}/images/%{name}.png %{buildroot}%{dali_icon_dir}

%if 0%{?enable_dali_smack_rules} && !%{with wayland}
mkdir -p %{buildroot}%{smack_rule_dir}
cp -f %{_builddir}/%{name}-%{version}/%{name}.rule %{buildroot}%{smack_rule_dir}
%endif

# LICENSE
mkdir -p %{buildroot}/usr/share/license
cp -af %{_builddir}/%{name}-%{version}/LICENSE %{buildroot}/usr/share/license/%{name}

##############################
# Post Install
##############################
%post
/sbin/ldconfig
exit 0

##############################
# Post Uninstall
##############################
%postun
/sbin/ldconfig
exit 0

##############################
# Files in Binary Packages
##############################

%files
%if 0%{?enable_dali_smack_rules}
%manifest com.samsung.dali-demo.manifest-smack
%else
%manifest com.samsung.dali-demo.manifest
%endif
%defattr(-,root,root,-)
%{dali_app_exe_dir}/dali-demo
%{dali_app_exe_dir}/*.example
%{dali_app_exe_dir}/dali-builder
%{dali_app_ro_dir}/images/*
%{dali_app_ro_dir}/models/*
%{dali_app_ro_dir}/scripts/*
%{dali_app_ro_dir}/style/*
%{dali_xml_file_dir}/%{name}.xml
%{dali_icon_dir}/*
%{locale_dir}/*
%if 0%{?enable_dali_smack_rules} && !%{with wayland}
%config %{smack_rule_dir}/%{name}.rule
%endif
%{_datadir}/license/%{name}
