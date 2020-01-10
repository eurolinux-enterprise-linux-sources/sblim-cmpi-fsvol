#!/bin/sh

# test sblim-cmpi-fsvol package

SCRIPT_PATH=`dirname ${BASH_SOURCE}`

#******************************************************************************#

export SBLIM_TESTSUITE_RUN=1;

#******************************************************************************#

declare -a CLASSNAMES[];
CLASSNAMES=([0]=Linux_Ext2FileSystem [1]=Linux_Ext3FileSystem \
[2]=Linux_ReiserFileSystem [3]=Linux_NFS \
[4]=Linux_HostedFileSystem [5]=Linux_BootOSFromFS \
[6]=Linux_BlockStorageStatisticalData)

declare -i max=6;
declare -i i=0;

cd $SCRIPT_PATH
while(($i<=$max))
do
  . $SCRIPT_PATH/run.sh ${CLASSNAMES[$i]} $1 $2 $3 $4 || exit 1;
  i=$i+1;
done
