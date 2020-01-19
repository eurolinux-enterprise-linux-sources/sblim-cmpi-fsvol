#ifndef _LINUX_BLOCKSTORAGESTATISTICALDATA_H_
#define _LINUX_BLOCKSTORAGESTATISTICALDATA_H_

/*
 * Linux_BlockStorageStatisticalData.h
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
 * Description: 
 * This file defines the interfaces for the resource access implementation 
 * of the CIM class Linux_BlockStorageStatisticalData
 * 
*/

/* ---------------------------------------------------------------------------*/

//#include <pthread.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

//static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

/* ---------------------------------------------------------------------------*/

/** the data of one single drive */
struct disk_data {
    char *id;
    unsigned long long kb_read;
    unsigned long long kb_transfered;
    unsigned long long read_ios;
    unsigned long long write_ios;
    unsigned long long total_ios;
    unsigned long long io_time;
};

/** the list of all known drives */
struct disk_list {
    struct disk_data *disk;
    struct disk_list *next;
};

/**
 * Returns a list of all currently known disk drives
 * This function allocates memory which must be freed by
 * free_disk_list() (only if return value is not 0).
 * @param list the list is returned using this parameter
 * @return amount of available disk drives, 0 if no drive
 * was found.
 */

int enum_all_disks(struct disk_list **list);

/**
 * Returns the current values of a specific disk drive.
 * This function allocates memory which must be freed by
 * free_disk() (only if return value is 1).
 * @param id a unique indentifier of the disk
 * @param disk the current data is returned using this parameter 
 * @return 1 on success, 0 otherwise
 */
int get_disk_data(char *id, struct disk_data **disk);

/**
 * Frees the memory allocated by enum_all_disks
 * @param list the list of all known drives returned by enum_all_disks
 * @see enum_all_disks
 */
void free_disk_list(struct disk_list *list);


/**
 * Frees the memory allocated by a drive structure
 * @param drive the structure to free
 */
void free_disk(struct disk_data *disk);

/**
 * Resets the given counters.
 * @param counters the counters to reset
 */
void reset_counters();



/* ---------------------------------------------------------------------------*/

#ifdef __cplusplus
   }
#endif

/* ---------------------------------------------------------------------------*/

#endif


