# $Id: Makefile.am,v 1.7 2009/06/02 19:42:24 tyreld Exp $
# ==================================================================
# (C) Copyright IBM Corp. 2005, 2009
#
# THIS FILE IS PROVIDED UNDER THE TERMS OF THE ECLIPSE PUBLIC LICENSE
# ("AGREEMENT"). ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS FILE
# CONSTITUTES RECIPIENTS ACCEPTANCE OF THE AGREEMENT.
#
# You can obtain a current copy of the Eclipse Public License from
# http://www.opensource.org/licenses/eclipse-1.0.php
#
# Author:       Viktor Mihajlovski <mihajlov@de.ibm.com>
# Contributors: Dr. Gareth S. Bestor <bestorga@us.ibm.com>
# Description:  Automake input file for OS Base Providers
# ==================================================================


# SBLIM Include Directory
sblimincdir = $(includedir)/sblim

# Start from an empty extra distribution file list
EXTRA_DIST=

# docdir defines where the documentation goes
docdir=$(datadir)/doc/$(PACKAGE)-$(VERSION)

#
# Automake instructions for documentation
#
doc_DATA=README AUTHORS COPYING 

# ADD EXTRA DOC FILES IF PRESENT
doc_DATA+=DEBUG README.tog-pegasus
EXTRA_DIST+=DEBUG README.tog-pegasus

# providerdir defines where provider libraries will be installed (@PROVIDERDIR@ is set by the configure script). 
providerdir = @PROVIDERDIR@

#
# Automake instructions for main dir
#

#  OSBase CMPI provider libraries
provider_LTLIBRARIES = 	libcmpiOSBase_LocalFileSystemProvider.la \
			libcmpiOSBase_NFSProvider.la \
			libcmpiOSBase_BlockStorageStatisticalDataProvider.la \
			libcmpiOSBase_HostedFileSystemProvider.la \
			libcmpiOSBase_BootOSFromFSProvider.la 

# Local Filesystem
libcmpiOSBase_LocalFileSystemProvider_la_SOURCES = \
		cmpiOSBase_LocalFileSystemProvider.c \
		cmpiOSBase_LocalFileSystem.c \
		OSBase_LocalFileSystem.c
libcmpiOSBase_LocalFileSystemProvider_la_LIBADD = \
		-lcmpiOSBase_CommonFsvol 
libcmpiOSBase_LocalFileSystemProvider_la_LDFLAGS = -avoid-version

# Network Filesystem
libcmpiOSBase_NFSProvider_la_SOURCES = \
		cmpiOSBase_NFSProvider.c \
		cmpiOSBase_NFS.c \
		OSBase_NFS.c
libcmpiOSBase_NFSProvider_la_LIBADD = \
		-lcmpiOSBase_CommonFsvol
libcmpiOSBase_NFSProvider_la_LDFLAGS = -avoid-version

# Block Storage Statistical Data
libcmpiOSBase_BlockStorageStatisticalDataProvider_la_SOURCES = \
		cmpiOSBase_BlockStorageStatisticalDataProvider.c \
		cmpiOSBase_BlockStorageStatisticalData.c \
		OSBase_BlockStorageStatisticalData.c
libcmpiOSBase_BlockStorageStatisticalDataProvider_la_LIBADD = \
		-lcmpiOSBase_CommonFsvol
libcmpiOSBase_BlockStorageStatisticalDataProvider_la_LDFLAGS = -avoid-version

# Hosted Filesystem (Association)
libcmpiOSBase_HostedFileSystemProvider_la_SOURCES = \
		cmpiOSBase_HostedFileSystemProvider.c
libcmpiOSBase_HostedFileSystemProvider_la_LIBADD = \
		-lcmpiOSBase_CommonFsvol
libcmpiOSBase_HostedFileSystemProvider_la_LDFLAGS = -avoid-version

