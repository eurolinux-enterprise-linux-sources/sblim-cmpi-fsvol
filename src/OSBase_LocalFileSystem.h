#ifndef _OSBASE_LOCALFILESYSTEM_H_
#define _OSBASE_LOCALFILESYSTEM_H_

/*
 * OSBase_LocalFileSystem.h
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
 * Description: 
 * This file defines the interfaces for the resource access implementation 
 * of the CIM class CIM_UnixLocalFileSystem.
 * 
*/

/* ---------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/* ---------------------------------------------------------------------------*/

#include <mntent.h> 

/* ---------------------------------------------------------------------------*/

/* reads all entries out of /etc/fstab and /etc/mtab (/proc/mounts)
 * without duplicates */
int enum_all_localfs ( struct mntlist ** lptr );


/* ---------------------------------------------------------------------------*/

#ifdef __cplusplus
   }
#endif

/* ---------------------------------------------------------------------------*/

#endif


