/*
 * cmpiOSBase_NFSProvider.c
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
 * Interface Type : Common Manageability Programming Interface ( CMPI )
 *
 * Description: 
 * 
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mntent.h> 
#include <sys/stat.h> 
#include <sys/vfs.h>

#include "cmpidt.h"
#include "cmpift.h"
#include "cmpimacs.h"

#include "OSBase_Common.h"
#include "cmpiOSBase_Common.h"
#include "OSBase_CommonFsvol.h"
#include "cmpiOSBase_NFS.h"

static const CMPIBroker * _broker;

/* ---------------------------------------------------------------------------*/
/* private declarations                                                       */
#ifdef CMPI_VER_100
#define OSBase_NFSProviderSetInstance OSBase_NFSProviderModifyInstance 
#endif


/* ---------------------------------------------------------------------------*/


/* ---------------------------------------------------------------------------*/
/*                      Instance Provider Interface                           */
/* ---------------------------------------------------------------------------*/


CMPIStatus OSBase_NFSProviderCleanup( CMPIInstanceMI * mi, 
           const CMPIContext * ctx, CMPIBoolean terminate) { 
  _OSBASE_TRACE(1,("--- %s CMPI Cleanup() called",_ClassName));
  _OSBASE_TRACE(1,("--- %s CMPI Cleanup() exited",_ClassName));
  CMReturn(CMPI_RC_OK);
}

CMPIStatus OSBase_NFSProviderEnumInstanceNames( CMPIInstanceMI * mi, 
           const CMPIContext * ctx, 
           const CMPIResult * rslt, 
           const CMPIObjectPath * ref) { 
  CMPIObjectPath * op = NULL;  
  CMPIStatus       rc = {CMPI_RC_OK, NULL};
  struct mntlist * lptr  = NULL;
  struct mntlist * rm    = NULL;
  
  _OSBASE_TRACE(1,("--- %s CMPI EnumInstanceNames() called",_ClassName));

  if( enum_all_nfs( &lptr ) != 0 ) {
    CMSetStatusWithChars( _broker, &rc, 
			  CMPI_RC_ERR_FAILED, "Could not list filesystems." ); 
    _OSBASE_TRACE(1,("--- %s CMPI EnumInstanceNames() failed : %s",_ClassName,CMGetCharPtr(rc.msg)));
    return rc;
  }
  
  rm = lptr;  
  // iteration of list of nfs filesystems
  if( lptr->me ) {
    for ( ; lptr && rc.rc == CMPI_RC_OK ; lptr = lptr->next ) {
      op = _makePath_NFS( _broker, ctx, ref, lptr->me, &rc );
      if( op == NULL ) { 
	if( rc.msg != NULL ){
	  _OSBASE_TRACE(1,("--- %s CMPI EnumInstanceNames() failed : %s",_ClassName,CMGetCharPtr(rc.msg)));
	}
	CMSetStatusWithChars( _broker, &rc,
			      CMPI_RC_ERR_FAILED, "Transformation from internal structure to CIM ObjectPath failed." ); 
	if(rm) free_mntlist(rm);
	_OSBASE_TRACE(1,("--- %s CMPI EnumInstanceNames() failed : %s",_ClassName,CMGetCharPtr(rc.msg)));
	return rc; 
      }
      else { CMReturnObjectPath( rslt, op ); }
    }
    if(rm) free_mntlist(rm);
  }

  CMReturnDone( rslt );
  _OSBASE_TRACE(1,("--- %s CMPI EnumInstanceNames() exited",_ClassName));
  return rc;
}

