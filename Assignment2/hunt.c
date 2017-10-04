//  hunt.c
//  OS_hw2
//  10/8/17

#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
//#include <unistd.h>

//Global Variables - stats for target file
ino_t inodenum;
off_t filesize;

//Function to compare contents of files
int compareFiles(char* path) {
    return 1;
}

//Recursive searching function
void searchFiles(char *name) {
    DIR *dir;
    struct dirent *entry;
    
    if (!(dir = opendir(name))) {
        fprintf(stderr, "Warning: Could not open directory %s: %s\n", name, strerror(errno));
        return;
    }
    
    while ((entry = readdir(dir)) != NULL) {
        //current path
        char path[1024];
        sprintf(path, "%s/%s", name, entry->d_name);
        
        //run stat on entry
        struct stat st;
        if (stat(path,&st) < 0) {
            fprintf(stderr, "Warning: Could not run stat on file %s: %s\n", name, strerror(errno));
        }
        //get stats
        ino_t ino = st.st_ino; //inode number
        mode_t mode = st.st_mode; //mode
        off_t size = st.st_size; //size in bytes
        
        //check if entry is another directory
        if (entry->d_type == DT_DIR) {                      //should i check type of entry using mode???
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            searchFiles(path);
        }
        //check for symlink
        else if (entry->d_type == DT_LNK) {                     //how to make symlink? alias doesn't work
            //find what it resolves to target or duplicate
            //if duplicate, get contents (path) of file
            //print path, "symlink resolves to...", and possibly contents
            printf("\t%s\tFOUND SYMLINK",path);
        }
        //if regular file, check for hardlink or duplicate
        else if (entry->d_type == DT_REG) {
            //get permissions
            int o_permissions = (mode & S_IROTH); //other read permissions          //doesn't work? check with chmod?
            char* perm_string;
            if (o_permissions == 1) {
                perm_string = "OK READ by OTHER";
            }
            else {
                perm_string = "NOT READABLE by OTHER";
            }

            //check for hardlink
            if (ino == inodenum) {
                //print path, "hardlink", permissions
                printf("%s\tHARD LINK TO TARGET\t%s\n",path,perm_string);
            }
            
            //check for duplicate
            else if ((size == filesize) && (compareFiles(path) == 1)) {
                //print path, "duplicate of target", nlink, permissions
                nlink_t links = st.st_nlink;
                printf("%s\tDUPLICATE OF TARGET\tnlink=%d\t%s\n",path,links,perm_string);
            }
        }
        else {
            fprintf(stderr, "Debug: Directory entry %s not a directory, regular file, or symlink, skipping\n", name);
        }
    }
    closedir(dir);
}

//main loop
int main(int argc, char**argv) {
    
    char* targetname = argv[1];
    char* dirname = argv[2];
    DIR* dir;
    struct stat st;
    
    if (stat(targetname,&st) < 0) {
        fprintf(stderr, "FATAL ERROR: Could not run stat on target file '%s': %s\n", targetname, strerror(errno));
        return -1;
    }
    else if (!(dir = opendir(dirname))) {
        fprintf(stderr, "FATAL ERROR: Could not open directory '%s': %s\n", dirname, strerror(errno));
        return -1;
    }
    inodenum = st.st_ino;
    filesize = st.st_size;
    nlink_t links = st.st_nlink;
    printf("Target inode number:%llu\nTarget file size:%lld\nTarget nlink:%hu\n",inodenum,filesize,links);
    searchFiles(dirname);
    
    return 0;
}