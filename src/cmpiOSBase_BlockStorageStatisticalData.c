/*
 * cmpiOSBase_BlockStorageStatisticalData.c
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
 * Interface Type : Common Magabeablity Programming Interface ( CMPI )
 *
 * Description:
 * This is the factory implementation for creating instances of CIM
 * class Linux_BlockStorageStatisticalData.
 *
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cmpidt.h"
#include "cmpimacs.h"

#include "cmpiOSBase_Common.h"
#include "OSBase_Common.h"

#include "cmpiOSBase_BlockStorageStatisticalData.h"

/* ---------------------------------------------------------------------------*/
/* private declarations                                                       */

/* ---------------------------------------------------------------------------*/


/* ---------------------------------------------------------------------------*/
/*                            Factory functions                               */
/* ---------------------------------------------------------------------------*/

/* ---------- method to create a CMPIObjectPath of this class ----------------*/

CMPIObjectPath * _makePath_BlockStorageStatisticalData(const CMPIBroker * _broker,
                                                       const CMPIContext * ctx, 
                                                       const CMPIObjectPath * ref,
                                                       CMPIStatus * rc,
                                                       const struct disk_data *disk) {
    CMPIObjectPath * op           = NULL;
    char           * instanceID   = NULL;
    char           * hostname     = NULL;

    _OSBASE_TRACE(2,("--- _makePath_BlockStorageStatisticalData() called"));

    op = CMNewObjectPath( _broker, CMGetCharPtr(CMGetNameSpace(ref,rc)), 
                          _ClassName, rc );
    if (CMIsNullObject(op)) {
        CMSetStatusWithChars( _broker, rc, 
                              CMPI_RC_ERR_FAILED, "Create CMPIObjectPath failed." ); 
        _OSBASE_TRACE(2,("--- _makePath_BlockStorageStatisticalData() failed : %s",CMGetCharPtr(rc->msg)));
        goto exit; 
    }

    /* keys are unique identifiers for an instance */
    // Linux:<hostname>_<disk_id>
    hostname = get_system_name();
    instanceID = calloc(_INSTANCE_PREFIX_LENGTH + strlen(hostname) + 1 + strlen(disk->id) + 1, sizeof(char));
    strcpy(instanceID, _INSTANCE_PREFIX);
    strcat(instanceID, hostname);
    strcat(instanceID, "_");
    strcat(instanceID, disk->id);
    CMAddKey(op, "InstanceID", instanceID, CMPI_chars);

    if (instanceID) free(instanceID);

    exit:
    _OSBASE_TRACE(2,("--- _makePath_BlockStorageStatisticalData() exited"));
    return op;                
}

/* ----------- method to create a CMPIInstance of this class ----------------*/

CMPIInstance * _makeInst_BlockStorageStatisticalData(const CMPIBroker * _broker,
                                                     const CMPIContext * ctx, 
                                                     const CMPIObjectPath * ref,
                                                     const char ** properties,
                                                     CMPIStatus * rc,
                                                     const struct disk_data *disk) {
    CMPIObjectPath *  op         = NULL;
    CMPIInstance   *  ci         = NULL;
    const char     ** keys       = NULL;
    int               keyCount   = 0;
    char           *  instanceID = NULL;
    CMPIDateTime   *  dt         = NULL;
    char           *  hostname   = NULL;

    _OSBASE_TRACE(2,("--- _makeInst_BlockStorageStatisticalData() called"));

    op = CMNewObjectPath( _broker, CMGetCharPtr(CMGetNameSpace(ref,rc)), 
                          _ClassName, rc );

    if (CMIsNullObject(op)) {
        CMSetStatusWithChars( _broker, rc, 
                              CMPI_RC_ERR_FAILED, "Create CMPIObjectPath failed." );
        _OSBASE_TRACE(2,("--- _makeInst_BlockStorageStatisticalData() failed : %s",CMGetCharPtr(rc->msg))); 
        goto exit; 
    }

    ci = CMNewInstance( _broker, op, rc);
    if (CMIsNullObject(ci)) {
        CMSetStatusWithChars( _broker, rc, 
                              CMPI_RC_ERR_FAILED, "Create CMPIInstance failed." ); 
        _OSBASE_TRACE(2,("--- _makeInst_BlockStorageStatisticalData() failed : %s",CMGetCharPtr(rc->msg)));
        goto exit; 
    }

    /* set property filter 
    */
/*
    keys = calloc(7,sizeof(char*));
    keys[0] = strdup("CreationClassName");
    keys[1] = strdup("DeviceCreationClassName");
    keys[2] = strdup("DeviceID");
    keys[3] = strdup("Name");
    keys[4] = strdup("SystemCreationClassName");
    keys[5] = strdup("SystemName");
    keys[6] = NULL;

    CMSetPropertyFilter(ci,properties,keys);
    for( ;keys[keyCount]!=NULL;keyCount++) { free((char*)keys[keyCount]); }
    free(keys);
*/

    // create instanceID: Linux:<hostname>_<disk_id>
    hostname = get_system_name();
    instanceID = calloc(_INSTANCE_PREFIX_LENGTH + strlen(hostname) + 1 + strlen(disk->id) + 1, sizeof(char));
    strcpy(instanceID, _INSTANCE_PREFIX);
    strcat(instanceID, hostname);
    strcat(instanceID, "_");
    strcat(instanceID, disk->id);
    CMSetProperty(ci, "InstanceID", instanceID, CMPI_chars);

    if (instanceID) free(instanceID);

    /* from CIM_ManagedElement */
    CMSetProperty(ci, "Caption", _ClassName, CMPI_chars);
    CMSetProperty(ci, "Description", "statistical information about disk drives", CMPI_chars);
    CMSetProperty(ci, "ElementName", disk->id, CMPI_chars);

    /* from CIM_StatisticalData */
    // not supported
    CMSetProperty(ci, "StartStatisticTime", NULL, CMPI_dateTime);

    dt = CMNewDateTime(_broker,rc);
    
    CMSetProperty(ci, "StatisticTime", (CMPIValue*)&(dt), CMPI_dateTime);

    // there's no consistent time interval
    //CMSetProperty(ci, "SampleInterval", NULL, CMPI_uint64);

    /* from CIM_BlockStorageStatisticalData */
    CMSetProperty(ci, "ReadIOs",          (CMPIValue*)&(disk->read_ios),      CMPI_uint64);
    CMSetProperty(ci, "WriteIOs",         (CMPIValue*)&(disk->write_ios),     CMPI_uint64);
    CMSetProperty(ci, "TotalIOs",         (CMPIValue*)&(disk->total_ios),     CMPI_uint64);
    CMSetProperty(ci, "KBytesRead",       (CMPIValue*)&(disk->kb_read),       CMPI_uint64);
    CMSetProperty(ci, "KBytesTransfered", (CMPIValue*)&(disk->kb_transfered), CMPI_uint64);
    CMSetProperty(ci, "IOTimeCounter",    (CMPIValue*)&(disk->io_time),       CMPI_uint64);

    exit:
    _OSBASE_TRACE(2,("--- _makeInst_BlockStorageStatisticalData() exited"));
    return ci;
}



/* ---------------------------------------------------------------------------*/
/*          end of cmpiOSBase_BlockStorageStatisticalData.c                   */
/* ---------------------------------------------------------------------------*/