CMPIStatus OSBase_NFSProviderEnumInstances( CMPIInstanceMI * mi, 
           const CMPIContext * ctx, 
           const CMPIResult * rslt, 
           const CMPIObjectPath * ref, 
           const char ** properties) { 
  CMPIInstance   * ci    = NULL;  
  CMPIStatus       rc    = {CMPI_RC_OK, NULL};
  struct mntlist * lptr  = NULL;
  struct mntlist * rm    = NULL;

  _OSBASE_TRACE(1,("--- %s CMPI EnumInstances() called",_ClassName));

  if( enum_all_nfs( &lptr ) != 0 ) {
    CMSetStatusWithChars( _broker, &rc, 
			  CMPI_RC_ERR_FAILED, "Could not list filesystems." ); 
    _OSBASE_TRACE(1,("--- %s CMPI EnumInstances() failed : %s",_ClassName,CMGetCharPtr(rc.msg)));
    return rc;
  }
  
  rm = lptr;  
  // iteration of list of nfs filesystems
  if( lptr->me ) {
    for ( ; lptr && rc.rc == CMPI_RC_OK ; lptr = lptr->next ) {
      ci = _makeInst_NFS( _broker, ctx, ref, properties, lptr->me, &rc );
      if( ci == NULL ) { 
	if( rc.msg != NULL ) {
	  _OSBASE_TRACE(1,("--- %s CMPI EnumInstances() failed : %s",_ClassName,CMGetCharPtr(rc.msg)));
	}
	CMSetStatusWithChars( _broker, &rc,
			      CMPI_RC_ERR_FAILED, "Transformation from internal structure to CIM Instance failed." ); 
	_OSBASE_TRACE(1,("--- %s CMPI EnumInstances() failed : %s",_ClassName,CMGetCharPtr(rc.msg)));
	if(rm) free_mntlist(rm);
	return rc; 
      }
      else { CMReturnInstance( rslt, ci ); }
    }
    if(rm) free_mntlist(rm);
  }

  CMReturnDone( rslt );
  _OSBASE_TRACE(1,("--- %s CMPI EnumInstances() exited",_ClassName));
  return rc;
}

CMPIStatus OSBase_NFSProviderGetInstance( CMPIInstanceMI * mi, 
           const CMPIContext * ctx, 
           const CMPIResult * rslt, 
           const CMPIObjectPath * cop, 
           const char ** properties) {
  CMPIInstance  * ci    = NULL;
  CMPIStatus      rc    = {CMPI_RC_OK, NULL};
  CMPIString    * name  = NULL;
  struct mntent * sptr  = NULL;
  int             cmdrc = 0;

  _OSBASE_TRACE(1,("--- %s CMPI GetInstance() called",_ClassName));

  _check_system_key_value_pairs( _broker, cop, "CSCreationClassName", "CSName", &rc );
  if( rc.rc != CMPI_RC_OK ) { 
    _OSBASE_TRACE(1,("--- %s CMPI GetInstance() failed : %s",_ClassName,CMGetCharPtr(rc.msg)));
    return rc; 
  }

  name = CMGetKey( cop, "Name", &rc).value.string;
  if( name == NULL ) {    
    CMSetStatusWithChars( _broker, &rc, 
			  CMPI_RC_ERR_FAILED, "Could not get NFS Name." );
    _OSBASE_TRACE(1,("--- %s CMPI GetInstance() failed : %s",_ClassName,CMGetCharPtr(rc.msg))); 
    return rc;
  }

  cmdrc = get_fs_data( &sptr, CMGetCharPtr(name));
  if( cmdrc != 0 ) {
    CMSetStatusWithChars( _broker, &rc, 
			  CMPI_RC_ERR_FAILED, "The specified filesystem entry is invalid." ); 
    _OSBASE_TRACE(1,("--- %s CMPI GetInstance() failed : %s",_ClassName,CMGetCharPtr(rc.msg)));
    return rc;
  }

  ci = _makeInst_NFS( _broker, ctx, cop, properties, sptr, &rc );
  if(sptr) free_mntent(sptr);

  if( ci == NULL ) {
    if( rc.msg != NULL ) {
      _OSBASE_TRACE(1,("--- %s CMPI GetInstance() failed : %s",_ClassName,CMGetCharPtr(rc.msg)));
    }
    else {
      _OSBASE_TRACE(1,("--- %s CMPI GetInstance() failed",_ClassName));
    } 
    return rc;
  }

  CMReturnInstance( rslt, ci );
  CMReturnDone(rslt);
  _OSBASE_TRACE(1,("--- %s CMPI GetInstance() exited",_ClassName));
  return rc;
}

