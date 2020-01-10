/*
 * cmpiOSBase_BootOSFromFS.c
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
 * class Linux_BootOSFromFS.
 *
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cmpidt.h"
#include "cmpimacs.h"

#include "cmpiOSBase_Common.h"
#include "OSBase_Common.h"
#include "cmpiOSBase_CommonFsvol.h"

#include "cmpiOSBase_BootOSFromFS.h"

/* ---------------------------------------------------------------------------*/
/* private declarations                                                       */


/* ---------------------------------------------------------------------------*/



/* ---------------------------------------------------------------------------*/
/*                            Factory functions                               */
/* ---------------------------------------------------------------------------*/

int _assoc_create_inst_BootOSFromFS( const CMPIBroker * _broker, 
				     const CMPIContext * ctx,
				     const CMPIResult * rslt,
				     const CMPIObjectPath * cop,
				     int inst,
				     CMPIStatus * rc) {
  CMPIObjectPath  * op  = NULL;
  CMPIEnumeration * en  = NULL;
  CMPIData          data ;
  int               arc = 0;

  _OSBASE_TRACE(2,("--- _assoc_create_inst_BootOSFromFS() called"));

  op = CMNewObjectPath( _broker, CMGetCharPtr(CMGetNameSpace(cop,rc)),
			_RefRightClass, rc );
  if( CMIsNullObject(op) ) { 
    CMSetStatusWithChars( _broker, rc, 
			  CMPI_RC_ERR_FAILED, "Create CMPIObjectPath failed." ); 
    _OSBASE_TRACE(2,("--- _assoc_create_inst_BootOSFromFS() failed : %s",CMGetCharPtr(rc->msg)));
    return -1; 
  }
  
  en = CBEnumInstanceNames( _broker, ctx, op, rc);
  if( en == NULL ) {
    CMSetStatusWithChars( _broker, rc, 
			  CMPI_RC_ERR_FAILED, "CBEnumInstanceNames( _broker, ctx, op, rc)" ); 
    _OSBASE_TRACE(2,("--- _assoc_create_inst_BootOSFromFS() failed : %s",CMGetCharPtr(rc->msg)));
    return -1; 
  }

  /* this approach works only from OperatingSystem to the CMPIInstance(s)
   * of the different supported LocalFileSystems
   */

  while( CMHasNext( en, rc) ) {
    data = CMGetNext( en, rc);
    if( data.value.ref == NULL ){
      CMSetStatusWithChars( _broker, rc, 
			    CMPI_RC_ERR_FAILED, "CMGetNext( en, rc)" ); 
      _OSBASE_TRACE(2,("--- _assoc_create_inst_BootOSFromFS() failed : %s",CMGetCharPtr(rc->msg)));
      return -1; 
    }
    arc = _assoc_BootOSFromFS( _broker, ctx, rslt, data.value.ref, inst, 0, rc);
  }

  _OSBASE_TRACE(2,("--- _assoc_create_inst_BootOSFromFS() exited"));
  return 0;
}



int _get_boot_fs( char ** root_fsclass, char ** root_fsname,
		  char ** boot_fsclass, char ** boot_fsname ) {
  int rc = 0;

  _OSBASE_TRACE(4,("--- _get_boot_fs() called"));

  /* get partition of "/" directory*/
  rc = get_fs_of_dir( "/" , root_fsclass , root_fsname );
  if( rc != 0 ) { goto exit; }
  
  /* get partition of "/boot/" directory*/
  rc = get_fs_of_dir( "/boot/" , boot_fsclass , boot_fsname );
  
  if( strcmp( *boot_fsname, *root_fsname) == 0 ) {
    /* "/" and "/boot/" are located within the same partition */
    if( *boot_fsclass ) free(*boot_fsclass);
    if( *boot_fsname )  free(*boot_fsname);
    *boot_fsclass = NULL;
    *boot_fsname  = NULL;
  }
  
 exit:
  _OSBASE_TRACE(4,("--- _get_boot_fs() exited"));
  return rc;
}


