#ifndef _CMPIOSBASE_BOOTOSFROMFS_H_
#define _CMPIOSBASE_BOOTOSFROMFS_H_

/*
 * cmpiOSBase_BootOSFromFS.h
 *
 * (C) Copyright IBM Corp. 2002, 2009
 *
 * THIS FILE IS PROVIDED UNDER THE TERMS OF THE ECLIPSE PUBLIC LICENSE
 * ("AGREEMENT"). ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS FILE
 * CONSTITUTES RECIPIENTS ACCEPTANCE OF THE AGREEMENT.
 *
 * You can obtain a current copy of the Eclipse Public License from
 * http://www.opensource.org/licenses/eclipse-1.0.php
 *
 * Author:       Heidi Neumann <heidineu@de.ibm.com>
 * Contributors:
 *
 * Interface Type : Common Magabeablity Programming Interface ( CMPI )
 *
 * Description:
 * This file defines the interfaces for the factory implementation of the
 * CIM class Linux_BootOSFromFS.
 *
*/


/* ---------------------------------------------------------------------------*/

#include "cmpidt.h"

/* ---------------------------------------------------------------------------*/

static char * _ClassName          = "Linux_BootOSFromFS";

static char * _RefLeft            = "Antecedent";
static char * _RefRight           = "Dependent";
static char * _RefLeftClass       = "CIM_UnixLocalFileSystem";
static char * _RefLeftClass_bk    = "CIM_UnixLocalFileSystem";
static char * _RefLeftClassExt2   = "Linux_Ext2FileSystem";
static char * _RefLeftClassExt3   = "Linux_Ext3FileSystem";
static char * _RefLeftClassReiser = "Linux_ReiserFileSystem";
static char * _RefRightClass      = "Linux_OperatingSystem";

/* ---------------------------------------------------------------------------*/

/* method to create a CMPIObjectPath of this class                            */


int _assoc_create_inst_BootOSFromFS( const CMPIBroker * _broker, 
				     const CMPIContext * ctx,
				     const CMPIResult * rslt,
				     const CMPIObjectPath * cop,
				     int inst,
				     CMPIStatus * rc);

int _get_boot_fs( char ** root_fsclass, char ** root_fsname,
		  char ** boot_fsclass, char ** boot_fsname );

CMPIObjectPath * _assoc_get_boot_fs_OP( const CMPIBroker * _broker, 
					const CMPIObjectPath * ref, 
					const char * fs_class, 
					const char * fs_name ,
					CMPIStatus * rc);

CMPIObjectPath * _assoc_get_assoc_OP_BootOSFromFS( const CMPIBroker * _broker, 
						   const CMPIObjectPath * ref_left,
						   const CMPIObjectPath * ref_right,
						   CMPIStatus * rc);

CMPIInstance * _assoc_get_assoc_INST_BootOSFromFS( const CMPIBroker * _broker,
						   const CMPIObjectPath * ref_left,
						   const CMPIObjectPath * ref_right,
						   CMPIStatus * rc);

int _assoc_BootOSFromFS( const CMPIBroker * _broker, 
			 const CMPIContext * ctx,
			 const CMPIResult * rslt,
			 const CMPIObjectPath * ref, 
			 int inst,
			 int associators,
			 CMPIStatus * rc);


/* ---------------------------------------------------------------------------*/

#endif

