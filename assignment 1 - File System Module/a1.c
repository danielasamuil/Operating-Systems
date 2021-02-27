#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include<string.h> 

void list(const char *pathname, int OK, bool success, const char *filtering, int permissions)
{
    DIR *directory = NULL;
    struct dirent *files = NULL;
    struct stat s;

    directory = opendir(pathname);

    if (directory == NULL)
    {
        printf("ERROR\ninvalid directory path\n");
        exit(1);
    }
    else
        if (success)
        {
            printf("SUCCESS\n");
            success = false;
        }

    while ((files = readdir(directory)) != NULL)
        if (strncmp(files->d_name, ".",1) != 0 && strncmp(files->d_name, "..",2) != 0)
        {
            char* path;

            path = (char*)malloc(sizeof(char) * strlen(pathname));

            if (strcmp(filtering, "") == 0 && permissions==0) 
            {
                snprintf(path, 512, "%s/%s", pathname, files->d_name);
                lstat(path, &s);
                printf("%s\n", path);
                free(path);
            }
            else if (strlen(files->d_name) >= strlen(filtering) && permissions == 0) //name_ends_with
                {
                    snprintf(path, 512, "%s/%s", pathname, files->d_name);
                    lstat(path, &s);
                    if ((S_ISDIR(s.st_mode) || S_ISREG(s.st_mode)) && strcmp(&files->d_name[strlen(files->d_name) - strlen(filtering)], filtering) == 0)
                    {
                        printf("%s\n", path);
                    }
                    free(path);
                }
            else if (strcmp(filtering, "") == 0 && permissions==1) // has_perm_write 
            {
                snprintf(path, 512, "%s/%s", pathname, files->d_name);
                lstat(path, &s);
                if ((S_ISDIR(s.st_mode) || S_ISREG(s.st_mode)) && s.st_mode & S_IWUSR)
                    printf("%s\n", path);
                free(path);
            }

            if (S_ISDIR(s.st_mode) && OK==1)
                list(path,1,false,filtering,permissions);
        }

            closedir(directory);
}