CMPIStatus OSBase_NFSProviderCreateInstance( CMPIInstanceMI * mi, 
           const CMPIContext * ctx, 
           const CMPIResult * rslt, 
           const CMPIObjectPath * cop, 
           const CMPIInstance * ci) {
  CMPIStatus rc = {CMPI_RC_OK, NULL};
  /*
  CMPIObjectPath * op     = NULL;
  FILE           * ffstab = NULL;
  struct mntent    me;
  int              cmdrc  = 0;
  CMPIData         dt_name;
  CMPIData         dt_root;
  */

  _OSBASE_TRACE(1,("--- %s CMPI CreateInstance() called",_ClassName));

  /*
  dt_name = CMGetProperty(ci,"Name",&rc);  
  if( rc.rc == CMPI_RC_ERR_NOT_FOUND ) {    
    CMSetStatusWithChars( _broker, &rc, 
			  CMPI_RC_ERR_FAILED, "Could not get NFS Name." ); 
    return rc;
  }

  dt_root = CMGetProperty(ci,"Root",&rc);
  if( rc.rc == CMPI_RC_ERR_NOT_FOUND ) {  
    CMSetStatusWithChars( _broker, &rc, 
			  CMPI_RC_ERR_NOT_FOUND, "Could not get NFS Root." ); 
    return rc;
  }

  me.mnt_fsname = strdup(CMGetCharPtr(dt_name.value.string));
  me.mnt_dir    = strdup(CMGetCharPtr(dt_root.value.string));
  me.mnt_type   = strdup("nfs");
  me.mnt_opts   = calloc(1,9);
  strcpy(me.mnt_opts, "defaults");
  me.mnt_freq   = 0;
  me.mnt_passno = 0;
  
  ffstab = fopen("/etc/fstab","r+");
  if( ffstab ) {
    cmdrc = addmntent( ffstab, &me );
    fclose( ffstab );
    op = CMGetObjectPath(ci,&rc); 
    CMSetNameSpace(op,CMGetCharPtr(CMGetNameSpace(cop,&rc)));
    CMReturnObjectPath( rslt, op );
  }

  CMReturnDone(rslt);
  CMReturn(CMPI_RC_OK);
  */

  CMSetStatusWithChars( _broker, &rc, 
			CMPI_RC_ERR_NOT_SUPPORTED, "CIM_ERR_NOT_SUPPORTED" ); 

  _OSBASE_TRACE(1,("--- %s CMPI CreateInstance() exited",_ClassName));
  return rc;
}

CMPIStatus OSBase_NFSProviderSetInstance( CMPIInstanceMI * mi, 
           const CMPIContext * ctx, 
           const CMPIResult * rslt, 
           const CMPIObjectPath * cop,
           const CMPIInstance * ci, 
           const char **properties) {
  CMPIStatus rc = {CMPI_RC_OK, NULL};

  _OSBASE_TRACE(1,("--- %s CMPI SetInstance() called",_ClassName));

  CMSetStatusWithChars( _broker, &rc, 
			CMPI_RC_ERR_NOT_SUPPORTED, "CIM_ERR_NOT_SUPPORTED" ); 

  _OSBASE_TRACE(1,("--- %s CMPI SetInstance() exited",_ClassName));
  return rc;
}

CMPIStatus OSBase_NFSProviderDeleteInstance( CMPIInstanceMI * mi, 
           const CMPIContext * ctx, 
           const CMPIResult * rslt, 
           const CMPIObjectPath * cop) {
  CMPIStatus rc = {CMPI_RC_OK, NULL}; 

  _OSBASE_TRACE(1,("--- %s CMPI DeleteInstance() called",_ClassName));

  CMSetStatusWithChars( _broker, &rc, 
			CMPI_RC_ERR_NOT_SUPPORTED, "CIM_ERR_NOT_SUPPORTED" ); 

  _OSBASE_TRACE(1,("--- %s CMPI DeleteInstance() exited",_ClassName));
  return rc;
}

CMPIStatus OSBase_NFSProviderExecQuery( CMPIInstanceMI * mi, 
           const CMPIContext * ctx, 
           const CMPIResult * rslt, 
           const CMPIObjectPath * ref, 
           const char * lang, 
           const char * query) {
  CMPIStatus rc = {CMPI_RC_OK, NULL};

  _OSBASE_TRACE(1,("--- %s CMPI ExecQuery() called",_ClassName));

  CMSetStatusWithChars( _broker, &rc, 
			CMPI_RC_ERR_NOT_SUPPORTED, "CIM_ERR_NOT_SUPPORTED" ); 

  _OSBASE_TRACE(1,("--- %s CMPI ExecQuery() exited",_ClassName));
  return rc;
}


/* ---------------------------------------------------------------------------*/
/*                              Provider Factory                              */
/* ---------------------------------------------------------------------------*/

CMInstanceMIStub( OSBase_NFSProvider, 
                  OSBase_NFSProvider, 
                  _broker, 
                  CMNoHook);


/* ---------------------------------------------------------------------------*/
/*                       end of cmpiOSBase_NFSProvider                        */
/* ---------------------------------------------------------------------------*/

