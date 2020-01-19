/*
 * cmpiOSBase_LocalFileSystem.c
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
 * class CIM_UnixLocalFileSystem.
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

#include "cmpiOSBase_LocalFileSystem.h"
#include "OSBase_LocalFileSystem.h"

/* ---------------------------------------------------------------------------*/
/* private declarations                                                       */


/* ---------------------------------------------------------------------------*/



/* ---------------------------------------------------------------------------*/
/*                            Factory functions                               */
/* ---------------------------------------------------------------------------*/

/* ---------- method to create a CMPIObjectPath of this class ----------------*/

CMPIObjectPath * _makePath_LocalFileSystem( const CMPIBroker * _broker,
                 const CMPIContext * ctx, 
                 const CMPIObjectPath * ref, 
		 const struct mntent * sptr,
		 int scope,
		 CMPIStatus * rc) {
  CMPIObjectPath * op = NULL;
  CMPIInstance   * ci = NULL;

  _OSBASE_TRACE(2,("--- _makePath_LocalFileSystem() called"));

  ci = _makeInst_LocalFileSystem( _broker, ctx, ref, NULL, sptr, scope, rc);
  if( ci != NULL ) {
    op = CMGetObjectPath(ci,rc); 
    CMSetNameSpace(op,CMGetCharPtr(CMGetNameSpace(ref,rc)));
  }
  
  _OSBASE_TRACE(2,("--- _makePath_LocalFileSystem() exited"));
  return op;       
}

/* ----------- method to create a CMPIInstance of this class ----------------*/

