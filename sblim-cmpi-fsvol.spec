%global provider_dir %{_libdir}/cmpi

Summary:        SBLIM fsvol instrumentation
Name:           sblim-cmpi-fsvol
Version:        1.5.1
Release:        12%{?dist}
License:        EPL
Group:          Applications/System
URL:            http://sourceforge.net/projects/sblim/
Source0:        http://downloads.sourceforge.net/project/sblim/providers/%{name}/%{version}/%{name}-%{version}.tar.bz2
Patch0:         sblim-cmpi-fsvol-1.5.0-ext4-support.patch
# Patch1: use Pegasus root/interop instead of root/PG_Interop
Patch1:         sblim-cmpi-fsvol-1.5.1-pegasus-interop.patch
# Patch2: backported from upstream
Patch2:         sblim-cmpi-fsvol-1.5.1-mounted-fs-shown-as-disabled.patch
# Patch3: fixes  mounted filesystem is shown as disabled when device mapper is used
Patch3:         sblim-cmpi-fsvol-1.5.1-mounted-dm-fs-shown-as-disabled.patch

BuildRequires:  sblim-cmpi-base-devel sblim-cmpi-devel
Requires:       sblim-cmpi-base cim-server
Requires:       /etc/ld.so.conf.d
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig

%description
Standards Based Linux Instrumentation Fsvol Providers

%package devel
Summary:        SBLIM Fsvol Instrumentation Header Development Files
Group:          Development/Libraries
Requires:       %{name} = %{version}-%{release}

%description devel
SBLIM Base Fsvol Development Package

%package test
Summary:        SBLIM Fsvol Instrumentation Testcases
Group:          Applications/System
Requires:       %{name} = %{version}-%{release}
Requires:       sblim-testsuite

%description test
SBLIM Base Fsvol Testcase Files for SBLIM Testsuite

%prep
%setup -q
%patch0 -p1 -b .ext4-support
%patch1 -p1 -b .pegasus-interop
%patch2 -p0 -b .mounted-fs-shown-as-disabled
%patch3 -p1 -b .mounted-dm-fs-shown-as-disabled

%build
%ifarch s390 s390x ppc ppc64
export CFLAGS="$RPM_OPT_FLAGS -fsigned-char"
%else
export CFLAGS="$RPM_OPT_FLAGS" 
%endif
%configure \
        TESTSUITEDIR=%{_datadir}/sblim-testsuite \
        PROVIDERDIR=%{provider_dir}
sed -i 's|^hardcode_libdir_flag_spec=.*|hardcode_libdir_flag_spec=""|g' libtool
sed -i 's|^runpath_var=LD_RUN_PATH|runpath_var=DIE_RPATH_DIE|g' libtool
make