# Boot OS from Filesystem (Association)
libcmpiOSBase_BootOSFromFSProvider_la_SOURCES = \
		cmpiOSBase_BootOSFromFSProvider.c \
		cmpiOSBase_BootOSFromFS.c 
libcmpiOSBase_BootOSFromFSProvider_la_LIBADD = \
		-lcmpiOSBase_CommonFsvol
libcmpiOSBase_BootOSFromFSProvider_la_LDFLAGS = -avoid-version

# OSBase support utility libraries
lib_LTLIBRARIES = libcmpiOSBase_CommonFsvol.la

libcmpiOSBase_CommonFsvol_la_SOURCES=OSBase_CommonFsvol.c \
		cmpiOSBase_CommonFsvol.c
libcmpiOSBase_CommonFsvol_la_LIBADD=-lcmpiOSBase_Common

# Installable Header Files
sbliminc_HEADERS = OSBase_CommonFsvol.h cmpiOSBase_CommonFsvol.h

# Non-Installable Header Files
noinst_HEADERS = OSBase_LocalFileSystem.h \
	OSBase_NFS.h \
	OSBase_BlockStorageStatisticalData.h \
	cmpiOSBase_LocalFileSystem.h \
	cmpiOSBase_NFS.h \
	cmpiOSBase_BlockStorageStatisticalData.h \
	cmpiOSBase_BootOSFromFS.h

# We must explicity add the RPM spec file to the distribution package
EXTRA_DIST+=$(PACKAGE).spec $(PACKAGE).rh.spec

#
# Automake instructions for ./mof subdir
#
SCHEMAS=mof/Linux_Fsvol.mof 
REGISTRATIONS=mof/Linux_Fsvol.registration

# We must explicity add all the schema files to the distribution package
pkgdata_DATA=$(SCHEMAS) $(REGISTRATIONS)
pkgdata_SCRIPTS=provider-register.sh
EXTRA_DIST+=mof $(pkgdata_SCRIPTS)

# Register the provider(s) and class definition(s) to the current CIM server/CIMOM
postinstall:
	sh provider-register.sh -t @CIMSERVER@ -r $(REGISTRATIONS) -m $(SCHEMAS)

preuninstall:
	sh provider-register.sh -d -t @CIMSERVER@ -r $(REGISTRATIONS) -m $(SCHEMAS)

dist-hook:
	test -d "$(distdir)" &&	rm -rf `find $(distdir) -type d -name CVS`
#
# Automake instructions for ./test subdir
#
EXTRA_DIST+=test README.TEST 
if TESTSUITE
testsuitedir=@TESTSUITEDIR@
testsuitesystemdir=@TESTSUITEDIR@/system/linux
testsuitecimdir=@TESTSUITEDIR@/cim

testsuite_SCRIPTS=test/test-cmpi-fsvol.sh
testsuitesystem_DATA=test/system/linux/Linux_Ext2FileSystem.system \
	test/system/linux/Linux_Ext3FileSystem.system \
	test/system/linux/Linux_HostedFileSystem.system \
	test/system/linux/Linux_ReiserFileSystem.system
testsuitesystem_SCRIPTS=test/system/linux/Linux_Ext2FileSystem.pl \
	test/system/linux/Linux_Ext3FileSystem.pl \
	test/system/linux/Linux_ReiserFileSystem.pl \
	test/system/linux/countFileSystemEntries.pl
testsuitecim_DATA=test/cim/Linux_BootOSFromFS.cim \
	test/cim/Linux_Ext2FileSystem.cim \
	test/cim/Linux_Ext3FileSystem.cim \
	test/cim/Linux_HostedFileSystem.cim \
	test/cim/Linux_NFS.cim \
	test/cim/Linux_BlockStorageStatisticalData.cim \
	test/cim/Linux_ReiserFileSystem.cim 

runtest: install
	cd $(DESTDIR)$(TESTSUITEDIR) &&	./test-cmpi-fsvol.sh

doc_DATA+=README.TEST
endif
