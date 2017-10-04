//  hunt.c
//  OS_hw2
//  10/8/17

#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

//Global Variables - stats for target file
ino_t inodenum;
off_t filesize;

//Function to compare contents of files
int compareFiles(char* path) {
    return 1;
}

//Recursive searching function
void searchFiles(const char *name, int indent)
{
    DIR *dir;
    struct dirent *entry;
    
    if (!(dir = opendir(name))) {
        fprintf(stderr, "Warning: Could not open directory '%s': %s\n", name, strerror(errno)); //need to specify difference between end of path and failure to open
        return;
    }
    
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            char path[1024];
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);
            printf("%*s[%s]\n", indent, "", entry->d_name);
            searchFiles(path, indent + 2);
        }
        else {
            printf("%*s- %s\n", indent, "", entry->d_name);
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
    
    //searchFiles(dirname,0);
    
    
    
    //ino_t test = 32878726;
    //printf("inode number:%llu\nfile size:%lld\n",inodenum,filesize);
    //printf("%d\n", (inodenum==test));
    
    return 0;
}