/*
 * Linux_BlockStorageStatisticalData.c
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
 * This shared library provides resource access functionality for the class
 * Linux_BlockStorageStatisticalData. 
 * It is independent from any specific CIM technology.
*/

/* ---------------------------------------------------------------------------*/

#include "OSBase_BlockStorageStatisticalData.h"
#include "OSBase_Common.h" /* logging */

#include <unistd.h>
#include <errno.h>
#include <dirent.h>

#define CHECK_FCT_PARAM

#define HD_CMD "grep \"[hsv]d[a-z] \" /proc/partitions"
#define HD_CMD_26 "grep \"[hsv]d[a-z] \" /proc/diskstats"

#define STATS_FILE "/proc/partitions"
#define STATS_FILE_26 "/proc/diskstats"

// uncomment to exclude CD drives
//#define HD_ONLY 

/* ---------------------------------------------------------------------------*/
// private declarations

#define IDENTIFIER_LENGTH 255


/**
 * Allocates memory, retrieves data from the given char array 
 * and sets the values of a single disk_data item.
 * Prior to this it is checked if the given drive is really a disk
 * (and not a cdrom or other media) (can be changed using HD_ONLY define).
 * Currently only IDE disks are supported!
 * Returned memory has to be freed using free_disk().
 * 
 *@param ddisk one line of the file /proc/partitions (if kernel26==0)
 * or one line of the file /proc/diskstats (if kernel26==1)
 *@param kernel26 0 if passed data is formatted in kernel24 style, any other
 * value indicates that data has style of kernel26.
 * @return NULL if an error occured, otherwise a structure with the data retrieved
 * from ddata
 */
struct disk_data * create_disk_data(char *ddata, int kernel26);

/**
 * Checks if the given name identifies a disk or not. 
 * Only IDE disks are supported!
 * @param name the name to check
 * @return 1 if the device is a disk, 0 otherwise
 */ 
int is_disk(const char * name);


/* ---------------------------------------------------------------------------*/

/* linux disk statistics (read <linux-kernel-source>/Documentation/iostats.txt for details): 
 *
 * 2.4: /proc/partitions
 * major minor  #blocks name rio  rmerge rsect    ruse     wio  wmerge wsect     wuse   running   use   aveq
 *  3     0   39082680 hda 446216 784926 9550688 4382310 424847 312726 5922052 19310380  0    3376340 23705160
 *  3     1    9221278 hda1 35486 0 35496 38030 0 0 0 0 0 38030 38030
 *
 * 2.6 /proc/diskstats:
 * major minor name  rio rmerge   rsect    ruse    wio   wmerge wsect    wuse   running   use    aveq
 *  3    0     hda 446216 784926 9550688 4382310 424847 312726 5922052 19310380     0   3376340 23705160
 *  3    1     hda1 35486 38030 38030    38030
 *
 * not supported by this implementation
 * 2.6 sysfs:
 *  446216 784926 9550688 4382310 424847 312726 5922052 19310380 0 3376340 23705160
 *  35486    38030    38030    38030
 */
int enum_all_disks(struct disk_list **list) {

    struct disk_list *disk_list_item = NULL;
    int i = 0;
    int rc = 0;
    int total_disks = 0;
    char **hdout = NULL;
    int kernel26 = 0;
    FILE *stats26 = NULL;
    char *cmd = NULL;
    struct disk_data *dd = NULL;

    _OSBASE_TRACE(3,("enum_all_disks called"));

    stats26 = fopen(STATS_FILE_26,"r");
    if (stats26) {
        fclose(stats26);
        cmd = HD_CMD_26;
        kernel26 = 1;
    } else {
        cmd = HD_CMD;
    }
    rc = runcommand(cmd,NULL,&hdout,NULL);
    if (rc == 0 && hdout != NULL) {
        while (hdout[i]) {
            dd = create_disk_data(hdout[i],kernel26);
            if (dd != NULL) {
                if (total_disks == 0) {
                    disk_list_item = (struct disk_list *)calloc(1, sizeof(struct disk_list));
                    disk_list_item->disk = dd;
                    *list = disk_list_item;
                    //disk_list_item->next = NULL;

                } else {
                    disk_list_item->next = (struct disk_list *)calloc(1, sizeof(struct disk_list));
                    disk_list_item->next->disk = dd;
                    disk_list_item = disk_list_item->next;
                    //disk_list_item->next = NULL;
                }
                disk_list_item->next = NULL;

                total_disks++;
            }
            i++;
        }
    } else {
        _OSBASE_TRACE(1,("enum_all_discs: error while running %s",cmd));
    }

    freeresultbuf(hdout);

    _OSBASE_TRACE(3,("enum_all_disks exited"));

    return total_disks;
}

/**
 * Allocates memory and gets the data for the specified drive.
 * @param id the id of the drive
 * @param disk the data will be returned through this parameter.
 * The memory has to be freed using free_disk_data().
 * @return 1 if data is available and some memory has been assigned
 * to "disk", 0 otherwise.
 */
