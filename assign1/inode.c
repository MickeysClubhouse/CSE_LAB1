#include <stdio.h>
#include <assert.h>

#include "inode.h"
#include "diskimg.h"


/**
 * Fetches the specified inode from the filesystem (to inp).
 * Returns 0 on success, -1 on error.  
 */
int inode_iget(struct unixfilesystem *fs, int inumber, struct inode *inp) {
    if(inumber < 1) return -1; // inumber no less than 1

    // one block can store 512/32 = 16 inodes,
    // and since the first inode in the first inode block has inumber 1, no number 0,
    // so the offset is start index + (int)((inumber-1)/16)
    struct inode inodes[16]; // inodes is used to put inodes in specified sector
    if(diskimg_readsector(fs->dfd, INODE_START_SECTOR + (inumber-1)/16 , inodes) == DISKIMG_SECTOR_SIZE){
        *inp = inodes[(inumber-1)%16]; // inp is the (inumber-1)%16+1 th inode in inodes
        return 0;
    }
    return -1;
}


/**
 * Given an index(blockNum) of a file block, retrieves the file's actual block number
 * of from the given inode.
 *
 * Returns the disk block number on success, -1 on error.  
 */
int inode_indexlookup(struct unixfilesystem *fs, struct inode *inp, int blockNum) {
    if(blockNum < 0 || blockNum > 67328) return -1; // 67329 blocks at most

    if(inp->i_mode & ILARG) { // file is large
        // indirect blocks, 2 bytes per index
        if(blockNum < 1792){ // 256 * 7 = 1792, the first 7 indirect blocks, not a doubly indirect block
            uint16_t index[256]; // numbers of 256 blocks which contains actual block number
            uint16_t b_index = blockNum/256; // index of the block containing the indexs
            if(diskimg_readsector(fs->dfd, inp->i_addr[b_index], index) == DISKIMG_SECTOR_SIZE){
                return index[blockNum%256];
            }
        } else { //doubly indirect block
            uint16_t index1[256]; // index of index
            uint16_t index2[256]; // index of block
            if(diskimg_readsector(fs->dfd, inp->i_addr[7], index1) == DISKIMG_SECTOR_SIZE){ // find the blocks with double indirect block numbers
                int b_index = blockNum/256 - 7; // compute the block index in index1
                if(diskimg_readsector(fs->dfd, index1[b_index], index2) == DISKIMG_SECTOR_SIZE){
                    return index2[blockNum%256];
                }
            }
        }
    } else if(blockNum <= 7) { // small file
        return inp->i_addr[blockNum];
    }

    return -1;
}


/**
 * Computes the size in bytes of the file identified by the given inode
 */
int inode_getsize(struct inode *inp) {
    return (inp->i_size0 << 16) | inp->i_size1; // 拼接size，高位左移16位
}
