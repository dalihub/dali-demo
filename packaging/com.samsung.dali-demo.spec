%bcond_with wayland

Name:       com.samsung.dali-demo
Summary:    The OpenGLES Canvas Core Demo
Version:    2.5.42
Release:    1
Group:      System/Libraries
License:    Apache-2.0 and BSD-3-Clause and MIT
URL:        https://review.tizen.org/git/?p=platform/core/uifw/dali-demo.git;a=summary
Source0:    %{name}-%{version}.tar.gz

Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig

BuildRequires:  cmake
BuildRequires:  pkgconfig
BuildRequires:  pkgconfig(capi-appfw-application)
BuildRequires:  pkgconfig(capi-appfw-app-control)
BuildRequires:  pkgconfig(capi-media-player)
BuildRequires:  pkgconfig(dlog)
BuildRequires:  gettext-tools
BuildRequires:  pkgconfig(dali2-core)
BuildRequires:  pkgconfig(dali2-adaptor)
BuildRequires:  pkgconfig(dali2-toolkit)
BuildRequires:  pkgconfig(dali2-scene3d)
BuildRequires:  pkgconfig(dali2-physics-2d)
BuildRequires:  pkgconfig(dali2-physics-3d)
BuildRequires:  dali2-integration-devel
BuildRequires:  dali2-adaptor-integration-devel
BuildRequires:  dali2-toolkit-integration-devel
BuildRequires:  dali2-scene3d-integration-devel
BuildRequires:  pkgconfig(libtzplatform-config)
BuildRequires:  pkgconfig(gles20)
BuildRequires:  pkgconfig(glesv2)
BuildRequires:  pkgconfig(libtbm)

%description
The OpenGLES Canvas Core Demo is a collection of examples and demonstrations
of the capability of the toolkit.

##############################
# Resources
##############################

%package resources_720x1280
Summary:    Specific resource files for a 720x1280 display
Requires:   %{name} = %{version}-%{release}
Conflicts:  %{name}-resources_1920x1080_rpi
Provides:   %{name}-resources_mobile = %{version}-%{release}
Obsoletes:  %{name}-resources_mobile < %{version}-%{release}
%description resources_720x1280
dali-demo specific resource files for a 720x1280 display
Contains style / style images

%package resources_1920x1080_rpi
Summary:    Specific resource files for a 1920x1080 display on Raspberry Pi 4
Requires:   %{name} = %{version}-%{release}
Conflicts:  %{name}-resources_720x1280
Provides:   %{name}-resources_rpi = %{version}-%{release}
Obsoletes:  %{name}-resources_rpi < %{version}-%{release}
%description resources_1920x1080_rpi
dali-demo specific resource files for a 1920x1080 display on Raspberry Pi 4
Contains style / style images

##############################
# Preparation
##############################
%prep
%setup -q

%define dali_app_ro_dir       %TZ_SYS_RO_APP/com.samsung.dali-demo/
%define dali_xml_file_dir     %TZ_SYS_RO_PACKAGES
%define dali_icon_dir         %TZ_SYS_RO_ICONS
%define smack_rule_dir        %TZ_SYS_SMACK/accesses2.d/

%define dali_app_res_dir      %{dali_app_ro_dir}/res/
%define dali_app_exe_dir      %{dali_app_ro_dir}/bin/
%define locale_dir            %{dali_app_res_dir}/locale

%if 0%{?rpi_style}
%define local_style_dir       ../../resources/style/1920x1080_rpi
%else
%define local_style_dir       ../../resources/style/720x1280
%endif
# Both resolutions ship the same images.
%define local_style_images_dir ../../resources/style/images-common

##############################
# Build
##############################
%build
PREFIX="/usr"
CXXFLAGS="$CXXFLAGS -Wall -g -O2"
LDFLAGS="$LDFLAGS -Wl,--rpath=$PREFIX/lib -Wl,--as-needed"

%ifarch %{arm}
CXXFLAGS="$CXXFLAGS -D_ARCH_ARM_"
%endif

cd %{_builddir}/%{name}-%{version}/build/tizen

cmake -DDALI_APP_DIR=%{dali_app_ro_dir}/bin \
      -DLOCALE_DIR=%{locale_dir} \
      -DDALI_APP_RES_DIR=%{dali_app_res_dir} \
