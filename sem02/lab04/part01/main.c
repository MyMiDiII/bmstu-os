#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>

#include "stat_expl.h"

#define BUF_SIZE 1000
#define ID_SIZE 10

void get_cmdline_info(const char *proc, FILE *out)
{
    char path[PATH_MAX];
    snprintf(path, PATH_MAX, "/proc/%s/cmdline", proc);

    FILE *f = fopen(path, "r");

    char buf[BUF_SIZE + 1];
    int len = fread(buf, 1, BUF_SIZE, f);
    buf[len] = '\0';

    fprintf(out, "CMDLINE\n");
    fprintf(out, "%s\n\n", buf);

    fclose(f);
}

void get_cwd_info(const char *proc, FILE *out)
{
    char path[PATH_MAX];
    snprintf(path, PATH_MAX, "/proc/%s/cwd", proc);

    char buf[BUF_SIZE + 1];
    int len = readlink(path, buf, BUF_SIZE);
    buf[len] = '\0';

    fprintf(out, "CWD\n");
    fprintf(out, "%s\n\n", buf);
}

void get_environ_info(const char *proc, FILE *out)
{
    char path[PATH_MAX];
    snprintf(path, PATH_MAX, "/proc/%s/environ", proc);

    FILE *f = fopen(path, "r");
    fprintf(out, "ENVIRON\n");

    char buf[BUF_SIZE + 1];
    int len;

    while ((len = fread(buf, 1, BUF_SIZE, f)) > 0)
    {
        for (int i = 0; i < len; ++i)
            if (buf[i] == 0)
                buf[i] = '\n';

        buf[len] = '\0';
        fprintf(out, "%s", buf);
    }
    fprintf(out, "\n");

    fclose(f);
}

void get_exe_info(const char *proc, FILE *out)
{
    char path[PATH_MAX];
    snprintf(path, PATH_MAX, "/proc/%s/exe", proc);

    char buf[BUF_SIZE + 1];
    int len = readlink(path, buf, BUF_SIZE);
    buf[len] = '\0';

    fprintf(out, "EXE\n");
    fprintf(out, "%s\n\n", buf);
}

void get_fd_info(const char *proc, FILE *out)
{
    char path[PATH_MAX];
    snprintf(path, PATH_MAX, "/proc/%s/fd", proc);

    DIR *dp = opendir(path);
    struct dirent *dirp;
    char full_path[BUF_SIZE + 1];
    char buf[BUF_SIZE + 1];
    fprintf(out, "FD\n");

    while ((dirp = readdir(dp)) != NULL)
    {
        if ((strcmp(dirp->d_name, ".") != 0)
                && (strcmp(dirp->d_name, "..") != 0))
        {
            sprintf(full_path, "%s/%s", path, dirp->d_name);
            int len = readlink(full_path, buf, BUF_SIZE);
            buf[len] = '\0';

            fprintf(out, "%s -> %s\n", dirp->d_name, buf);
        }
    }
    fprintf(out, "\n");

    closedir(dp);
}

void get_root_info(const char *proc, FILE *out)
{
    char path[PATH_MAX];
    snprintf(path, PATH_MAX, "/proc/%s/root", proc);

    char buf[BUF_SIZE + 1];
    int len = readlink(path, buf, BUF_SIZE);
    buf[len] = '\0';

    fprintf(out, "ROOT\n");
    fprintf(out, "%s\n\n", buf);
}

void get_stat_info(const char *proc, FILE *out)
{
    char path[PATH_MAX];
    snprintf(path, PATH_MAX, "/proc/%s/stat", proc);

    FILE *f = fopen(path, "r");
    fprintf(out, "STAT\n");

    char buf[BUF_SIZE + 1] = "\0";
    fread(buf, 1, BUF_SIZE, f);

    char *stat_elem = strtok(buf, " ");
    int i = 0;

    while (stat_elem != NULL)
    {
        fprintf(out, expls[i++], stat_elem);
        stat_elem = strtok(NULL, " ");
    }

    fclose(f);
}

void get_io_info(const char *proc, FILE *out)
{
    char path[PATH_MAX];
    snprintf(path, PATH_MAX, "/proc/%s/io", proc);

    FILE *f = fopen(path, "r");
    fprintf(out, "IO\n");

    char buf[BUF_SIZE + 1];
    int len;

    while ((len = fread(buf, 1, BUF_SIZE, f)) > 0)
    {
        for (int i = 0; i < len; ++i)
            if (buf[i] == 0)
                buf[i] = '\n';

        buf[len] = '\0';
        fprintf(out, "%s", buf);
    }
    fprintf(out, "\n");

    fclose(f);
}

void get_maps_info(const char *proc, FILE *out)
{
    char path[PATH_MAX];
    snprintf(path, PATH_MAX, "/proc/%s/maps", proc);

    FILE *f = fopen(path, "r");
    fprintf(out, "MAPS\n");

    char buf[BUF_SIZE + 1];
    int len;

    while ((len = fread(buf, 1, BUF_SIZE, f)) > 0)
    {
        for (int i = 0; i < len; ++i)
            if (buf[i] == 0)
                buf[i] = '\n';

        buf[len] = '\0';
        fprintf(out, "%s", buf);
    }
    fprintf(out, "\n");

    fclose(f);
}

void get_comm_info(const char *proc, FILE *out)
{
    char path[PATH_MAX];
    snprintf(path, PATH_MAX, "/proc/%s/comm", proc);

    FILE *f = fopen(path, "r");

    char buf[BUF_SIZE + 1];
    int len = fread(buf, 1, BUF_SIZE, f);
    buf[len] = '\0';

    fprintf(out, "COMM\n");
    fprintf(out, "%s\n", buf);

    fclose(f);
}

//void get_pagemap_info(const char *proc, FILE *out)
//{
//    int m_fd = open("/proc/self/maps", O_RDONLY);
//    int pm_fd = open("/proc/self/pagemap", O_RDONLY);
//
//    uint64_t start_address = strtoul(argv[2], NULL, 0);
//    uint64_t end_address = strtoul(argv[3], NULL, 0);
//
//    for(uint64_t i = start_address; i < end_address; i += 0x1000) {
//        uint64_t data;
//        uint64_t index = (i / PAGE_SIZE) * sizeof(data);
//        if(pread(fd, &data, sizeof(data), index) != sizeof(data)) {
//            perror("pread");
//            break;
//        }
//
//        print_page(i, data);
//    }
//
//    close(fd);
//    return 0;
//}

char *get_proc_id(int argc, char **argv)
{
    static char id[ID_SIZE] = "\0";

    if (argc < 2)
    {
        snprintf(id, ID_SIZE, "self");
    }
    else
    {
        snprintf(id, ID_SIZE, "%s", argv[1]);
    }

    printf("%s\n", id);

    return id;
}

int main(int argc, char **argv)
{
    char *id = get_proc_id(argc, argv);

    FILE *out = fopen("info.txt", "w");

    get_cmdline_info(id, out);
    get_cwd_info(id, out);
    get_exe_info(id, out);
    get_comm_info(id, out);
    get_root_info(id, out);
    get_environ_info(id, out);
    get_fd_info(id, out);
    get_stat_info(id, out);
    get_io_info(id, out);
    get_maps_info(id, out);
    //get_pagemap_info(id, out);

    fclose(out);

    return 0;
}
