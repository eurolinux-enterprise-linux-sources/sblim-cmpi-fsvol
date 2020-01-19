/*
 * cmpiOSBase_NFS.c
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
 * This is the factory implementation for creating instances of CIM
 * class Linux_NFS.
 *
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mntent.h> 
#include <sys/stat.h> 
#include <sys/vfs.h>

#include <unistd.h>
#include <pthread.h>

#include "cmpidt.h"
#include "cmpimacs.h"

#include "cmpiOSBase_Common.h"
#include "OSBase_Common.h"
#include "OSBase_CommonFsvol.h"

#include "cmpiOSBase_NFS.h"
#include "OSBase_NFS.h"

/* ---------------------------------------------------------------------------*/
/* private declarations                                                       */


/* ---------------------------------------------------------------------------*/



/* ---------------------------------------------------------------------------*/
/*                            Factory functions                               */
/* ---------------------------------------------------------------------------*/

/* ---------- method to create a CMPIObjectPath of this class ----------------*/

CMPIObjectPath * _makePath_NFS( const CMPIBroker * _broker,
                 const CMPIContext * ctx, 
                 const CMPIObjectPath * ref,
		 const struct mntent * sptr,
		 CMPIStatus * rc) {
  CMPIObjectPath * op = NULL;
 
  _OSBASE_TRACE(2,("--- _makePath_NFS() called"));
  
  /* the sblim-cmpi-base package offers some tool methods to get common
   * system datas 
   * CIM_HOST_NAME contains the unique hostname of the local system 
  */ 
  if( !get_system_name() ) {   
    CMSetStatusWithChars( _broker, rc, 
			  CMPI_RC_ERR_FAILED, "no host name found" );
    _OSBASE_TRACE(2,("--- _makePath_NFS() failed : %s",CMGetCharPtr(rc->msg)));
    goto exit;
  }

  op = CMNewObjectPath( _broker, CMGetCharPtr(CMGetNameSpace(ref,rc)), 
			_ClassName, rc );
  if( CMIsNullObject(op) ) { 
    CMSetStatusWithChars( _broker, rc, 
			  CMPI_RC_ERR_FAILED, "Create CMPIObjectPath failed." ); 
    _OSBASE_TRACE(2,("--- _makePath_NFS() failed : %s",CMGetCharPtr(rc->msg)));
    goto exit; 
  }

  CMAddKey(op, "CSCreationClassName", CSCreationClassName, CMPI_chars);  
  CMAddKey(op, "CSName", get_system_name(), CMPI_chars);
  CMAddKey(op, "CreationClassName", _ClassName, CMPI_chars);  
  CMAddKey(op, "Name", sptr->mnt_fsname, CMPI_chars);
   
 exit:
  _OSBASE_TRACE(2,("--- _makePath_NFS() exited"));
  return op;                
}

/* ----------- method to create a CMPIInstance of this class ----------------*/

