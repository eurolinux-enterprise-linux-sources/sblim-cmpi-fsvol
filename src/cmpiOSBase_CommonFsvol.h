#ifndef _CMPIOSBASE_COMMONFSVOL_H_
#define _CMPIOSBASE_COMMONFSVOL_H_

/*
 * cmpiOSBase_CommonFsvol.h
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
 * This file defines the interfaces for the factory implementation of the
 * CIM class -.
 *
*/


/* ---------------------------------------------------------------------------*/

#include "cmpidt.h"
#include "OSBase_CommonFsvol.h"

/* ---------------------------------------------------------------------------*/


/* ---------------------------------------------------------------------------*/
// get file system name for a certain directory

int get_fs_of_dir( const char * dir, char ** fscname, char ** fsname);

/* ---------------------------------------------------------------------------*/

#endif

