
#include "pathname.h"
#include "directory.h"
#include "inode.h"
#include "diskimg.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>


/**
 * Returns the inode number associated with the specified pathname.  This need only
 * handle absolute paths.  Returns a negative number (-1 is fine) if an error is 
 * encountered.
 */
int pathname_lookup(struct unixfilesystem *fs, const char *pathname) {
    int dirinumber = 1; // home
    struct direntv6 dirEnt; // to store
    char pathname_copy[512];
    strcpy(pathname_copy, pathname);

    if(strcmp(pathname, "/") == 0) return 1; // root, inumber is 1

    char *p = strtok(pathname_copy, "/"); //split the name of first dir

    if(p == NULL) return -1;

    while(p != NULL){
        if(directory_findname(fs, p, dirinumber, &dirEnt) <0) return -1; //look for entry of inode with name p

        dirinumber = dirEnt.d_inumber;

        p = strtok(NULL, "/"); // get the name of next dir
    }



    return dirinumber;
}
