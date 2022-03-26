#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>

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

void print_page(uint64_t address, uint64_t data, FILE *out)
{
    fprintf(out, "0x%-16lx : %-16lx %-10ld %-10ld %-10ld %-10ld\n",
        address,
        data & (((uint64_t)1 << 55) - 1),
        (data >> 55) & 1,
        (data >> 61) & 1,
        (data >> 62) & 1,
        (data >> 63) & 1);
}

void get_pagemap_info(const char *proc, FILE *out)
{
    fprintf(out, "PAGEMAP\n");
    fprintf(out, "       addr        : pfn          "
                 "soft-dirty file/shared "
                 "swapped    present\n");
    char path[PATH_MAX];
    snprintf(path, PATH_MAX, "/proc/%s/maps", proc);

    FILE *maps = fopen(path, "r");

    snprintf(path, PATH_MAX, "/proc/%s/pagemap", proc);
    int pm_fd = open(path, O_RDONLY);

    char buf[BUF_SIZE + 1] = "\0";
    int len;

    while ((len = fread(buf, 1, BUF_SIZE, maps)) > 0)
    {
        for (int i = 0; i < len; ++i)
            if (buf[i] == 0)
                buf[i] = '\n';

        buf[len] = '\0';

        char *save_row;
        char *row = strtok_r(buf, "\n", &save_row);

        while (row)
        {
            char *addresses = strtok(row, " ");

            char *start_str, *end_str;

            if ((start_str = strtok(addresses, "-")) &&
                (end_str = strtok(NULL, "-")))
            {

                uint64_t start = strtoul(start_str, NULL, 16);
                uint64_t end = strtoul(end_str, NULL, 16);

                for(uint64_t i = start; i < end; i += sysconf(_SC_PAGE_SIZE))
                {
                    uint64_t offset;
                    uint64_t index = ((i / sysconf(_SC_PAGE_SIZE))
                                         * sizeof(offset));

                    pread(pm_fd, &offset, sizeof(offset), index);
                    print_page(i, offset, out);
                }
            }

            row = strtok_r(NULL, "\n", &save_row);
        }

    }

    fclose(maps);
    close(pm_fd);
}

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
    get_pagemap_info(id, out);

    fclose(out);

    return 0;
}
