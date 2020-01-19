#ifndef _CMPIOSBASE_LOCALFILESYSTEM_H_
#define _CMPIOSBASE_LOCALFILESYSTEM_H_

/*
 * cmpiOSBase_LocalFileSystem.h
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
 * CIM class CIM_UnixLocalFileSystem.
 *
*/


/* ---------------------------------------------------------------------------*/

#include "cmpidt.h"
#include "OSBase_LocalFileSystem.h"

/* ---------------------------------------------------------------------------*/

static char * _ClassName   = "CIM_UnixLocalFileSystem";
static char * _ClassExt2   = "Linux_Ext2FileSystem";
static char * _ClassExt3   = "Linux_Ext3FileSystem";
static char * _ClassReiser = "Linux_ReiserFileSystem";

/* ---------------------------------------------------------------------------*/


/* method to create a CMPIObjectPath of this class                            */

CMPIObjectPath * _makePath_LocalFileSystem( const CMPIBroker * _broker,
                 const CMPIContext * ctx, 
                 const CMPIObjectPath * cop,
	         const struct mntent * sptr,
	         int scope,
                 CMPIStatus * rc);


/* method to create a CMPIInstance of this class                              */

CMPIInstance * _makeInst_LocalFileSystem( const CMPIBroker * _broker,
               const CMPIContext * ctx, 
               const CMPIObjectPath * cop,
	       const char ** properties,
	       const struct mntent * sptr,
	       int scope,
               CMPIStatus * rc);

void _get_class_scope( const CMPIObjectPath * ref, 
		       int * scope,
		       CMPIStatus * rc );

int _valid_class_and_fstype_combination( int scope,
					 const char * type);

/* ---------------------------------------------------------------------------*/

#endif