CMPIObjectPath * _assoc_get_boot_fs_OP( const CMPIBroker * _broker, 
					const CMPIObjectPath * ref, 
					const char * fs_class, 
					const char * fs_name ,
					CMPIStatus * rc) {
  CMPIObjectPath * op = NULL;

  _OSBASE_TRACE(4,("--- _assoc_get_boot_fs_OP() called"));

  /* tool method call to get the unique name of the system */
  if( !get_system_name() ) {   
    CMSetStatusWithChars( _broker, rc, 
			  CMPI_RC_ERR_FAILED, "no host name found" );
    _OSBASE_TRACE(4,("--- _assoc_get_boot_fs_OP() failed : %s",CMGetCharPtr(rc->msg)));
    goto exit;
  }

  /* create CMPIObjectPath of _fs_name */
  op = CMNewObjectPath( _broker, CMGetCharPtr(CMGetNameSpace(ref,rc)),
			fs_class, rc );
  if( CMIsNullObject(op) ) { 
    CMSetStatusWithChars( _broker, rc, 
			  CMPI_RC_ERR_FAILED, "Create CMPIObjectPath failed." ); 
    _OSBASE_TRACE(4,("--- _assoc_get_boot_fs_OP() failed : %s",CMGetCharPtr(rc->msg)));
    goto exit;
  }

  CMAddKey(op, "CSCreationClassName", CSCreationClassName, CMPI_chars);  
  CMAddKey(op, "CSName", get_system_name(), CMPI_chars);
  CMAddKey(op, "CreationClassName", fs_class, CMPI_chars);  
  CMAddKey(op, "Name", fs_name, CMPI_chars);

 exit:
  _OSBASE_TRACE(4,("--- _assoc_get_boot_fs_OP() exited"));
  return op; 
}


CMPIObjectPath * _assoc_get_assoc_OP_BootOSFromFS( const CMPIBroker * _broker, 
						   const CMPIObjectPath * ref_left,
						   const CMPIObjectPath * ref_right,
						   CMPIStatus * rc) {
  CMPIObjectPath * op = NULL;
  CMPIInstance   * ci = NULL;

  _OSBASE_TRACE(4,("--- _assoc_get_assoc_OP_BootOSFromFS() called"));

  ci = _assoc_get_assoc_INST_BootOSFromFS( _broker, ref_left,ref_right,rc);
  if( ci == NULL ) { return NULL; }

  op = CMGetObjectPath(ci,rc); 
  CMSetNameSpace(op,CMGetCharPtr(CMGetNameSpace(ref_left,rc)));

  _OSBASE_TRACE(4,("--- _assoc_get_assoc_OP_BootOSFromFS() exited"));
  return op; 
}


CMPIInstance * _assoc_get_assoc_INST_BootOSFromFS( const CMPIBroker * _broker,
						   const CMPIObjectPath * ref_left,
						   const CMPIObjectPath * ref_right,
						   CMPIStatus * rc) {
  CMPIObjectPath * op = NULL;
  CMPIInstance   * ci = NULL;

  _OSBASE_TRACE(4,("--- _assoc_get_assoc_INST_BootOSFromFS() called"));

  /* create CMPIObjectPath of this association <_ClassName> */
  op = CMNewObjectPath( _broker, CMGetCharPtr(CMGetNameSpace(ref_left,rc)),
			_ClassName, rc );
  if( CMIsNullObject(op) ) { 
    CMSetStatusWithChars( _broker, rc, 
			  CMPI_RC_ERR_FAILED, "Create CMPIObjectPath failed." ); 
    goto exit;
  }

  ci = CMNewInstance( _broker, op, rc);
  if( CMIsNullObject(ci) ) { 
    CMSetStatusWithChars( _broker, rc, 
			  CMPI_RC_ERR_FAILED, "Create CMPIInstance failed." ); 
    _OSBASE_TRACE(4,("--- _assoc_get_assoc_INST_BootOSFromFS() failed : %s",CMGetCharPtr(rc->msg)));
    goto exit;
  }

  CMSetProperty( ci, _RefLeft, (CMPIValue*)&(ref_left), CMPI_ref ); 
  CMSetProperty( ci, _RefRight, (CMPIValue*)&(ref_right), CMPI_ref );

 exit:
  _OSBASE_TRACE(4,("--- _assoc_get_assoc_INST_BootOSFromFS() exited"));
  return ci; 
}