void parse(const char* path) {
    int fd;

    fd = open(path, O_RDONLY);

    if (fd == -1)
    {
        printf("ERROR: You cannot open this file");
        close(fd);
        exit(1);
    }

    else
    {
        ssize_t size_of_read;
        char* magic;
        magic = (char*)malloc(4 * sizeof(char));
        int* header_size;
        header_size = (int*)malloc(2 * sizeof(int));
        int* version;
        version = (int*)malloc(2*sizeof(int));
        int* nr_of_sections;
        nr_of_sections = (int*)malloc(2*sizeof(int));
        int* sect_type;
        sect_type = (int*)malloc(2*sizeof(int));
        char* section_name;
        section_name = (char*)malloc(10 * sizeof(char));
        int* sect_size;
        sect_size = (int*)malloc(4 * sizeof(int));

        lseek(fd, 0, SEEK_SET);
        size_of_read = read(fd, magic, 4);
        if (size_of_read < 0)
        {
            free(magic);
            free(header_size);
            free(version);
            free(nr_of_sections);
            free(sect_type);
            free(section_name);
            free(sect_size);
            close(fd);
            exit(2);
        }

        if (strcmp(magic, "jAWa") != 0)
        {
            printf("ERROR\nwrong magic");
            free(magic);
            free(header_size);
            free(version);
            free(nr_of_sections);
            free(sect_type);
            free(section_name);
            free(sect_size);
            close(fd);
            exit(3);
        }

        size_of_read = read(fd, header_size, 2);
        if (size_of_read < 0)
        {
            free(magic);
            free(header_size);
            free(version);
            free(nr_of_sections);
            free(sect_type);
            free(section_name);
            free(sect_size);
            close(fd);
            exit(4);
        }

        size_of_read = read(fd, version, 1);
        if (size_of_read < 0)
        {
            free(magic);
            free(header_size);
            free(version);
            free(nr_of_sections);
            free(sect_type);
            free(section_name);
            free(sect_size);
            close(fd);
            exit(5);
        }

        if (*version < 12 || *version>37)
        {
            printf("ERROR\nwrong version");
            free(magic);
            free(header_size);
            free(version);
            free(nr_of_sections);
            free(sect_type);
            free(section_name);
            free(sect_size);
            close(fd);
            exit(6);
        }

        size_of_read = read(fd, nr_of_sections, 1);
        if (size_of_read < 0)
        {
            free(magic);
            free(header_size);
            free(version);
            free(nr_of_sections);
            free(sect_type);
            free(section_name);
            free(sect_size);
            close(fd);
            exit(7);
        }

        if (*nr_of_sections < 4 || *nr_of_sections>15)
        {
            printf("ERROR\nwrong sect_nr");
            free(magic);
            free(header_size);
            free(version);
            free(nr_of_sections);
            free(sect_type);
            free(section_name);
            free(sect_size);
            close(fd);
            exit(8);
        }

        if(*nr_of_sections)
        for (int i = 1; i <= *nr_of_sections; i++) {
            lseek(fd, 10, SEEK_CUR);
            size_of_read = read(fd, sect_type, 1);
            if (size_of_read < 0)
            {
                free(magic);
                free(header_size);
                free(version);
                free(nr_of_sections);
                free(sect_type);
                free(section_name);
                free(sect_size);
                close(fd);
                exit(9);
            }
            if (*sect_type != 10)
                if(*sect_type!=25)
            {
                printf("ERROR\nwrong sect_types");
                free(magic);
                free(header_size);
                free(version);
                free(nr_of_sections);
                free(sect_type);
                free(section_name);
                free(sect_size);
                close(fd);
                exit(10);
            }
            lseek(fd, 8, SEEK_CUR);
        }

        printf("SUCCESS\n");
        printf("version=%d\n",*version);
        printf("nr_sections=%d\n", *nr_of_sections);

        if (*nr_of_sections) {
            lseek(fd, 8, SEEK_SET);
            for (int i = 1; i <= *nr_of_sections; i++) {
                printf("section%d:", i);
                size_of_read = read(fd, section_name, 10);
                printf(" %s", section_name);
                size_of_read = read(fd, sect_type, 1);
                printf(" %d", *sect_type);
                lseek(fd, 4, SEEK_CUR);
                size_of_read = read(fd, sect_size, 4);
                printf(" %d\n", *sect_size);
            }
        }

        free(magic);
        free(header_size);
        free(version);
        free(nr_of_sections);
        free(sect_type);
        free(section_name);
        free(sect_size);
    }
    close(fd);
}

void reverseStr(char* str, int length)
{
    char aux;

    for (int i = 0; i < length / 2; i++) {
        aux = str[i];
        str[i] = str[length - i - 1];
        str[length - i - 1] = aux;
    }
}