int get_disk_data(char *id, struct disk_data **disk) {

    int res = 0;
    int rc = 0;
    char *cmd;
    char **hdout = NULL;
    int kernel26 = 0;
    FILE *stats26 = NULL;

    _OSBASE_TRACE(3,("get_disk_data called"));

    int cmdlen = strlen(id)+31;
    cmd = malloc(cmdlen);
    strcpy(cmd, "grep \"");
    strcat(cmd, id);

    stats26 = fopen(STATS_FILE_26,"r");
    if (stats26) {
        fclose(stats26);
        strcat(cmd, " \" ");
        strcat(cmd, STATS_FILE_26);
        kernel26 = 1;
    } else {
        strcat(cmd, " \" ");
        strcat(cmd,STATS_FILE);
    }

    rc = runcommand(cmd,NULL,&hdout,NULL);
    if (rc == 0 && hdout != NULL) {
        *disk = create_disk_data(hdout[0],kernel26);
        if (*disk != NULL) {
            res = 1;
        }
    } else {
        _OSBASE_TRACE(1,("get_disk_data: could not run %s (%s)",cmd,hdout));
    }
    freeresultbuf(hdout);
    free(cmd);

    _OSBASE_TRACE(3,("get_disk_data exited"));

    return res;
}

struct disk_data * create_disk_data(char *ddisk, int kernel26) {

    long long rio = 0;
    long long rsect = 0;
    long long wio = 0;
    long long wsect = 0;
    long long use = 0;
    int sres = 0;
    int id_length = 0;
    char dname[IDENTIFIER_LENGTH];
    struct disk_data * res = NULL;

    _OSBASE_TRACE(3,("create_disk_data called"));

    if (kernel26) {
        sres = sscanf(ddisk,"%*d %*d %s %lld %*d %lld %*d %lld %*d %lld %*d %*d %lld %*d",
                      dname,&rio,&rsect,&wio,&wsect,&use);
    } else {
        sres = sscanf(ddisk,"%*d %*d %*d %s %lld %*d %lld %*d %lld %*d %lld %*d %*d %lld %*d",
                      dname,&rio,&rsect,&wio,&wsect,&use);
    }
    if (sres != 6 || sres == EOF) {
        _OSBASE_TRACE(1,("create_disk_data: could not get data, wrong kernel version or kernel config (CONFIG_BLK_STATS)"));
        return 0;
    }
#ifdef HD_ONLY
    if (is_disk(dname)) {
#endif
        res = (struct disk_data *)calloc(1,sizeof(struct disk_data)); // free in free_disk 
        id_length = strlen(dname);
        if (id_length > 0) {
            res->id = (char*)malloc(strlen(dname)+1); 
            strncpy(res->id, dname, id_length);
            *(res->id+strlen(dname)) = '\0';
        } else {
            res->id = '\0';
        }
        // one sect has a size of 512 bytes  
        res->kb_read = rsect / 2; 
        res->kb_transfered = (wsect + rsect) / 2; 
        res->read_ios = rio;
        res->write_ios = wio;
        res->total_ios = rio + wio;
        res->io_time = use;
        _OSBASE_TRACE(3,("name %s, rio %lld, rbw %lld, wio %lld, wbw %lld, use %lld",dname, rio,rsect*512,wio,wsect*512,use));
#ifdef HD_ONLY
    }
#endif
    _OSBASE_TRACE(3,("create_disk_data exited"));

    return res;
} 

void free_disk_list(struct disk_list *list) {

    struct disk_list *list_item = NULL;
    int count = 0;

    _OSBASE_TRACE(3,("free_disk_list called"));

    while (list != NULL) {
        if (list->disk) free_disk(list->disk);
        list_item = list;
        list = list->next;
        free(list_item);
        count++;
    }
    _OSBASE_TRACE(3,("free_disk_list exited"));
}

void free_disk(struct disk_data *disk) {

    _OSBASE_TRACE(3,("free_disk called"));

#ifdef CHECK_FCT_PARAM
    if (disk == NULL) {
        _OSBASE_TRACE(2,("free_disk: called with NULL arg"));      
        return;
    }
#endif
    if (disk->id) free(disk->id);
    free(disk);
    _OSBASE_TRACE(3,("free_disk exited"));
}

int is_disk(const char * name) {

    int res = 0;
    FILE *f = NULL;
    int length = strlen(name);
    char filename[length+17];
    char media[10];

    _OSBASE_TRACE(3,("is_disk called"));

    // /proc/ide/<name>/media
    strcpy(filename,"/proc/ide/");
    strcat(filename,name);
    strcat(filename,"/media");
    if ((f = fopen(filename,"r")) != NULL) {

        fgets(media,9,f);
        if (strncmp(media,"disk",4) == 0) {
            res = 1;
        } else {
            _OSBASE_TRACE(2,("is_disk: %s is not a disk drive, type is %s - ",name,media));
        }
        fclose(f);
    } else {
        _OSBASE_TRACE(1,("is_disk: could not open %s: %s",filename,strerror(errno)));
    }

    _OSBASE_TRACE(3,("is_disk ended: res=%d",res));

    return res;
}

#warning TODO: implement reset_counters
void reset_counters() {

    // resetting counters not yet supported
    //pthread_mutex_lock(&mutex);
    // nothing
    //pthread_mutex_unlock(&mutex);
}

/* ---------------------------------------------------------------------------*/
/*          end of OSBase_BlockStorageStatisticalData.c                       */
/* ---------------------------------------------------------------------------*/





