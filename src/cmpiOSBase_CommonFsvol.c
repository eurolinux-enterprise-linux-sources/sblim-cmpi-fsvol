/*
 * cmpiOSBase_CommonFsvol.c
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
 * class -.
 *
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mntent.h> 
#include <sys/stat.h> 
#include <sys/vfs.h>

#include "cmpidt.h"
#include "cmpimacs.h"

#include "cmpiOSBase_Common.h"
#include "OSBase_Common.h"

#include "cmpiOSBase_CommonFsvol.h"
#include "OSBase_CommonFsvol.h"

/* ---------------------------------------------------------------------------*/
/* private declarations                                                       */


/* ---------------------------------------------------------------------------*/


/* 
 * get_fs_of_dir offers the information to which FileSystem ( property values
 * 'CreationClassName' and 'DeviceID' ) the directory belongs
 *
 * parameter description :
 * dir     -> contains the full qualified path of the directory
 * 
 * the important return values of this function are 'fscname' and 'fsname'
 * fscname -> conatins the 'CreationClassName' of the corresponding FileSystem , 
 *            e.g. 'Linux_LocalFileSystem'
 * fsname  -> contains the value of the property 'DeviceID' of the corresponding
 *            FileSystem , e.g. '/dev/hda6'
 */

int get_fs_of_dir( const char * dir , char ** fscname , char ** fsname ) {
  struct mntlist * ml   = NULL ;
  struct mntlist * del  = NULL ;
  struct mntent  * me   = NULL ;
  char           * fsc  = NULL ;
  char           * fs   = NULL ;
  char           * cdir = NULL ;
  int              rc   = 0 ;

  _OSBASE_TRACE(4,("--- _get_fs_of_dir() called"));

  // get all local filesystems
  rc = enum_all_fs( &ml );
  if(rc > 0) { return rc; }

  /*
   * ToDo : check if directory does exist !!!
   */

  del = ml;
  // iteration through list of local filesystems
  if( ml->me ) {
    for ( ; ml ; ml = ml->next) {
      me = ml->me ;
      
      if( ( strcmp( dir , me->mnt_dir ) == 0 ) &&
	  ( strcmp( dir , "/" )         == 0 ) ) {
	if(cdir) {
	  free(cdir);
	  cdir = NULL ;
	}
	cdir = strdup(me->mnt_dir);
	break;
      }   
      if( ( strcmp( dir , me->mnt_dir ) != 0 ) &&
	  ( strncmp( dir , me->mnt_dir , strlen(me->mnt_dir) ) == 0 ) ) {
	if( cdir ) {
	  if( strlen(cdir) < strlen(me->mnt_dir) ) { free(cdir); }
	  cdir = NULL ;
	}
	if( !cdir ) { cdir = strdup(me->mnt_dir); }
      }
    }  
  }
  
  ml = del ;
  if( ml->me && cdir ) {
    for ( ; ml ; ml = ml->next) {
      me = ml->me ;
      if( strcmp( cdir , me->mnt_dir ) == 0 ) {
	
	fs = (char*) malloc( strlen(me->mnt_fsname)+1 ) ;
	memset( fs , 0 , strlen(me->mnt_fsname)+1 );
	strcpy( fs , me->mnt_fsname);
	*fsname = fs ;
	
	if( strcmp( me->mnt_type, "nfs") == 0 ) {
	    fsc = calloc(1,10);
	    strcpy( fsc , "Linux_NFS");
	    *fscname = fsc ;
	}
	if( strcmp( me->mnt_type, "ext2") == 0 ) {
	    fsc = calloc(1,21);
	    strcpy( fsc , "Linux_Ext2FileSystem");
	    *fscname = fsc ;
	}
	if( strcmp( me->mnt_type, "ext3") == 0 ) {
	    fsc = calloc(1,21);
	    strcpy( fsc , "Linux_Ext3FileSystem");
	    *fscname = fsc ;
	}
	if( strcmp( me->mnt_type, "reiserfs") == 0 ) {  
	    fsc = calloc(1,23);
	    strcpy( fsc , "Linux_ReiserFileSystem");
	    *fscname = fsc ;
	}
	break;
      }
      me = NULL ;
    }
  }
  
  /* garbagge collecion */
  if(cdir) free(cdir);
  if(del) free_mntlist(del);
  
  _OSBASE_TRACE(4,("--- _get_fs_of_dir() exited : %s : %s",*fscname,*fsname));
  return rc ;
}


/* ---------------------------------------------------------------------------*/
/*                     end of cmpiOSBase_CommonFsvol.c                        */
/* ---------------------------------------------------------------------------*/

