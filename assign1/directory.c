#include "directory.h"
#include "inode.h"
#include "diskimg.h"
#include "file.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

/**
 * Looks up the specified name (name) in the specified directory (dirinumber).  
 * If found, return the directory entry in space addressed by dirEnt.  Returns 0 
 * on success and something negative on failure. 
 */
int directory_findname(struct unixfilesystem *fs, const char *name, int dirinumber, struct direntv6 *dirEnt) {
    // get dir inode
    struct inode inp;
    if(inode_iget(fs, dirinumber, &inp) == -1) return -1;

    // check if the inode is allocated and is dir
    if(!(inp.i_mode & IALLOC) || (inp.i_mode & IFMT) != IFDIR) return -1;

    int size = inode_getsize(&inp);
    assert(size % sizeof(struct direntv6) == 0); // dir should have integer dir entry

    int blockAm = size % 512 == 0 ? size/512 : size/512+1; // get the amount of blocks

    struct direntv6 buf[32]; // block size/direntv6 size = 512/16 = 32, max size to store

    struct direntv6 *dir = (struct direntv6 *) buf; // change to a pointer

    int i;
    for(i = 0; i < blockAm; i++){ // check every block

        int byteGot = file_getblock(fs, dirinumber, i, dir);
        int entryAm = byteGot/ sizeof(struct direntv6); // get the amount of entry;

        int j;
        for(j = 0; j < entryAm; j++){ // check every entry
            if(strcmp(dir[j].d_name, name) == 0) { // compare name
                *dirEnt = dir[j];
                return 0;
            }
        }
    }


    return -1;
}