CMPIInstance * _makeInst_LocalFileSystem( const CMPIBroker * _broker,
               const CMPIContext * ctx, 
               const CMPIObjectPath * ref, 
	       const char ** properties,
	       const struct mntent * sptr,
	       int scope,
	       CMPIStatus * rc) {
  CMPIObjectPath *   op       = NULL;
  CMPIInstance   *   ci       = NULL;
  struct statfs  *   fs       = NULL; 
  const char     **  keys     = NULL;
  int                keyCount = 0;
  unsigned char      ro       = 0;
  unsigned long long bsize    = 0;
  unsigned long long size     = 0;
  unsigned char      use      = 0;
#ifndef CIM26COMPAT
  unsigned short     mntstat  = 0;
  unsigned short     status   = 2; /* Enabled */
#endif

  _OSBASE_TRACE(2,("--- _makeInst_LocalFileSystem() called"));

  /* the sblim-cmpi-base package offers some tool methods to get common
   * system datas 
   * CIM_HOST_NAME contains the unique hostname of the local system 
  */
  if( !get_system_name() ) {   
    CMSetStatusWithChars( _broker, rc, 
			  CMPI_RC_ERR_FAILED, "no host name found" );
    _OSBASE_TRACE(2,("--- _makeInst_LocalFileSystem() failed : %s",CMGetCharPtr(rc->msg)));
    goto exit;
  }

  /* Linux_Ext2FileSystem */
  if( scope == 1 || ( scope==0 && strcmp(sptr->mnt_type,"ext2")==0) ) {
    op = CMNewObjectPath( _broker, CMGetCharPtr(CMGetNameSpace(ref,rc)), 
			_ClassExt2, rc );
    if( CMIsNullObject(op) ) { 
      CMSetStatusWithChars( _broker, rc, 
			    CMPI_RC_ERR_FAILED, "Create CMPIObjectPath failed." ); 
      _OSBASE_TRACE(2,("--- _makeInst_LocalFileSystem() failed : %s",CMGetCharPtr(rc->msg)));
      goto exit; 
    }
    /*
    ci = CMNewInstance( _broker, op, rc);
    if( CMIsNullObject(ci) ) { 
      CMSetStatusWithChars( _broker, rc, 
			    CMPI_RC_ERR_FAILED, "Create CMPIInstance failed." ); 
      _OSBASE_TRACE(2,("--- _makeInst_LocalFileSystem() failed : %s",CMGetCharPtr(rc->msg)));
      goto exit; 
    }
    
    CMSetProperty( ci, "CreationClassName", _ClassExt2, CMPI_chars ); 
    CMSetProperty( ci, "Caption", "Ext2 local file system", CMPI_chars);
    */
    
  } /* end of scope "ext2" */
  

  /* Linux_Ext3FileSystem */
  if( scope == 2 || ( scope==0 && strcmp(sptr->mnt_type,"ext3")==0) ) {
    op = CMNewObjectPath( _broker, CMGetCharPtr(CMGetNameSpace(ref,rc)), 
			_ClassExt3, rc );
    if( CMIsNullObject(op) ) { 
      CMSetStatusWithChars( _broker, rc, 
			    CMPI_RC_ERR_FAILED, "Create CMPIObjectPath failed." ); 
      _OSBASE_TRACE(2,("--- _makeInst_LocalFileSystem() failed : %s",CMGetCharPtr(rc->msg)));
      goto exit; 
    }
    /*
    ci = CMNewInstance( _broker, op, rc);
    if( CMIsNullObject(ci) ) { 
      CMSetStatusWithChars( _broker, rc, 
			    CMPI_RC_ERR_FAILED, "Create CMPIInstance failed." ); 
      _OSBASE_TRACE(2,("--- _makeInst_LocalFileSystem() failed : %s",CMGetCharPtr(rc->msg)));
      goto exit; 
    }
    
    CMSetProperty( ci, "CreationClassName", _ClassExt3, CMPI_chars ); 
    CMSetProperty( ci, "Caption", "Ext3 local file system", CMPI_chars);
    */
    
  } /* end of scope "ext3" */

  /* Linux_ReiserFileSystem */
  if( scope == 3 || ( scope==0 && strcmp(sptr->mnt_type,"reiserfs")==0) ) {
    op = CMNewObjectPath( _broker, CMGetCharPtr(CMGetNameSpace(ref,rc)),
			  _ClassReiser, rc );
    if( CMIsNullObject(op) ) { 
      CMSetStatusWithChars( _broker, rc, 
			    CMPI_RC_ERR_FAILED, "Create CMPIObjectPath failed." ); 
      _OSBASE_TRACE(2,("--- _makeInst_LocalFileSystem() failed : %s",CMGetCharPtr(rc->msg)));
      goto exit; 
    }
    /*
    ci = CMNewInstance( _broker, op, rc);
    if( CMIsNullObject(ci) ) { 
      CMSetStatusWithChars( _broker, rc, 
			    CMPI_RC_ERR_FAILED, "Create CMPIInstance failed." ); 
      _OSBASE_TRACE(2,("--- _makeInst_LocalFileSystem() failed : %s",CMGetCharPtr(rc->msg)));
      goto exit; 
    }
    
    CMSetProperty( ci, "CreationClassName", _ClassReiser, CMPI_chars ); 
    CMSetProperty( ci, "Caption", "Reiser local file system", CMPI_chars);
    */
    
  } /* end of scope "reiserfs" */


  ci = CMNewInstance( _broker, op, rc);
  if( CMIsNullObject(ci) ) { 
    CMSetStatusWithChars( _broker, rc, 
			  CMPI_RC_ERR_FAILED, "Create CMPIInstance failed." ); 
    _OSBASE_TRACE(2,("--- _makeInst_LocalFileSystem() failed : %s",CMGetCharPtr(rc->msg)));
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

  /* Linux_Ext2FileSystem */
  if( scope == 1 || ( scope==0 && strcmp(sptr->mnt_type,"ext2")==0) ) {
    CMSetProperty( ci, "CreationClassName", _ClassExt2, CMPI_chars ); 
    CMSetProperty( ci, "Caption", "Ext2 local file system", CMPI_chars);
  }

  /* Linux_Ext3FileSystem */
  if( scope == 2 || ( scope==0 && strcmp(sptr->mnt_type,"ext3")==0) ) {
    CMSetProperty( ci, "CreationClassName", _ClassExt3, CMPI_chars ); 
    CMSetProperty( ci, "Caption", "Ext3 local file system", CMPI_chars);
  }

  /* Linux_ReiserFileSystem */
  if( scope == 3 || ( scope==0 && strcmp(sptr->mnt_type,"reiserfs")==0) ) {
    CMSetProperty( ci, "CreationClassName", _ClassReiser, CMPI_chars ); 
    CMSetProperty( ci, "Caption", "Reiser local file system", CMPI_chars);
  }

  CMSetProperty( ci, "CSCreationClassName", CSCreationClassName, CMPI_chars ); 
  CMSetProperty( ci, "CSName", get_system_name(), CMPI_chars );
  CMSetProperty( ci, "Name", sptr->mnt_fsname, CMPI_chars );
  CMSetProperty( ci, "Root", sptr->mnt_dir, CMPI_chars );
  CMSetProperty( ci, "FileSystemType", sptr->mnt_type, CMPI_chars );
  CMSetProperty( ci, "Status", "OK", CMPI_chars);
  CMSetProperty( ci, "Description", "Represents the file store controlled by a ComputerSystem through local means.", CMPI_chars);

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

    use = ((fs->f_blocks-fs->f_bfree)*100) / fs->f_blocks;
    CMSetProperty( ci, "PercentageSpaceUse", (CMPIValue*)&(use), CMPI_uint8 );

    if(fs) free(fs);
  }
  
  /* 2.7 */ 
#ifndef CIM26COMPAT
  CMSetProperty( ci, "ElementName", sptr->mnt_dir, CMPI_chars);

  mntstat = fs_mount_status(sptr->mnt_fsname);
  if( mntstat == 0 ) { status = 3; /* Disabled */ } 
  CMSetProperty( ci, "EnabledState", (CMPIValue*)&(status), CMPI_uint16);
  CMSetProperty( ci, "OtherEnabledState", "NULL", CMPI_chars);
  CMSetProperty( ci, "RequestedState", (CMPIValue*)&(status), CMPI_uint16);

  mntstat = fs_default_mount_status(sptr->mnt_fsname);
  if( mntstat == 0 ) { status = 3; /* Disabled */ } 
  if( mntstat == 1 ) { status = 2; /* Enabled */ } 
  CMSetProperty( ci, "EnabledDefault", (CMPIValue*)&(status), CMPI_uint16);
#endif  

 exit:
  _OSBASE_TRACE(2,("--- _makeInst_LocalFileSystem() exited"));
  return ci;
}