/* combination of int <inst> and int <associators> :         
 * 0 0 -> referenceNames()
 * 1 0 -> references()
 * 0 1 -> associatorNames()
 * 1 1 -> associators()                     
 */

int _assoc_BootOSFromFS( const CMPIBroker * _broker, 
			 const CMPIContext * ctx,
			 const CMPIResult * rslt,
			 const CMPIObjectPath * ref, 
			 int inst,
			 int associators,
			 CMPIStatus * rc) {
  CMPIObjectPath  * op           = NULL;
  CMPIObjectPath  * rop          = NULL;
  CMPIObjectPath  * root_op      = NULL;
  CMPIObjectPath  * boot_op      = NULL;
  CMPIInstance    * rci          = NULL;
  CMPIInstance    * cis          = NULL;
  CMPIEnumeration * en           = NULL;
  CMPIData          data;
  CMPIData          dt_name;
  CMPIString      * sourceClass  = NULL;
  char            * boot_fsclass = NULL;
  char            * boot_fsname  = NULL;
  char            * root_fsclass = NULL;
  char            * root_fsname  = NULL;
  int               boot         = 0;
  int               cmdrc        = 0;

  _OSBASE_TRACE(2,("--- _assoc_BootOSFromFS() called"));

  /* check if source instance does exist */
  cis = CBGetInstance(_broker, ctx, ref, NULL, rc);
  if( cis == NULL ) {
    if( rc->rc == CMPI_RC_ERR_FAILED ) {
      CMSetStatusWithChars( _broker, rc,
			    CMPI_RC_ERR_FAILED, "GetInstance of source object failed.");
    }
    if( rc->rc == CMPI_RC_ERR_NOT_FOUND ) {
      CMSetStatusWithChars( _broker, rc,
			    CMPI_RC_ERR_NOT_FOUND, "Source object not found.");
    }
    _OSBASE_TRACE(2,("--- _assoc_BootOSFromFS() failed : %s",CMGetCharPtr(rc->msg)));
    return -1;
  }

  /* has "/boot/" its own partition ? */
  cmdrc = _get_boot_fs( &root_fsclass, &root_fsname, &boot_fsclass, &boot_fsname );
  if( cmdrc != 0 ) goto exit;

  if( root_fsclass == NULL ) { goto exit; }

  if( boot_fsclass != NULL && boot_fsname != NULL ) {
    if( (strcasecmp(root_fsclass, _RefLeftClass) != 0) && 
	(strcasecmp(boot_fsclass, _RefLeftClass) != 0) &&
	(strcasecmp(_RefLeftClass, _RefLeftClass_bk) != 0) ) { goto exit; }
    boot = 1;
  }

  if( strcasecmp(root_fsclass, _RefLeftClass) != 0 &&
      strcasecmp(_RefLeftClass, _RefLeftClass_bk) != 0 ) { goto exit; }

  /* get ClassName of source CMPIObjectPath ref */
  sourceClass = CMGetClassName(ref, rc);
  if( sourceClass == NULL ) {
    CMSetStatusWithChars( _broker, rc, 
			  CMPI_RC_ERR_FAILED, "CMGetClassName(ref, rc)" ); 
    _OSBASE_TRACE(2,("--- _assoc_BootOSFromFS() failed : %s",CMGetCharPtr(rc->msg)));
    cmdrc = -1;
    goto exit;
  }

  if( strcasecmp(CMGetCharPtr(sourceClass),_RefRightClass) == 0 ) {
  
    /* create CMPIObjectPath of "/" directory */
    root_op = _assoc_get_boot_fs_OP( _broker, ref, root_fsclass, root_fsname, rc);
    if( root_op == NULL ){
      CMSetStatusWithChars( _broker, rc, 
			    CMPI_RC_ERR_FAILED, 
			    "_assoc_get_boot_fs_OP( ref, root_fsclass, root_fsname, rc)" ); 
      _OSBASE_TRACE(2,("--- _assoc_BootOSFromFS() failed : %s",CMGetCharPtr(rc->msg)));
      cmdrc = -1;
      goto exit;
    }
    //    else { fprintf(stderr,"root_op : %s\n",CMGetCharPtr(CDToString(_broker, root_op, rc)) ); }
    
    /* if "/boot/" has an own partition, this instance is part of the result */
    if( boot == 1 ) {
      boot_op = _assoc_get_boot_fs_OP( _broker, ref, boot_fsclass, boot_fsname, rc);
      if( boot_op == NULL ){
	CMSetStatusWithChars( _broker, rc, 
			      CMPI_RC_ERR_FAILED, 
			      "_assoc_get_boot_fs_OP( ref, boot_fsclass, boot_fsname, rc)" ); 
	_OSBASE_TRACE(2,("--- _assoc_BootOSFromFS() failed : %s",CMGetCharPtr(rc->msg)));
	cmdrc = -1;
	goto exit;
      }
      //      else { fprintf(stderr,"boot_op : %s\n",CMGetCharPtr(CDToString(_broker, boot_op, rc)) ); }
    }   

    if( associators == 0 ) {
      /* references() || referenceNames() */
      if( inst == 0 ) { 
	rop = _assoc_get_assoc_OP_BootOSFromFS( _broker, root_op, ref, rc );
	if( rop == NULL ) { goto exit; }
	CMReturnObjectPath( rslt, rop ); 
	if( boot == 1 ) { 
	  rop = _assoc_get_assoc_OP_BootOSFromFS( _broker, boot_op, ref, rc );
	  if( rop == NULL ) { goto exit; }
	  CMReturnObjectPath( rslt, rop );  
	}
      }
      else { /* inst == 1 */
	rci = _assoc_get_assoc_INST_BootOSFromFS( _broker, root_op, ref, rc );
	if( rci == NULL ) { goto exit; }
	CMReturnInstance( rslt, rci );
	if( boot == 1 ) { 
	  rci = _assoc_get_assoc_INST_BootOSFromFS( _broker, boot_op, ref, rc );
	  if( rci == NULL ) { goto exit; }
	  CMReturnInstance( rslt, rci );
	}
      }
    }
    else { /* associators == 1 */
      /* associators() || associatorNames() */
      if( inst == 0 ) { 
	CMReturnObjectPath( rslt, root_op ); 
	if( boot == 1 ) { CMReturnObjectPath( rslt, boot_op );  }
      }
      else { /* inst == 1 */
	rci = CBGetInstance( _broker, ctx, root_op, NULL, rc);
	if( rci == NULL ) { goto exit; }
	CMReturnInstance( rslt, rci );
	if( boot == 1 ) { 
	  rci = CBGetInstance( _broker, ctx, boot_op, NULL, rc);
	  if( rci == NULL ) { goto exit; }
	  CMReturnInstance( rslt, rci );
	}
      }
    }  
   
  }
  else if( strcasecmp(CMGetCharPtr(sourceClass), _RefLeftClassExt2 ) == 0 || 
	   strcasecmp(CMGetCharPtr(sourceClass), _RefLeftClassExt3 ) == 0 ||
	   strcasecmp(CMGetCharPtr(sourceClass), _RefLeftClassReiser ) == 0  ) {
    
    dt_name = CMGetKey( ref, "Name", rc);
    if( dt_name.value.string == NULL ) { 
      CMSetStatusWithChars( _broker, rc, 
			    CMPI_RC_ERR_FAILED, "CMGetKey( ref, 'Name', rc)" ); 
      _OSBASE_TRACE(2,("--- _assoc_BootOSFromFS() failed : %s",CMGetCharPtr(rc->msg)));
      cmdrc = -1; 
      goto exit; 
    }

    /* FS is boot device */
    if( (strcmp(root_fsname, CMGetCharPtr(dt_name.value.string)) == 0) ||
	(boot_fsname != NULL) ) {

      if( boot_fsname != NULL ) {
	if( (strcmp(boot_fsname, CMGetCharPtr(dt_name.value.string)) != 0) &&
	    (strcmp(root_fsname, CMGetCharPtr(dt_name.value.string)) != 0) ) 
	  { goto exit; }
      }

      op = CMNewObjectPath( _broker, CMGetCharPtr(CMGetNameSpace(ref,rc)),
			    _RefRightClass, rc );
      if( CMIsNullObject(op) ) { 
	CMSetStatusWithChars( _broker, rc, 
			      CMPI_RC_ERR_FAILED, "Create CMPIObjectPath failed." ); 
	_OSBASE_TRACE(2,("--- _assoc_BootOSFromFS() failed : %s",CMGetCharPtr(rc->msg)));
	cmdrc = -1;
	goto exit; 
      }
      
      en = CBEnumInstanceNames( _broker, ctx, op, rc);
      if( en == NULL ) {      
	CMSetStatusWithChars( _broker, rc, 
			      CMPI_RC_ERR_FAILED, "CBEnumInstanceNames( _broker, ctx, op, rc)" ); 
	_OSBASE_TRACE(2,("--- _assoc_BootOSFromFS() failed : %s",CMGetCharPtr(rc->msg)));
	cmdrc = -1; 
	goto exit; 
      }

      while( CMHasNext( en, rc) ) {
	data = CMGetNext( en, rc);
	if( data.value.ref == NULL ) {   
	  CMSetStatusWithChars( _broker, rc, 
				CMPI_RC_ERR_FAILED, "CMGetNext( en, rc)" ); 
	  _OSBASE_TRACE(2,("--- _assoc_BootOSFromFS() failed : %s",CMGetCharPtr(rc->msg)));
	  cmdrc = -1; 
	  goto exit; 
	}
	
	if( associators == 0 ) {
	  /* references() || referenceNames() */
	  if( inst == 0 ) { 
	    rop = _assoc_get_assoc_OP_BootOSFromFS( _broker, ref, data.value.ref, rc );
	    if( rop == NULL ) { goto exit; }
	    CMReturnObjectPath( rslt, rop ); }
	  else { /* inst == 1 */
	    rci = _assoc_get_assoc_INST_BootOSFromFS( _broker, ref, data.value.ref, rc );
	    if( rci == NULL ) { goto exit; }
	    CMReturnInstance( rslt, rci );
	  }
	}
	else { /* associators == 1 */
	  /* associators() || associatorNames() */
	  if( inst == 0 ) { CMReturnObjectPath( rslt, data.value.ref ); }
	  else { /* inst == 1 */
	    rci = CBGetInstance( _broker, ctx, data.value.ref, NULL, rc);
	    if( rci == NULL ) { goto exit; }
	    CMReturnInstance( rslt, rci );
	  }
	}
      }
    } /* end of FS is boot device */
  }
  else { goto exit; }

 exit:
  if(boot_fsclass) free(boot_fsclass);
  if(boot_fsname)  free(boot_fsname);
  if(root_fsclass) free(root_fsclass);
  if(root_fsname)  free(root_fsname);

  _OSBASE_TRACE(2,("--- _assoc_BootOSFromFS() exited"));
  return cmdrc;
}


/* ---------------------------------------------------------------------------*/
/*                     end of cmpiOSBase_BootOSFromFS.c                       */
/* ---------------------------------------------------------------------------*/