%if 0%{?enable_debug}
      -DCMAKE_BUILD_TYPE=Debug \
%endif
      -DENABLE_TRACE:BOOL=ON \
      -DLOCAL_STYLE_DIR=%{local_style_dir} \
      -DLOCAL_STYLE_IMAGES_DIR=%{local_style_images_dir} \
      -DINTERNATIONALIZATION:BOOL=OFF \
      -DTIZEN:BOOL=ON \
%if "%{?build_example_name}" != ""
      -DBUILD_EXAMPLE_NAME=%{?build_example_name} \
%endif
      .

make %{?jobs:-j%jobs}

##############################
# Installation
##############################
%install
rm -rf %{buildroot}
cd build/tizen
%make_install DALI_APP_DIR=%{dali_app_ro_dir}/bin
%make_install DDALI_APP_RES_DIR=%{dali_app_res_dir}

mkdir -p %{buildroot}%{dali_xml_file_dir}
cp -f %{_builddir}/%{name}-%{version}/%{name}.xml %{buildroot}%{dali_xml_file_dir}

mkdir -p %{buildroot}%{dali_icon_dir}
mv %{buildroot}/%{dali_app_res_dir}/images/%{name}.png %{buildroot}%{dali_icon_dir}
mv %{buildroot}/%{dali_app_res_dir}/images/dali-examples.png %{buildroot}%{dali_icon_dir}
mv %{buildroot}/%{dali_app_res_dir}/images/dali-tests.png %{buildroot}%{dali_icon_dir}

%if 0%{?enable_dali_smack_rules} && !%{with wayland}
mkdir -p %{buildroot}%{smack_rule_dir}
cp -f %{_builddir}/%{name}-%{version}/%{name}.rule %{buildroot}%{smack_rule_dir}
%endif

cp -rf %{_builddir}/%{name}-%{version}/resources/style/1920x1080_rpi %{buildroot}/%{dali_app_res_dir}/style_rpi
cp -rf %{_builddir}/%{name}-%{version}/resources/style/images-common %{buildroot}/%{dali_app_res_dir}/style_rpi/images

##############################
# Post Install
##############################
%post
/sbin/ldconfig
exit 0

%post resources_1920x1080_rpi
_saved_dir=$(pwd)
cd %{dali_app_res_dir}
rm -rf style
mv style_rpi style
cd "$_saved_dir"

##############################
# Pre Uninstall
##############################

%preun resources_1920x1080_rpi
case "$1" in
  0)
    %preun resources_1920x1080_rpi
    _saved_dir=$(pwd)
    cd %{dali_app_res_dir}
    mv style style_rpi
    cd "$_saved_dir"
  ;;
esac

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
%{dali_app_exe_dir}/dali-examples
%{dali_app_exe_dir}/dali-tests
%{dali_app_exe_dir}/*.example
%{dali_app_exe_dir}/dali-builder
%if "%{?build_example_name}" == ""
%{dali_app_res_dir}/images/*
%{dali_app_res_dir}/game/*
%{dali_app_res_dir}/videos/*
%{dali_app_res_dir}/models/*
%endif
%{dali_app_res_dir}/scripts/*
%{dali_app_res_dir}/shaders/*
%{dali_xml_file_dir}/%{name}.xml
%{dali_icon_dir}/*
%if 0%{?enable_dali_smack_rules} && !%{with wayland}
%config %{smack_rule_dir}/%{name}.rule
%endif
%license LICENSE

%files resources_720x1280
%if 0%{?enable_dali_smack_rules}
%manifest com.samsung.dali-demo.manifest-smack
%else
%manifest com.samsung.dali-demo.manifest
%endif
%defattr(-,root,root,-)
%{dali_app_res_dir}/style/*
%{dali_app_res_dir}/style/images/*

%files resources_1920x1080_rpi
%if 0%{?enable_dali_smack_rules}
%manifest com.samsung.dali-demo.manifest-smack
%else
%manifest com.samsung.dali-demo.manifest
%endif
%defattr(-,root,root,-)
%{dali_app_res_dir}/style_rpi/*
%{dali_app_res_dir}/style_rpi/images/*
