/*
 * cmpiOSBase_BlockStorageStatisticalData
 *
 * (C) Copyright IBM Corp. 2004, 2005, 2009
 *
 * THIS FILE IS PROVIDED UNDER THE TERMS OF THE ECLIPSE PUBLIC LICENSE 
 * ("AGREEMENT"). ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS FILE 
 * CONSTITUTES RECIPIENTS ACCEPTANCE OF THE AGREEMENT.
 *
 * You can obtain a current copy of the Eclipse Public License from
 * http://www.opensource.org/licenses/eclipse-1.0.php
 *
 * Author:       Michael Schuele <schuelem@de.ibm.com>
 * Contributors:
 *
 * Interface Type : Common Manageability Programming Interface ( CMPI )
 *
 * Description: Provider for BlockStorageStatisticalData
 * 
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cmpidt.h"
#include "cmpift.h"
#include "cmpimacs.h"

#include "OSBase_Common.h"
#include "cmpiOSBase_Common.h"
#include "cmpiOSBase_BlockStorageStatisticalData.h"

static const CMPIBroker * _broker;
#ifdef CMPI_VER_100
#define OSBase_BlockStorageStatisticalDataProviderSetInstance OSBase_BlockStorageStatisticalDataProviderModifyInstance 
#endif


/* ---------------------------------------------------------------------------*/
/* private declarations                                                       */


/* ---------------------------------------------------------------------------*/


/* ---------------------------------------------------------------------------*/
/*                      Instance Provider Interface                           */
/* ---------------------------------------------------------------------------*/


CMPIStatus OSBase_BlockStorageStatisticalDataProviderCleanup(CMPIInstanceMI * mi, 
                                                             const CMPIContext * ctx,
							     CMPIBoolean terminate) { 
    _OSBASE_TRACE(1,("--- %s CMPI Cleanup() called",_ClassName));

    CMReturn(CMPI_RC_OK);
}

CMPIStatus OSBase_BlockStorageStatisticalDataProviderEnumInstanceNames(CMPIInstanceMI * mi, 
                                                                       const CMPIContext * ctx, 
                                                                       const CMPIResult * rslt, 
                                                                       const CMPIObjectPath * ref) { 
    CMPIStatus rc = {CMPI_RC_OK, NULL};
    CMPIObjectPath * op = NULL;
    struct disk_list * disk_list = NULL;
    struct disk_list * list_backup = NULL; 

    _OSBASE_TRACE(4,("--- CMPI EnumInstanceNames() entered"));      

    enum_all_disks(&disk_list);

    _OSBASE_TRACE(1,("--- %s CMPI EnumInstanceNames() called",_ClassName));

    if (disk_list == NULL) {
        _OSBASE_TRACE(1,("--- CMPI EnumInstanceNames() failed : no disks available"));
        return rc;
    }

    list_backup = disk_list;

    while (disk_list != NULL) {
        op = _makePath_BlockStorageStatisticalData(_broker,ctx,ref,&rc, disk_list->disk);

        if (op == NULL) {
            if (rc.msg != NULL) {
                _OSBASE_TRACE(1,("--%s CMPI EnumInstanceNames() failed : %s", _ClassName,CMGetCharPtr(rc.msg)));
            } else {
                _OSBASE_TRACE(1,("--%s CMPI EnumInstanceNames() failed", _ClassName));
            }
            return rc;

        }
        CMReturnObjectPath(rslt, op);
        disk_list = disk_list->next;
    }

    if (list_backup) {
        free_disk_list(list_backup);
    }

    CMReturnDone(rslt);

    _OSBASE_TRACE(1,("--- %s CMPI EnumInstanceNames() exited",_ClassName));
    return rc;
}

CMPIStatus OSBase_BlockStorageStatisticalDataProviderEnumInstances(CMPIInstanceMI * mi, 
                                                                   const CMPIContext * ctx, 
                                                                   const CMPIResult * rslt, 
                                                                   const CMPIObjectPath * ref, 
                                                                   const char ** properties) { 
    CMPIStatus rc = {CMPI_RC_OK, NULL};
    const CMPIInstance * ci  = NULL;

    struct disk_list * disk_list = NULL;
    struct disk_list * list_backup = NULL; 

    _OSBASE_TRACE(1,("--- %s CMPI EnumInstances() called",_ClassName));

    enum_all_disks(&disk_list);

    list_backup = disk_list;

    if (disk_list == NULL) {
        _OSBASE_TRACE(1,("--- CMPI EnumInstances() failed : no disks available"));
        return rc;
    }

    while (disk_list != NULL) {
        ci = _makeInst_BlockStorageStatisticalData(_broker, ctx, ref, properties, &rc, disk_list->disk);

        if (ci == NULL) {
            if (rc.msg != NULL) {
                _OSBASE_TRACE(1,("--%s CMPI EnumInstances() failed : %s", _ClassName,CMGetCharPtr(rc.msg)));
            } else {
                _OSBASE_TRACE(1,("--%s CMPI EnumInstances() failed", _ClassName));
            }

            return rc;
        }

        CMReturnInstance( rslt, ci );
        disk_list = disk_list->next;
    }

    if (list_backup) free_disk_list(list_backup);


    CMReturnDone( rslt );
    _OSBASE_TRACE(1,("--- %s CMPI EnumInstances() exited",_ClassName));
    return rc;
}

