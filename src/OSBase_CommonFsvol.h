#ifndef _OSBASE_COMMONFSVOL_H_
#define _OSBASE_COMMONFSVOL_H_

/*
 * OSBase_CommonFsvol.h
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
 * of the CIM class -.
 * 
*/

/* ---------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/* ---------------------------------------------------------------------------*/

#define PROC_MOUNTS     "/proc/mounts"
#define ETC_MTAB        "/etc/mtab"
#define ETC_FSTAB       "/etc/fstab"
#define PROC_DEVICES    "/proc/devices"
#define PROC_PARTITIONS "/proc/partitions" 

/* ---------------------------------------------------------------------------*/

struct mntlist {
  struct mntent  * me;
  struct mntlist * next;
};

int enum_all_fs ( struct mntlist ** lptr );

int get_fs_data( struct mntent ** sptr, char * name );
struct mntent * _cpy_fs_data( struct mntent * );

void free_mntlist( struct mntlist * lptr );
void free_mntent( struct mntent * sptr );


/* ---------------------------------------------------------------------------*/
// get mount status

unsigned char fs_mount_status( char * fsname );
unsigned char fs_default_mount_status( char * fsname );


/* ---------------------------------------------------------------------------*/

#ifdef __cplusplus
   }
#endif

/* ---------------------------------------------------------------------------*/

#endif


