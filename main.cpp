#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <grp.h>
#include <pwd.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

#define ERROR -1
#define NOINFO "-"
#define RIGHTS_SYMBS_NUM 3
#define RIGHTS_TYPES_NUM 9
#define CUR_DIR "."


char getType(struct stat * status){

    //если это просто файл
    if(status->st_mode == S_IFDIR){
        return ('d');
    }

    //если файл является каталогом
    if(status->st_mode == S_IFREG){
        return ('-');
    }

    //все остальные случаи
    return ('?');
}

char* getUserName(struct stat* status){

    struct passwd* userInfo = getpwuid(status->st_uid);

    if(userInfo == NULL){
        perror("Get owner info");
        return NOINFO;
    }

    return userInfo->pw_name;
}

char* getGroupName(struct stat* status){

    struct group* groupInfo = getgrgid(status->st_gid);

    if(groupInfo == NULL){
        perror("Get user info");
        return NOINFO;
    }

    return groupInfo->gr_name;
}

off_t getFileSize(struct stat* status){

    //если это обычный файл,то выводим размер
    if(status->st_mode == S_IFDIR){
        return status->st_size;
    }
}

char* getFileTimestamp(struct stat* status){

    //требуется использовать ctime
    char* time = ctime(&status->st_mtime);

    if (time == NULL) {
        perror("Get time");
        exit(EXIT_FAILURE);
    }

    return time;
}


char* getFileName(char* filepath)
{
    int filenameIndex = strlen(filepath) - 1;

    while (filepath[filenameIndex] != '/' && filenameIndex != 0) {
        filenameIndex--;
    }

    if (filenameIndex != 0) {
        filenameIndex++;
    }

    return &filepath[filenameIndex];
}

nlink_t getLinksNum(struct stat* status)
{
    return status->st_nlink;
}

char* getAccessRights(struct stat* status, char* accessRights){
    char rights_symbols[RIGHTS_SYMBS_NUM] = { 'r', 'w', 'x' };
    int rights_types[RIGHTS_TYPES_NUM] = { S_IRUSR, S_IWUSR, S_IXUSR, S_IRGRP, S_IWGRP, S_IXGRP, S_IROTH, S_IWOTH, S_IXOTH };

    for (int right_index = 0; right_index < RIGHTS_TYPES_NUM; right_index++) {
        if (status->st_mode & rights_types[right_index]) {
            accessRights[right_index] = rights_symbols[right_index % RIGHTS_SYMBS_NUM];
        }
    }

    return accessRights;
}


void printFileInfo(char* fileName){
    struct stat status;

    //Функция stat возвращает структуру с информацией о файле, указанном в аргументе pathname
    if(stat(fileName, &status) == ERROR){
        perror(fileName);
        return;
    }

    char access_rights[RIGHTS_TYPES_NUM + 1];
    for (int i = 0; i < RIGHTS_TYPES_NUM; ++i) {
        access_rights[i] = '-';
    }

    getAccessRights(&status, access_rights);

    printf("%c%s   %d %s %s    %lld %.19s %s\n", getType(&status), access_rights, getLinksNum(&status),
           getUserName(&status), getGroupName(&status), getFileSize(&status),
           getFileTimestamp(&status), getFileName(fileName));
}

int main(int argc, char** argv) {

    if (argc < 2) {
        printFileInfo(CUR_DIR);
        return EXIT_SUCCESS;
    }

    int filename_index;

    for (filename_index = 1; filename_index < argc; filename_index++) {
        printFileInfo(argv[filename_index]);
    }
    return 0;
}
