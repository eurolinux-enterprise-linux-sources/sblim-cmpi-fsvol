/*
 * OSBase_CommonFsvol.c
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
 * Contributors: Marcin Gozdalik <gozdal@gozdal.eu.org>
 *               
 * Description:
 * This shared library provides resource access functionality for the class
 * -. 
 * It is independent from any specific CIM technology.
*/

/* ---------------------------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mntent.h> 
#include <sys/stat.h> 
#include <sys/vfs.h>

#include "OSBase_Common.h"
#include "OSBase_CommonFsvol.h"
#include <unistd.h>

/* ---------------------------------------------------------------------------*/
// private declarations


/* ---------------------------------------------------------------------------*/


/* ---------------------------------------------------------------------------*/
// get all file system instances, independent of their type
/* ---------------------------------------------------------------------------*/

int enum_all_fs( struct mntlist ** ls ) {  
  struct mntent    mebuf;
  char             mntbuf[4000];
  struct mntent  * me  = NULL ;
  struct mntlist * mel = NULL ;
  struct mntlist * ml  = NULL ;
  FILE           * tab = NULL ;
  int              add = 0 ;
  
  _OSBASE_TRACE(3,("--- enum_all_fs() called"));

  mel = calloc (1, sizeof (struct mntlist) );
  *ls = mel;
  
  tab = setmntent( ETC_FSTAB , "r" );
  if( tab == NULL ) { return -1; }
  
  while ( ( me = getmntent_r(tab,&mebuf,mntbuf,sizeof(mntbuf))) != NULL ) {
    
    if (mel->me != NULL ) {
      mel->next = calloc (1, sizeof (struct mntlist) );
      mel = mel->next;    
    }
    if( strncmp(me->mnt_fsname, "LABEL=",6)!=0 )
      mel->me = _cpy_fs_data( me );
  }
  endmntent(tab); 
  
  /* search in mtab for additional fs entries */
  tab = setmntent( ETC_MTAB ,"r");
  if ( tab == NULL ) 
    tab = setmntent( PROC_MOUNTS ,"r");
  if ( tab == NULL ) { return -1; }
  
  while ( ( me = getmntent_r(tab,&mebuf,mntbuf,sizeof(mntbuf))) != NULL ) {
    
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
  endmntent(tab); 
    
  _OSBASE_TRACE(3,("--- enum_all_fs() exited"));
  return 0;
}

/* ---------------------------------------------------------------------------*/
// end of get all file system instances, independent of their type
/* ---------------------------------------------------------------------------*/


/* ---------------------------------------------------------------------------*/
// collect the data of a certain file system instance
/* ---------------------------------------------------------------------------*/

int get_fs_data( struct mntent ** mer , char * name ) {
  struct mntent   mebuf;
  char            mntbuf[4000];
  struct mntent * me  = NULL ;
  FILE          * tab = NULL ;
  *mer                = NULL ;
    
  _OSBASE_TRACE(3,("--- get_fs_data() called"));
    
  tab = setmntent( ETC_FSTAB ,"r");
  if( tab != NULL ) { 
    while ( ( me = getmntent_r(tab,&mebuf,mntbuf,sizeof(mntbuf))) != NULL ) {
      if ( strcmp( name, me->mnt_fsname ) == 0 ) { *mer = _cpy_fs_data( me ); }
    }
    endmntent(tab);
  }
  
  if( !(*mer) ) {
    tab = setmntent( ETC_MTAB ,"r");
    if( tab != NULL ) { 
      while ( ( me = getmntent_r(tab,&mebuf,mntbuf,sizeof(mntbuf))) != NULL ) {
	if ( strcmp( name, me->mnt_fsname ) == 0 ) { *mer = _cpy_fs_data( me ); }
      }
      endmntent(tab);
    }
  }
  
  if( !(*mer) ) {
    tab = setmntent( PROC_MOUNTS ,"r");
    if ( tab == NULL ) return -1;
    while ( ( me = getmntent_r(tab,&mebuf,mntbuf,sizeof(mntbuf))) != NULL ) {
      if( strcmp( name, me->mnt_fsname ) == 0 ) { *mer = _cpy_fs_data( me ); }
    }
    endmntent(tab);
  }
  
  if(*mer) {
    _OSBASE_TRACE(3,("--- get_fs_data() exited"));
    return 0;
  }

  _OSBASE_TRACE(3,("--- get_fs_data() failed"));
  return -1;
}


/* 
 * tool function to copy the struct mntent ;
 * while iterating through the list of mount entries , the data
 * of struct mntent * get lost by calling getmntent_r() again
 */
struct mntent * _cpy_fs_data( struct mntent * me ) {
  struct mntent   mebuf;
  char            mntbuf[4000];
  struct mntent * mer ;
  struct mntent * md   = NULL ;
  FILE          * mtab = NULL ;
  char          * buf  = NULL ;
  
  _OSBASE_TRACE(4,("--- _cpy_fs_data() called"));

  mer = calloc (1,sizeof (struct mntent));
    
  /* check if root :
   * strangely enough the fsname of root fs (mounted at '/')
     * is always /dev/root when reading from /proc/mounts and points
     * to the correct place in /etc/mtab
     */
  if ( strcmp( me->mnt_fsname, "/dev/root" ) == 0 ) {
    mtab = setmntent( ETC_MTAB , "r" );
    while ( ( md = getmntent_r(mtab,&mebuf,mntbuf,sizeof(mntbuf))) != NULL ) {
      if( strcmp(md->mnt_dir, "/") == 0 ) {
	buf = calloc(1, strlen(md->mnt_fsname)+1 );
	strcpy(buf,md->mnt_fsname);
	break;
      }
    }
    endmntent(mtab); 
  }
  if(buf) {
    mer->mnt_fsname = calloc (1, strlen(buf)+1 );
    strcpy( mer->mnt_fsname, buf  );
    free(buf);
  }
  else {
    mer->mnt_fsname = calloc (1, strlen(me->mnt_fsname)+1 );
    strcpy( mer->mnt_fsname, me->mnt_fsname );
  }
  
  mer->mnt_dir = calloc (1, strlen(me->mnt_dir)+1 );
  strcpy( mer->mnt_dir, me->mnt_dir);
  
  mer->mnt_type = calloc (1, strlen(me->mnt_type)+1 );
  strcpy( mer->mnt_type, me->mnt_type);
  
  mer->mnt_opts = calloc (1, strlen(me->mnt_opts)+1 );
  strcpy( mer->mnt_opts, me->mnt_opts);
  
  _OSBASE_TRACE(4,("--- _cpy_fs_data() exited"));
  return mer;
}

void free_mntlist( struct mntlist * lptr) {
  struct mntlist * ls = NULL ;

  if( lptr == NULL ) return;
  for( ; lptr ; ) {
    if(lptr->me) { free_mntent(lptr->me); }
    ls = lptr;
    lptr = lptr->next;
    free(ls);
  }

}

void free_mntent( struct mntent * sptr) {
  if( sptr == NULL ) return; 
  if(sptr->mnt_fsname) free(sptr->mnt_fsname);
  if(sptr->mnt_dir)    free(sptr->mnt_dir);
  if(sptr->mnt_type)   free(sptr->mnt_type);
  if(sptr->mnt_opts)   free(sptr->mnt_opts);
  free(sptr);
}


/* ---------------------------------------------------------------------------*/
// get mount status of a certain file system
/* ---------------------------------------------------------------------------*/

/*
 * 0 ... not mounted
 * 1 ... mounted
 */

unsigned char fs_mount_status( char * fsname ) {
  char       *  cmd   = NULL;
  char       ** hdout = NULL;
  unsigned char vrc   = 0;
  int           rc    = 0;

  _OSBASE_TRACE(4,("--- fs_mount_status() called"));

  cmd = calloc (1, (14+strlen(fsname)));
  strcpy( cmd, "mount | grep ");
  strcat( cmd, fsname );

  rc = runcommand( cmd , NULL , &hdout , NULL );
  if( rc == 0 ) {
    if( strlen(hdout[0])!=0 ) {
      vrc = 1;
    }
  }

  if(cmd) free(cmd);
  freeresultbuf(hdout);
  _OSBASE_TRACE(4,("--- fs_mount_status() exited : %s %i",fsname,vrc));
  return vrc;
}

/*
 * 0 ... not mounted
 * 1 ... mounted
 */

unsigned char fs_default_mount_status( char * fsname ) {
  char       *  cmd   = NULL;
  char       ** hdout = NULL;
  unsigned char vrc   = 0;
  int           rc    = 0;

  _OSBASE_TRACE(4,("--- fs_default_mount_status() called"));

  cmd = calloc (1, (23+strlen(fsname)));
  strcpy( cmd, "cat /etc/fstab | grep ");
  strcat( cmd, fsname );

  rc = runcommand( cmd , NULL , &hdout , NULL );
  if( rc == 0 ) {
    if( strlen(hdout[0])!=0 ) {
      if( strstr(hdout[0],"noauto") == NULL ) { vrc = 1; }
      else { vrc = 0; }
    }
  }

  if(cmd) free(cmd);
  freeresultbuf(hdout);
  _OSBASE_TRACE(4,("--- fs_default_mount_status() exited : %s %i",fsname,vrc));
  return vrc;
}


/* ---------------------------------------------------------------------------*/
/*                         end of OSBase_CommonFsvol.c                        */
/* ---------------------------------------------------------------------------*/