void extract(const char* path,char* section, char* line) {
    int section_;
    int line_;

    section_ = atoi(section);
    line_ = atoi(line);

    int fd;

    fd = open(path, O_RDONLY);

    if (fd == -1)
    {
        printf("ERROR\ninvalid file");
        close(fd);
        exit(1);
    }
    else {
        int* nr_of_sections;
        nr_of_sections = (int*)malloc(2 * sizeof(int));
        int* sect_size;
        sect_size = (int*)malloc(4 * sizeof(int));
        int* sect_offset;
        sect_offset = (int*)malloc(4 * sizeof(int));
        lseek(fd, 7, SEEK_SET);
        read(fd, nr_of_sections, 1);

        if (section_<0 || section_>* nr_of_sections)
        {
            printf("ERROR\ninvalid section");
            free(nr_of_sections);
            free(sect_offset);
            free(sect_size);
            exit(2);
        }

        for (int i = 1; i <= *nr_of_sections; i++) {
            if (i == section_)
            {
                lseek(fd, 11, SEEK_CUR);
                read(fd, sect_offset, 4);
                read(fd, sect_size, 4);
            }
            else
                lseek(fd, 19, SEEK_CUR);
        }
            lseek(fd, *sect_offset, SEEK_CUR);
            char reading_byte;
            int nr = 1;
            int position_of_reading = 0;
            char* reading_line;
            reading_line = (char*)malloc(10000000 * sizeof(char));
            bool line_found = false;

            for (int j = 0; j <= *sect_size && line_found == false; j++) {
                if (nr != line_) {
                    read(fd, &reading_byte, 1);

                    if (reading_byte == '\n')
                        nr++;
                }
                else
                {

                    off_t position_of_cursor = lseek(fd, 0, SEEK_CUR);
                        read(fd, &reading_byte, 1);

                        if (reading_byte == '\n' || (position_of_cursor == *sect_size - 1)) {
                            line_found = true;
                            position_of_cursor = 0;
                        }
                        else
                        {
                            reading_line[position_of_reading] = reading_byte;
                            position_of_reading++;
                        }
                }
            }

            if (!line_found) {
                printf("ERROR\ninvalid line");
                free(nr_of_sections);
                free(sect_offset);
                free(sect_size);
                free(reading_line);
                exit(3);
            }
            else {
                reverseStr(reading_line, position_of_reading - 1);
                printf("SUCCESS\n");
                printf("%s", reading_line);
            }
            free(nr_of_sections);
            free(sect_offset);
            free(sect_size);
            free(reading_line);
    }
    close(fd);
}

void findall(const char* pathname, int OK, bool success) {
    DIR* directory = NULL;
    struct dirent* files = NULL;
    struct stat s;

    directory = opendir(pathname);

    if (directory == NULL)
    {
        printf("ERROR\ninvalid directory path\n");
        exit(1);
    }
    else
        if (success)
        {
            printf("SUCCESS\n");
            success = false;
        }

    while ((files = readdir(directory)) != NULL)
        if (strncmp(files->d_name, ".", 1) != 0 && strncmp(files->d_name, "..", 2) != 0)
        {
            char* path;

            path = (char*)malloc(sizeof(char) * strlen(pathname));

            snprintf(path, 512, "%s/%s", pathname, files->d_name);
            lstat(path, &s);
            
                int fd;
                fd = open(pathname, O_RDONLY);

                ssize_t size_of_read;
                char* magic;
                magic = (char*)malloc(4 * sizeof(char));
                int* header_size;
                header_size = (int*)malloc(2 * sizeof(int));
                int* version;
                version = (int*)malloc(2 * sizeof(int));
                int* nr_of_sections;
                nr_of_sections = (int*)malloc(2 * sizeof(int));
                int* sect_type;
                sect_type = (int*)malloc(2 * sizeof(int));
                char* section_name;
                section_name = (char*)malloc(10 * sizeof(char));
                int* sect_size;
                sect_size = (int*)malloc(4 * sizeof(int));

                lseek(fd, 0, SEEK_SET);
                size_of_read = read(fd, magic, 4);
                if (size_of_read < 0)
                {
                    free(magic);
                    free(header_size);
                    free(version);
                    free(nr_of_sections);
                    free(sect_type);
                    free(section_name);
                    free(sect_size);
                    close(fd);
                    exit(2);
                }

                if (strcmp(magic, "jAWa") != 0)
                {
                    printf("ERROR\nwrong magic");
                    free(magic);
                    free(header_size);
                    free(version);
                    free(nr_of_sections);
                    free(sect_type);
                    free(section_name);
                    free(sect_size);
                    close(fd);
                    exit(3);
                }

                size_of_read = read(fd, header_size, 2);
                if (size_of_read < 0)
                {
                    free(magic);
                    free(header_size);
                    free(version);
                    free(nr_of_sections);
                    free(sect_type);
                    free(section_name);
                    free(sect_size);
                    close(fd);
                    exit(4);
                }

                size_of_read = read(fd, version, 1);
                if (size_of_read < 0)
                {
                    free(magic);
                    free(header_size);
                    free(version);
                    free(nr_of_sections);
                    free(sect_type);
                    free(section_name);
                    free(sect_size);
                    close(fd);
                    exit(5);
                }

                if (*version < 12 || *version>37)
                {
                    printf("ERROR\nwrong version");
                    free(magic);
                    free(header_size);
                    free(version);
                    free(nr_of_sections);
                    free(sect_type);
                    free(section_name);
                    free(sect_size);
                    close(fd);
                    exit(6);
                }

                size_of_read = read(fd, nr_of_sections, 1);
                if (size_of_read < 0)
                {
                    free(magic);
                    free(header_size);
                    free(version);
                    free(nr_of_sections);
                    free(sect_type);
                    free(section_name);
                    free(sect_size);
                    close(fd);
                    exit(7);
                }

                if (*nr_of_sections < 4 || *nr_of_sections>15)
                {
                    printf("ERROR\nwrong sect_nr");
                    free(magic);
                    free(header_size);
                    free(version);
                    free(nr_of_sections);
                    free(sect_type);
                    free(section_name);
                    free(sect_size);
                    close(fd);
                    exit(8);
                }

                if (*nr_of_sections)
                    for (int i = 1; i <= *nr_of_sections; i++) {
                        lseek(fd, 10, SEEK_CUR);
                        size_of_read = read(fd, sect_type, 1);
                        if (size_of_read < 0)
                        {
                            free(magic);
                            free(header_size);
                            free(version);
                            free(nr_of_sections);
                            free(sect_type);
                            free(section_name);
                            free(sect_size);
                            close(fd);
                            exit(9);
                        }
                        if (*sect_type != 10)
                            if (*sect_type != 25)
                            {
                                printf("ERROR\nwrong sect_types");
                                free(magic);
                                free(header_size);
                                free(version);
                                free(nr_of_sections);
                                free(sect_type);
                                free(section_name);
                                free(sect_size);
                                close(fd);
                                exit(10);
                            }
                        lseek(fd, 8, SEEK_CUR);
                    }

                if (*nr_of_sections) {
                    for (int i = 1; i <= *nr_of_sections; i++) {
                        lseek(fd, 15, SEEK_SET);
                        size_of_read = read(fd, sect_size, 4);
                        if(*sect_size>1202)
                            printf("%s\n", path);
                    }
                }
                free(nr_of_sections);
                free(sect_size);
                free(path);
           

            if (S_ISDIR(s.st_mode) && OK == 1)
                findall(path, 1, success);
        }

    closedir(directory);
}