/* ---------------------------------------------------------------------------*/

void _get_class_scope( const CMPIObjectPath * ref, 
		       int * scope,
		       CMPIStatus * rc ) {
  CMPIString * className = NULL;
  
  /* scope vaules :
   * 0 = global
   * 1 = ext2
   * 2 = ext3
   * 3 = reiserfs
   */

  className = CMGetClassName(ref, rc);

  if( strcasecmp(CMGetCharPtr(className), _ClassExt2) == 0 )
    *scope = 1;
  else if( strcasecmp(CMGetCharPtr(className), _ClassExt3) == 0 )
    *scope = 2;
  else if( strcasecmp(CMGetCharPtr(className), _ClassReiser) == 0 )
    *scope = 3;
  else 
    *scope = 0;
}


/* ---------------------------------------------------------------------------*/

int _valid_class_and_fstype_combination( int scope ,
					 const char * type ) {
  /* 0 ... not valid 
   * 1 ... valid 
   */
  if(scope == 0) return 1;

  if( scope==1 && strcmp(type,"ext2")==0 )
    return 1;
  if( scope==2 && strcmp(type,"ext3")==0 )
    return 1;
  
  if( scope==3 && strcmp(type,"reiserfs")==0 )
    return 1;
  
  return 0;
}


/* ---------------------------------------------------------------------------*/
/*                   end of cmpiOSBase_LocalFileSystem.c                      */
/* ---------------------------------------------------------------------------*/

