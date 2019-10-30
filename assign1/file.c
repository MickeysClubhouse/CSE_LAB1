#include <stdio.h>
#include <assert.h>

#include "file.h"
#include "inode.h"
#include "diskimg.h"

/**
 * Fetches the specified file block from the specified inode.
 * Returns the number of valid bytes in the block, -1 on error.
 */
int file_getblock(struct unixfilesystem *fs, int inumber, int blockNum, void *buf) {
    struct inode inp;
    if(inode_iget(fs, inumber, &inp) == -1) return -1; // get inode

    int block;
    if((block = inode_indexlookup(fs, &inp, blockNum)) < 0) return -1; // use inode to get block number


    if(diskimg_readsector(fs->dfd, block, buf) == DISKIMG_SECTOR_SIZE){ // read the content of the block
        int size = inode_getsize(&inp);

        // compute how may blocks the file includes, except indirect blocks
//        int num;
//        if(size % 512 == 0) num = size/512;
//        else num = size/512 + 1;
//
//        if(blockNum + 1 > size) return -1; //
//        return num == blockNum + 1 ? size%512 : 512; // if file include n blocks, then if blockNum = n-1, the block is the last one


        // theoretically equals to the code annotated above...not sure...
        if(size%512 == 0) return 512;

        if(blockNum == size/512) return size%512;

        return 512;
    }

    return -1;
}