CMPIInstance * _makeInst_NFS( const CMPIBroker * _broker,
               const CMPIContext * ctx, 
               const CMPIObjectPath * ref,
	       const char ** properties,
	       const struct mntent * sptr,
	       CMPIStatus * rc) {
  CMPIObjectPath *   op       = NULL;
  CMPIInstance   *   ci       = NULL;
  struct statfs  *   fs       = NULL; 
  const char     **  keys     = NULL;
  int                keyCount = 0;
  unsigned char      ro       = 0;
  unsigned long long bsize    = 0;
  unsigned long long size     = 0;
  int                use      = 0;
#ifndef CIM26COMPAT
  int                mntstat  = 0;
  int                status   = 2; /* Enabled */
#endif

  _OSBASE_TRACE(2,("--- _makeInst_NFS() called"));

  /* the sblim-cmpi-base package offers some tool methods to get common
   * system datas 
   * CIM_HOST_NAME contains the unique hostname of the local system 
  */
  if( !get_system_name() ) {   
    CMSetStatusWithChars( _broker, rc, 
			  CMPI_RC_ERR_FAILED, "no host name found" );
    _OSBASE_TRACE(2,("--- _makeInst_NFS() failed : %s",CMGetCharPtr(rc->msg)));
    goto exit;
  }

  op = CMNewObjectPath( _broker, CMGetCharPtr(CMGetNameSpace(ref,rc)), 
			_ClassName, rc );
  if( CMIsNullObject(op) ) { 
    CMSetStatusWithChars( _broker, rc, 
			  CMPI_RC_ERR_FAILED, "Create CMPIObjectPath failed." ); 
    _OSBASE_TRACE(2,("--- _makeInst_NFS() failed : %s",CMGetCharPtr(rc->msg)));
    goto exit; 
  }

  ci = CMNewInstance( _broker, op, rc);
  if( CMIsNullObject(ci) ) { 
    CMSetStatusWithChars( _broker, rc, 
			  CMPI_RC_ERR_FAILED, "Create CMPIInstance failed." ); 
    _OSBASE_TRACE(2,("--- _makeInst_NFS() failed : %s",CMGetCharPtr(rc->msg)));
    goto exit; 
  }

  /* set property filter */
  keys = calloc(5,sizeof(char*));
  keys[0] = strdup("CSCreationClassName");
  keys[1] = strdup("CSName");
  keys[2] = strdup("CreationClassName");
  keys[3] = strdup("Name");
  CMSetPropertyFilter(ci,properties,keys);
  for( ;keys[keyCount]!=NULL;keyCount++) { free((char*)keys[keyCount]); }
  free(keys);

  CMSetProperty( ci, "CSCreationClassName", CSCreationClassName, CMPI_chars ); 
  CMSetProperty( ci, "CSName", get_system_name(), CMPI_chars );

  CMSetProperty( ci, "CreationClassName", _ClassName, CMPI_chars ); 
  CMSetProperty( ci, "Name", sptr->mnt_fsname, CMPI_chars);

  CMSetProperty( ci, "Status", "OK", CMPI_chars);
  CMSetProperty( ci, "Caption", "NFS File System", CMPI_chars);
  CMSetProperty( ci, "Description", "The class represents the nfs mounts of the system.", CMPI_chars);

  CMSetProperty( ci, "Root", sptr->mnt_dir, CMPI_chars);
  CMSetProperty( ci, "FileSystemType", sptr->mnt_type, CMPI_chars);

  if( hasmntopt( sptr, "ro") != NULL ) { ro = 1; }
  CMSetProperty( ci, "ReadOnly", (CMPIValue*)&(ro), CMPI_boolean );
 
  // structure to get statistical informations about the filesystem
  fs = (struct statfs *) malloc (sizeof (struct statfs));
  memset(fs, 0, sizeof (struct statfs) );
  if (statfs( sptr->mnt_dir, fs) == 0) {
    bsize = (long long)(fs->f_bsize);
    CMSetProperty( ci, "BlockSize", (CMPIValue*)&(bsize), CMPI_uint64 );
    size = ((long long)fs->f_blocks)*((long long)fs->f_bsize);
    CMSetProperty( ci, "FileSystemSize", (CMPIValue*)&(size), CMPI_uint64 );
    size =((long long)fs->f_bavail)*((long long)fs->f_bsize);
    CMSetProperty( ci, "AvailableSpace", (CMPIValue*)&(size), CMPI_uint64 );
    
    CMSetProperty( ci, "MaxFileNameLength", (CMPIValue*)&(fs->f_namelen), CMPI_uint32 );

    if (fs->f_blocks > 0) {
      use = ((fs->f_blocks-fs->f_bfree)*100) / fs->f_blocks;
    } else {
      use = 100;
    }
    CMSetProperty( ci, "PercentageSpaceUse", (CMPIValue*)&(use), CMPI_uint8 );

    if(fs) free(fs);
  }

  /* 2.7 */ 
#ifndef CIM26COMPAT
  CMSetProperty( ci, "ElementName", get_system_name(), CMPI_chars);

  mntstat = fs_mount_status(sptr->mnt_fsname);
  if( mntstat == 0 ) { status = 3; /* Disabled */ } 
  CMSetProperty( ci, "EnabledState", (CMPIValue*)&(status), CMPI_uint16);
  CMSetProperty( ci, "OtherEnabledState", "NULL", CMPI_chars);
  CMSetProperty( ci, "RequestedState", (CMPIValue*)&(status), CMPI_uint16);

  mntstat =   fs_default_mount_status(sptr->mnt_fsname);
  if( mntstat == 0 ) { status = 3; /* Disabled */ } 
  if( mntstat == 1 ) { status = 2; /* Enabled */ } 
  CMSetProperty( ci, "EnabledDefault", (CMPIValue*)&(status), CMPI_uint16);
#endif

 exit:
  _OSBASE_TRACE(2,("--- _makeInst_NFS() exited"));
  return ci;
}



/* ---------------------------------------------------------------------------*/
/*                         end of cmpiOSBase_NFS.c                            */
/* ---------------------------------------------------------------------------*/

