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
void searchFiles(char *name)            //should name be const?
{
    DIR *dir;
    struct dirent *entry;
    
    if (!(dir = opendir(name))) {
        fprintf(stderr, "Warning: Could not open directory %s: %s\n", name, strerror(errno));
        return;
    }
    
    while ((entry = readdir(dir)) != NULL) {
        //check if entry is another directory
        if (entry->d_type == DT_DIR) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            char path[1024];
            sprintf(path, "%s/%s", name, entry->d_name);
            printf("[%s]\n", entry->d_name);
            searchFiles(path);
            continue;                                           //does this work???
        }
        //check for symlink
        else if (entry->d_type == DT_LNK) {
            //find what it resolves to target or duplicate
            //if duplicate, get contents (path) of file
            //print path, "symlink resolves to...", and possibly contents
        }
        else if (entry->d_type == DT_REG) {
            //run stats on file
            struct stat st;
            if (stat(name,&st) < 0) {
                fprintf(stderr, "Warning: Could not run stat on file %s: %s\n", name, strerror(errno));
            }
            ino_t ino = st.st_ino; //inode number
            mode_t mode = st.st_mode; //mode
            int o_permissions = (mode & S_IROTH); //other read permissions
            char* perm_string;
            if (o_permissions == 1) {
                perm_string = "OK READ by OTHER";
            }
            else {
                perm_string = "NOT READABLE by OTHER";
            }
            off_t size = st.st_size; //size in bytes
            
            //check for hardlink
            if (ino == inodenum) {
                //hardlink: print path, "hardlink", permissions
                printf("%s\tHARD LINK TO TARGET\t%s\n",name,perm_string);
            }
            
            //check for duplicate
            else if ((size == filesize) && (compareFiles(name) == 1)) {
                //duplicate: print path, "duplicate of target", nlink, permissions
                nlink_t links = st.st_nlink;
                printf("%s\tDUPLICATE OF TARGET\tnlink=%d\t%s\n",name,links,perm_string);
            }
        }
        else {
            fprintf(stderr, "Debug: Directory entry %s not a directory, regular file, or symlink, skipping\n", name);
        }
        return;
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
    
    searchFiles(dirname);
    
    
    
    //ino_t test = 32878726;
    //printf("inode number:%llu\nfile size:%lld\n",inodenum,filesize);
    //printf("%d\n", (inodenum==test));
    
    return 0;
}