CMPIStatus OSBase_BlockStorageStatisticalDataProviderGetInstance(CMPIInstanceMI * mi, 
                                                                 const CMPIContext * ctx, 
                                                                 const CMPIResult * rslt, 
                                                                 const CMPIObjectPath * cop, 
                                                                 const char ** properties) {
    CMPIInstance     *   ci    = NULL;
    CMPIStatus           rc    = {CMPI_RC_OK, NULL};
    CMPIString       * name    = NULL;
    char             * c_name  = NULL;
    char             * disk_id = NULL;
    struct disk_data * disk    = NULL;
    int cmdrc                  = 0;

    _OSBASE_TRACE(1,("--- %s CMPI GetInstance() called",_ClassName));

    name = CMGetKey( cop, "InstanceID", &rc).value.string;
    if (name) {
      c_name = CMGetCharPtr(name);
    } else {
      c_name = "";
    }
    disk_id = strrchr(c_name,'_');
    
    if (disk_id == NULL || strlen(disk_id+1) <= 0) {
        CMSetStatusWithChars( _broker, &rc, 
                              CMPI_RC_ERR_FAILED, "Could not get disk name." ); 
        _OSBASE_TRACE(1,("--- %s CMPI GetInstance() failed for %s : %s",_ClassName,c_name,CMGetCharPtr(rc.msg)));
        return rc;
    }

    disk_id += 1;
    cmdrc = get_disk_data(disk_id, &disk);

    if (cmdrc == 0 || disk == NULL) {
        CMSetStatusWithChars( _broker, &rc, 
                              CMPI_RC_ERR_NOT_FOUND, "disk does not exist"); 
        _OSBASE_TRACE(1,("--- %s CMPI GetInstance() exited : %s",_ClassName,CMGetCharPtr(rc.msg)));
        if (disk) free_disk(disk);
        return rc;
    }

    ci = _makeInst_BlockStorageStatisticalData(_broker, ctx, cop, properties, &rc, disk);

    if (disk) free_disk(disk);

    CMReturnInstance( rslt, ci );
    CMReturnDone(rslt);
    _OSBASE_TRACE(1,("--- %s CMPI GetInstance() exited",_ClassName));
    return rc;
}

CMPIStatus OSBase_BlockStorageStatisticalDataProviderCreateInstance(CMPIInstanceMI * mi, 
                                                                    const CMPIContext * ctx, 
                                                                    const CMPIResult * rslt, 
                                                                    const CMPIObjectPath * cop, 
                                                                    const CMPIInstance * ci) {
    CMPIStatus rc = {CMPI_RC_OK, NULL};

    _OSBASE_TRACE(1,("--- %s CMPI CreateInstance() called",_ClassName));

    CMSetStatusWithChars( _broker, &rc, 
                          CMPI_RC_ERR_NOT_SUPPORTED, "CIM_ERR_NOT_SUPPORTED" ); 

    _OSBASE_TRACE(1,("--- %s CMPI CreateInstance() exited",_ClassName));
    return rc;
}

CMPIStatus OSBase_BlockStorageStatisticalDataProviderSetInstance(CMPIInstanceMI * mi, 
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

CMPIStatus OSBase_BlockStorageStatisticalDataProviderDeleteInstance(CMPIInstanceMI * mi, 
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

CMPIStatus OSBase_BlockStorageStatisticalDataProviderExecQuery(CMPIInstanceMI * mi, 
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
/*                        Method Provider Interface                           */
/* ---------------------------------------------------------------------------*/


CMPIStatus OSBase_BlockStorageStatisticalDataProviderMethodCleanup( CMPIMethodMI * mi, 
                                                                    const CMPIContext * ctx,
								    CMPIBoolean terminate) {
  _OSBASE_TRACE(1,("--- %s CMPI MethodCleanup() called",_ClassName));
  _OSBASE_TRACE(1,("--- %s CMPI MethodCleanup() exited",_ClassName));
  CMReturn(CMPI_RC_OK);
}

CMPIStatus OSBase_BlockStorageStatisticalDataProviderInvokeMethod( CMPIMethodMI * mi,
                                                                   const CMPIContext * ctx,
                                                                   const CMPIResult * rslt,
                                                                   const CMPIObjectPath * ref,
                                                                   const const char * methodName,
                                                                   const CMPIArgs * in,
                                                                   CMPIArgs * out) {
  CMPIString * class = NULL; 
  CMPIStatus   rc    = {CMPI_RC_OK, NULL};

  _OSBASE_TRACE(1,("--- %s CMPI InvokeMethod() called",_ClassName));

  class = CMGetClassName(ref, &rc);

  if( strcasecmp(CMGetCharPtr(class), _ClassName) == 0 && 
      strcasecmp("ResetSelectedStats",methodName) == 0 ) {  
    CMSetStatusWithChars( _broker, &rc, 
			  CMPI_RC_ERR_NOT_SUPPORTED, methodName ); 
  }
  else {
    CMSetStatusWithChars( _broker, &rc, 
			  CMPI_RC_ERR_NOT_FOUND, methodName ); 
  }
 
  _OSBASE_TRACE(1,("--- %s CMPI InvokeMethod() exited",_ClassName));
  return rc;
}

/* ---------------------------------------------------------------------------*/
/*                              Provider Factory                              */
/* ---------------------------------------------------------------------------*/

CMInstanceMIStub(OSBase_BlockStorageStatisticalDataProvider, 
                 OSBase_BlockStorageStatisticalDataProvider, 
                 _broker, 
                 CMNoHook);

CMMethodMIStub( OSBase_BlockStorageStatisticalDataProvider,
                OSBase_BlockStorageStatisticalDataProvider, 
                _broker, 
                CMNoHook);


/* ---------------------------------------------------------------------------*/
/*          end of cmpiOSBase_BlockStorageStatisticalDataProvider                      */
/* ---------------------------------------------------------------------------*/

