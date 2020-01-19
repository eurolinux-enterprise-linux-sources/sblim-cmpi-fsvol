#ifndef _CMPIOSBASE_BLOCKSTORAGESTATISTICALDATA_H_
#define _CMPIOSBASE_BLOCKSTORAGESTATISTICALDATA_H_

/*
 * cmpiOSBase_BlockStorageStatisticalData.h
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
 * This file defines the interfaces for the factory implementation of the
 * CIM class Linux_BlockStorageStatisticalData.
 *
*/


/* ---------------------------------------------------------------------------*/

#include "cmpidt.h"
#include "OSBase_BlockStorageStatisticalData.h"

/* ---------------------------------------------------------------------------*/

static char * _ClassName = "Linux_BlockStorageStatisticalData";
static const char * _INSTANCE_PREFIX = "Linux:";
static int _INSTANCE_PREFIX_LENGTH = 6;

/* ---------------------------------------------------------------------------*/


/* method to create a CMPIObjectPath of this class                            */

CMPIObjectPath * _makePath_BlockStorageStatisticalData(const CMPIBroker * _broker,
                                                       const CMPIContext * ctx, 
                                                       const CMPIObjectPath * cop,
                                                       CMPIStatus * rc,
                                                       const struct disk_data *disk);
    

/* method to create a CMPIInstance of this class                              */

CMPIInstance * _makeInst_BlockStorageStatisticalData(const CMPIBroker * _broker,
                                                     const CMPIContext * ctx, 
                                                     const CMPIObjectPath * cop,
                                                     const char ** properties,
                                                     CMPIStatus * rc,
                                                     const struct disk_data *disk);


/* ---------------------------------------------------------------------------*/

#endif

