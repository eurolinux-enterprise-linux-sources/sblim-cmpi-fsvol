/*
 * OSBase_LocalFileSystem.c
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
 * This shared library provides resource access functionality for the class
 * CIM_UnixLocalFileSystem. 
 * It is independent from any specific CIM technology.
*/

/* ---------------------------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mntent.h> 
#include <sys/stat.h> 
#include <sys/vfs.h>

#include <unistd.h>
#include <pthread.h>

#include "OSBase_Common.h"
#include "OSBase_CommonFsvol.h"
#include "OSBase_LocalFileSystem.h"
#include <unistd.h>

/* ---------------------------------------------------------------------------*/
// private declarations


/* ---------------------------------------------------------------------------*/


int enum_all_localfs( struct mntlist ** ls ) {
  struct mntent    mebuf;
  char             mntbuf[4000];
  struct mntent  * me  = NULL ;
  struct mntlist * mel = NULL ;
  struct mntlist * ml  = NULL ;
  FILE           * tab = NULL ;
  int              add = 0 ;

  _OSBASE_TRACE(2,("--- enum_all_localfs() called"));
  
  mel = calloc (1, sizeof (struct mntlist) );
  *ls = mel;
  
  tab = setmntent( ETC_FSTAB ,"r");
  if ( tab == NULL ) { return -1; }
  
  while ( ( me = getmntent_r(tab,&mebuf,mntbuf,sizeof(mntbuf))) != NULL ) {
    if (  (strcmp("ext2",me->mnt_type) == 0)     || 
	  (strcmp("ext3",me->mnt_type) == 0)     ||    
	  (strcmp("reiserfs",me->mnt_type) == 0) ) {
      
      if (mel->me != NULL ) {
	mel->next = calloc (1, sizeof (struct mntlist) );
	mel = mel->next;    
      }
      if( strncmp(me->mnt_fsname, "LABEL=",6)!=0 )
	mel->me = _cpy_fs_data( me );
    }
  }
  endmntent(tab); 

  /* search for additional entries from mtab */
  tab = setmntent( PROC_MOUNTS ,"r");
  if ( tab == NULL ) 
    tab = setmntent( ETC_MTAB ,"r");
  if ( tab == NULL ) { return -1; }
  
  while ( ( me = getmntent_r(tab,&mebuf,mntbuf,sizeof(mntbuf))) != NULL ) {
    if (  (strcmp("ext2",me->mnt_type) == 0)     || 
	  (strcmp("ext3",me->mnt_type) == 0)     ||    
	  (strcmp("reiserfs",me->mnt_type) == 0) ) {
      
      ml = *ls;
      add = 1;
      if( ml->me ) {
	for( ; ml ; ml=ml->next ) {
	  if( (ml->me != NULL) &&
	    ((strcmp(ml->me->mnt_fsname, me->mnt_fsname)==0) ||
	     (strcmp(ml->me->mnt_dir, me->mnt_dir)==0)
	     ) ) {
	    add = 0;
	    break;
	  }
	}
      }
      if( add == 1) {
	if (mel->me != NULL ) {
	  mel->next = calloc (1, sizeof (struct mntlist) );
	  mel = mel->next;    
	}
	mel->me = _cpy_fs_data( me );
      }
    }
  }
  endmntent(tab); 
    
  _OSBASE_TRACE(2,("--- enum_all_localfs() exited"));
  return 0;
}


/* ---------------------------------------------------------------------------*/
/*                     end of OSBase_LocalFileSystem.c                        */
/* ---------------------------------------------------------------------------*/