int main(int argc, char **argv){
    char *directoryName;
    char *nameToEndWith;

    directoryName = (char*)malloc(sizeof(char) * 150);
    nameToEndWith = (char*)malloc(sizeof(char) * 150);


    if(argc >= 2){
        if(strcmp(argv[1], "variant") == 0){
            printf("55777\n");
        }

        if (strcmp(argv[1], "list") == 0)
        {
            if (strncmp(argv[2], "path=", 5) == 0)
            {
                strcpy(directoryName, &argv[2][5]);
                list(directoryName,0,true,"",0);
            }
            else if (strncmp(argv[2], "name_ends_with=", 15) == 0 && strncmp(argv[3], "path=", 5) == 0)
            {
                strcpy(nameToEndWith, &argv[2][15]);
                strcpy(directoryName, &argv[3][5]);
                list(directoryName, 0, true, nameToEndWith, 0);
            }
            else if (strncmp(argv[3], "name_ends_with=", 15) == 0 && strncmp(argv[2], "path=", 5) == 0)
            {
                strcpy(nameToEndWith, &argv[3][15]);
                strcpy(directoryName, &argv[2][5]);
                list(directoryName, 0, true, nameToEndWith, 0);
            }
            else if (strcmp(argv[2], "has_perm_write") == 0 && strncmp(argv[3], "path=", 5) == 0)
            {
                strcpy(directoryName, &argv[3][5]);
                list(directoryName, 0, true, "", 1);
            }
            else if (strcmp(argv[3], "has_perm_write") == 0 && strncmp(argv[2], "path=", 5) == 0)
            {
                strcpy(directoryName, &argv[2][5]);
                list(directoryName, 0, true, "", 1);
            }
            else if (strcmp(argv[2], "recursive") == 0)
            {
                if (strncmp(argv[3], "path=", 5) == 0)
                {
                    strcpy(directoryName, &argv[3][5]);
                    list(directoryName,1,true,"",0);
                }
                else if (strncmp(argv[3], "name_ends_with=", 15) == 0 && strncmp(argv[4], "path=", 5) == 0)
                {
                    strcpy(nameToEndWith, &argv[3][15]);
                    strcpy(directoryName, &argv[4][5]);
                    list(directoryName, 1, true, nameToEndWith, 0);
                }
                else if (strncmp(argv[4], "name_ends_with=", 15) == 0 && strncmp(argv[3], "path=", 5) == 0)
                {
                    strcpy(nameToEndWith, &argv[4][15]);
                    strcpy(directoryName, &argv[3][5]);
                    list(directoryName, 1, true, nameToEndWith, 0);
                }
                else if (strcmp(argv[3], "has_perm_write") == 0 && strncmp(argv[4], "path=", 5) == 0)
                {
                    strcpy(directoryName, &argv[4][5]);
                    list(directoryName, 1, true, "", 1);
                }
                else if (strcmp(argv[4], "has_perm_write") == 0 && strncmp(argv[3], "path=", 5) == 0)
                {
                    strcpy(directoryName, &argv[3][5]);
                    list(directoryName, 1, true, "", 1);
                }
            }
            else if (strcmp(argv[3], "recursive") == 0)
            {
                if (strncmp(argv[2], "path=", 5) == 0)
                {
                    strcpy(directoryName, &argv[2][5]);
                    list(directoryName, 1, true, "", 0);
                }
                else if (strncmp(argv[2], "name_ends_with=", 15) == 0 && strncmp(argv[4], "path=", 5) == 0)
                {
                    strcpy(nameToEndWith, &argv[2][15]);
                    strcpy(directoryName, &argv[4][5]);
                    list(directoryName, 1, true, nameToEndWith, 0);
                }
                else if (strncmp(argv[4], "name_ends_with=", 15) == 0 && strncmp(argv[2], "path=", 5) == 0)
                {
                    strcpy(nameToEndWith, &argv[4][15]);
                    strcpy(directoryName, &argv[2][5]);
                    list(directoryName, 1, true, nameToEndWith, 0);
                }
                else if (strcmp(argv[2], "has_perm_write") == 0 && strncmp(argv[4], "path=", 5) == 0)
                {
                    strcpy(directoryName, &argv[4][5]);
                    list(directoryName, 1, true, "", 1);
                }
                else if (strcmp(argv[4], "has_perm_write") == 0 && strncmp(argv[2], "path=", 5) == 0)
                {
                    strcpy(directoryName, &argv[2][5]);
                    list(directoryName, 1, true, "", 1);
                }
            }
        }
        else if (strcmp(argv[1], "parse") == 0) {
            if (strncmp(argv[2], "path=", 5) == 0) {
                strcpy(directoryName, &argv[2][5]);
                parse(directoryName);
            }
            
        }
        else if (strcmp(argv[1], "extract") == 0) {
            if (strncmp(argv[2], "path=", 5) == 0 && strncmp(argv[3], "section=", 8) == 0 && strncmp(argv[4],"line=",5)==0) {
                strcpy(directoryName, &argv[2][5]);
                char *section;
                section = (char*)malloc(sizeof(char));
                strcpy(section, &argv[3][8]);
                char* line;
                line = (char*)malloc(sizeof(char));
                strcpy(line, &argv[4][5]);
                extract(directoryName, section, line);
            }
        }
        else if (strcmp(argv[1], "findall") == 0 && strncmp(argv[2], "path=", 5) == 0) {
        strcpy(directoryName, &argv[2][5]);
        findall(directoryName,1,true);
        }
        
    }
    

    free(directoryName);
    free(nameToEndWith);

    return 0;
}