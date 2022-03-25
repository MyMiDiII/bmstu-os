#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>

#include "stat_expl.h"

#define BUF_SIZE 1000

void get_cmdline_info()
{
    FILE *f = fopen("/proc/self/cmdline", "r");

    char buf[BUF_SIZE + 1];
    int len = fread(buf, 1, BUF_SIZE, f);
    buf[len] = '\0';

    FILE *out = fopen("info.txt", "a");
    fprintf(out, "CMDLINE\n");
    fprintf(out, "%s\n\n", buf);

    fclose(f);
    fclose(out);
}

void get_cwd_info()
{
    char buf[BUF_SIZE + 1];
    int len = readlink("/proc/self/cwd", buf, BUF_SIZE);
    buf[len] = '\0';

    FILE *out = fopen("info.txt", "a");
    fprintf(out, "CWD\n");
    fprintf(out, "%s\n\n", buf);

    fclose(out);
}

void get_environ_info()
{
    FILE *f = fopen("/proc/self/environ", "r");
    FILE *out = fopen("info.txt", "a");
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
    fclose(out);
}

void get_exe_info()
{
    char buf[BUF_SIZE + 1];
    int len = readlink("/proc/self/exe", buf, BUF_SIZE);
    buf[len] = '\0';

    FILE *out = fopen("info.txt", "a");
    fprintf(out, "EXE\n");
    fprintf(out, "%s\n\n", buf);

    fclose(out);
}

void get_fd_info()
{
    DIR *dp = opendir("/proc/self/fd");
    struct dirent *dirp;
    char path[BUF_SIZE + 1];
    char buf[BUF_SIZE + 1];
    FILE *out = fopen("info.txt", "a");
    fprintf(out, "FD\n");

    while ((dirp = readdir(dp)) != NULL)
    {
        if ((strcmp(dirp->d_name, ".") != 0)
                && (strcmp(dirp->d_name, "..") != 0))
        {
            sprintf(path, "%s/%s", "/proc/self/fd", dirp->d_name);
            int len = readlink(path, buf, BUF_SIZE);
            buf[len] = '\0';

            fprintf(out, "%s -> %s\n", dirp->d_name, buf);

        }
    }
    fprintf(out, "\n");

    closedir(dp);
    fclose(out);
}

void get_root_info()
{
    char buf[BUF_SIZE + 1];
    int len = readlink("/proc/self/root", buf, BUF_SIZE);
    buf[len] = '\0';

    FILE *out = fopen("info.txt", "a");
    fprintf(out, "ROOT\n");
    fprintf(out, "%s\n\n", buf);

    fclose(out);
}

void get_stat_info()
{
    FILE *f = fopen("/proc/self/stat", "r");
    FILE *out = fopen("info.txt", "a");
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

    fprintf(out, "\n");

    fclose(f);
    fclose(out);
}

int main(int argc, char **argv)
{
    get_cmdline_info();
    get_cwd_info();
    get_environ_info();
    get_exe_info();
    get_fd_info();
    get_root_info();
    get_stat_info();

    return 0;
}