%install
make install DESTDIR=$RPM_BUILD_ROOT
# remove unused libtool files
rm -f $RPM_BUILD_ROOT/%{_libdir}/*a
rm -f $RPM_BUILD_ROOT/%{provider_dir}/*a
# shared libraries
mkdir -p $RPM_BUILD_ROOT/%{_sysconfdir}/ld.so.conf.d
echo "%{_libdir}/cmpi" > $RPM_BUILD_ROOT/%{_sysconfdir}/ld.so.conf.d/%{name}-%{_arch}.conf

%files
%{_libdir}/libcmpiOSBase_CommonFsvol*.so.*
%{provider_dir}/libcmpiOSBase_LocalFileSystemProvider.so
%{provider_dir}/libcmpiOSBase_NFSProvider.so
%{provider_dir}/libcmpiOSBase_BlockStorageStatisticalDataProvider.so
%{provider_dir}/libcmpiOSBase_HostedFileSystemProvider.so
%{provider_dir}/libcmpiOSBase_BootOSFromFSProvider.so
%{_datadir}/%{name}
%docdir %{_datadir}/doc/%{name}-%{version}
%{_datadir}/doc/%{name}-%{version}
%config(noreplace) %{_sysconfdir}/ld.so.conf.d/%{name}-%{_arch}.conf

%files devel
%{_libdir}/libcmpiOSBase_CommonFsvol*.so
%{_includedir}/sblim/*Fsvol.h

%files test
%{_datadir}/sblim-testsuite/test-cmpi-fsvol.sh
%{_datadir}/sblim-testsuite/cim/*FileSystem.cim
%{_datadir}/sblim-testsuite/cim/*FS.cim
%{_datadir}/sblim-testsuite/cim/*BlockStorageStatisticalData.cim
%{_datadir}/sblim-testsuite/system/linux/*FileSystem.*
%{_datadir}/sblim-testsuite/system/linux/*FileSystemEntries.*

%global SCHEMA %{_datadir}/%{name}/Linux_Fsvol.mof
%global REGISTRATION %{_datadir}/%{name}/Linux_Fsvol.registration

%pre
function unregister()
{
  %{_datadir}/%{name}/provider-register.sh -d \
        $1 \
        -m %{SCHEMA} \
        -r %{REGISTRATION} > /dev/null 2>&1 || :;
  # don't let registration failure when server not running fail upgrade!
}
 
# If upgrading, deregister old version
if [ $1 -gt 1 ]
then
        unregistered=no
        if [ -e /usr/sbin/cimserver ]; then
           unregister "-t pegasus";
           unregistered=yes
        fi
 
        if [ -e /usr/sbin/sfcbd ]; then
           unregister "-t sfcb";
           unregistered=yes
        fi
 
        if [ "$unregistered" != yes ]; then
           unregister
        fi
fi

%post
function register()
{
  # The follwoing script will handle the registration for various CIMOMs.
  %{_datadir}/%{name}/provider-register.sh \
        $1 \
        -m %{SCHEMA} \
        -r %{REGISTRATION} > /dev/null 2>&1 || :;
  # don't let registration failure when server not running fail install!
}
 
/sbin/ldconfig
if [ $1 -ge 1 ]
then
        registered=no
        if [ -e /usr/sbin/cimserver ]; then
          register "-t pegasus";
          registered=yes
        fi
 
        if [ -e /usr/sbin/sfcbd ]; then
          register "-t sfcb";
          registered=yes
        fi
 
        if [ "$registered" != yes ]; then
          register
        fi
fi

%preun
function unregister()
{
  %{_datadir}/%{name}/provider-register.sh -d \
        $1 \
        -m %{SCHEMA} \
        -r %{REGISTRATION} > /dev/null 2>&1 || :;
  # don't let registration failure when server not running fail erase!
}
 
if [ $1 -eq 0 ]
then
        unregistered=no
        if [ -e /usr/sbin/cimserver ]; then
          unregister "-t pegasus";
          unregistered=yes
        fi
 
        if [ -e /usr/sbin/sfcbd ]; then
          unregister "-t sfcb";
          unregistered=yes
        fi
 
        if [ "$unregistered" != yes ]; then
          unregister
        fi
fi

%postun -p /sbin/ldconfig

%changelog
* Thu Mar 09 2017 Vitezslav Crhonek <vcrhonek@redhat.com> - 1.5.1-12
- Fix mounted filesystem is shown as disabled when device mapper is used
  Resolves: #1136116

* Wed Feb 26 2014 Vitezslav Crhonek <vcrhonek@redhat.com> - 1.5.1-11
- Fix mounted filesystem is shown as disabled when fstab entry uses link, UUID or LABEL
  Resolves: #921487

* Fri Jan 24 2014 Daniel Mach <dmach@redhat.com> - 1.5.1-10
- Mass rebuild 2014-01-24

* Fri Dec 27 2013 Daniel Mach <dmach@redhat.com> - 1.5.1-9
- Mass rebuild 2013-12-27

* Wed Aug 14 2013 Vitezslav Crhonek <vcrhonek@redhat.com> - 1.5.1-8
- Use Pegasus root/interop instead of root/PG_Interop

* Thu Feb 14 2013 Fedora Release Engineering <rel-eng@lists.fedoraproject.org> - 1.5.1-7
- Rebuilt for https://fedoraproject.org/wiki/Fedora_19_Mass_Rebuild

* Thu Nov 08 2012 Vitezslav Crhonek <vcrhonek@redhat.com> - 1.5.1-6
- Fix source URL

* Tue Sep 04 2012 Vitezslav Crhonek <vcrhonek@redhat.com> - 1.5.1-5
- Fix issues found by fedora-review utility in the spec file

* Sat Jul 21 2012 Fedora Release Engineering <rel-eng@lists.fedoraproject.org> - 1.5.1-4
- Rebuilt for https://fedoraproject.org/wiki/Fedora_18_Mass_Rebuild

* Sat Jan 14 2012 Fedora Release Engineering <rel-eng@lists.fedoraproject.org> - 1.5.1-3
- Rebuilt for https://fedoraproject.org/wiki/Fedora_17_Mass_Rebuild

* Thu Nov 10 2011 Vitezslav Crhonek <vcrhonek@redhat.com> - 1.5.1-2
- Add better support of mofs registration for various CIMOMs

* Wed May 25 2011 Vitezslav Crhonek <vcrhonek@redhat.com> - 1.5.1-1
- Update to sblim-cmpi-fsvol-1.5.1

* Wed Feb 09 2011 Fedora Release Engineering <rel-eng@lists.fedoraproject.org> - 1.5.0-3
- Rebuilt for https://fedoraproject.org/wiki/Fedora_15_Mass_Rebuild

* Tue Dec 21 2010 Vitezslav Crhonek <vcrhonek@redhat.com> - 1.5.0-2
- Add ext4 support (without update of testcase files)
- Fix mofs registration for various CIMOMs

* Thu Nov  4 2010 Vitezslav Crhonek <vcrhonek@redhat.com> - 1.5.0-1
- Update to sblim-cmpi-fsvol-1.5.0
- Remove CIMOM dependencies

* Wed Oct 14 2009 Vitezslav Crhonek <vcrhonek@redhat.com> - 1.4.4-1
- Initial support
