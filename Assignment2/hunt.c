//  hunt.c
//  OS_hw2
//  10/8/17

#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

//Global Variables - stats for target file
ino_t target_ino;
off_t target_size;

//Function to compare contents of files
//returns 1 if identical
//returns 0 if not identical
//returns -1 on error
int compareFiles(char* path) {
    //fprintf(stderr, "Error comparing '%s' to target file, skipping\n", path);
    return 1;
}

//Recursive searching function
void searchFiles(char *directory) {
    DIR *dir;
    struct dirent *entry;
    
    if (!(dir = opendir(directory))) {
        fprintf(stderr, "Warning: Could not open directory %s: %s\n", directory, strerror(errno));
        return;
    }
    
    while ((entry = readdir(dir)) != NULL) {

        //current path
        char path[1024];
        sprintf(path, "%s/%s", directory, entry->d_name);
        
        //if entry is another directory, recursively search
        if (entry->d_type == DT_DIR) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            searchFiles(path);
        }
        //not directory
        else {
            
            //run stat on entry
            struct stat st;
            if (stat(path,&st) < 0) {
                fprintf(stderr, "Warning: Could not run stat on file %s: %s\n", directory, strerror(errno));
            }
            
            //get stats
            ino_t ino = st.st_ino; //inode number
            off_t size = st.st_size; //size in bytes
            
            //symlink
            if (entry->d_type == DT_LNK) {                     //how to test symlink? alias doesn't work
                //resolves to target
                if (ino == target_ino) {
                    printf("%s\tSYMLINK RESOLVES TO TARGET\n",path);
                }
                //resolves to duplicate
                else if (size == target_size && compareFiles(path) == 1) {
                    //find contents of link
                    char link[1024];
                    if (readlink(path,link,sizeof(link)) < 0) {
                        fprintf(stderr, "Warning: Could check contents of symlink %s: %s\n", path, strerror(errno));
                    }
                    else {
                        printf("%s\tSYMLINK (%s) RESOLVES TO DUPLICATE\n",path,link);
                    }
                }
            }
            //regular file
            else if (entry->d_type == DT_REG) {
                //get permissions
                mode_t mode = st.st_mode;
                int o_permissions = (mode & S_IROTH); //'other' read permissions          //doesn't work? check with chmod?
                char* perm_string;
                if (o_permissions == 1) {
                    perm_string = "OK READ by OTHER";
                }
                else {
                    perm_string = "NOT READABLE by OTHER";
                }
                
                //check for hardlink
                if (ino == target_ino) {
                    printf("%s\tHARD LINK TO TARGET\t%s\n",path,perm_string);
                }
                
                //check for duplicate
                else if (size == target_size && compareFiles(path) == 1) {
                    nlink_t links = st.st_nlink;
                    printf("%s\tDUPLICATE OF TARGET\tnlink=%d\t%s\n",path,links,perm_string);
                }
            }
            //other file type
            else {
                fprintf(stderr, "Directory entry %s not a directory, regular file, or symlink, skipping\n", path);
            }
        }
    }
    closedir(dir);
}

//main loop
int main(int argc, char**argv) {
    
    char* targetname = argv[1];
    char* starting_directory = argv[2];
    DIR* dir;
    struct stat st;
    
    if (stat(targetname,&st) < 0) {
        fprintf(stderr, "FATAL ERROR: Could not run stat on target file '%s': %s\n", targetname, strerror(errno));
        return -1;
    }
    else if (!(dir = opendir(starting_directory))) {
        fprintf(stderr, "FATAL ERROR: Could not open directory '%s': %s\n", starting_directory, strerror(errno));
        return -1;
    }
    target_ino = st.st_ino;
    target_size = st.st_size;
    //nlink_t links = st.st_nlink;
    //printf("Target inode number:%llu\nTarget file size:%lld\nTarget nlink:%hu\n",target_ino,target_size,links);
    searchFiles(starting_directory);
    
    return 0;
}