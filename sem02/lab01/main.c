#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/syslog.h>
#include <time.h>
#include <syslog.h>
#include <unistd.h>

#define LOCKFILE "/var/run/daemon.pid"
#define LOCKMODE S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH

sigset_t mask;

int lockfile(int fd)
{
    struct flock fl = {
        .l_type = F_WRLCK,
        .l_start = 0,
        .l_whence = SEEK_SET,
        .l_len = 0
    };

    return fcntl(fd, F_SETLK, &fl);
}

void daemonize(const char* cmd)
{
    int i, fd0, fd1, fd2;
    pid_t pid;
    struct rlimit rl;
    struct sigaction sa;
    
    umask(0);

    if (getrlimit(RLIMIT_NOFILE, &rl) < 0)
        printf("%s: getrlimit failed", cmd);

    if ((pid = fork()) < 0)
        printf("%s: fork failed", cmd);
    else if (pid != 0) 
        exit(0);

    if (setsid() < 0)
        printf("%s: setsid failed", cmd);

    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGHUP, &sa, NULL) < 0)
        printf("%s: ignore SIGHUP failed", cmd);

    if (chdir("/") < 0)
        printf("%s: change directory to / failed", cmd);

    if (rl.rlim_max == RLIM_INFINITY)
        rl.rlim_max = 1024;

    for (i = 0; i < rl.rlim_max; i++)
        close(i);

    fd0 = open("/dev/null", O_RDWR);
    fd1 = dup(0);
    fd2 = dup(0);

    openlog(cmd, LOG_CONS, LOG_DAEMON);

    if (fd0 != 0 || fd1 != 1 || fd2 != 2)
    {
        syslog(LOG_ERR, "unexpected file descriptors %d %d %d",
               fd0, fd1, fd2);
        exit(1);
    }
}

int already_running(void)
{
    int fd;
    char buf[16];

    fd = open(LOCKFILE, O_RDWR | O_CREAT, LOCKMODE);

    if (fd < 0)
    {
        syslog(LOG_ERR, "open %s failed: %s", LOCKFILE, strerror(errno));
        return 1;
    }

    if (lockfile(fd) < 0)
    {
        if (errno == EACCES || errno == EAGAIN)
        {
            close(fd);
            syslog(LOG_INFO, "lock %s failed: %s (already locked)", LOCKFILE,
                   strerror(errno));

            return 1;
        }

        syslog(LOG_ERR, "lock %s failed: %s", LOCKFILE, strerror(errno));
        return 1;
    }

    ftruncate(fd, 0);
    sprintf(buf, "%d", getpid());
    write(fd, buf, strlen(buf) + 1);

    return 0;
}

void* thr_fn(void* arg)
{
    int err, signo;

    while (1)
    {
        err = sigwait(&mask, &signo);

        if (err != 0) {
            syslog(LOG_ERR, "sigwait failed");
            exit(1);
        }

        switch (signo)
        {
        case SIGHUP:
            syslog(LOG_INFO, "got SIGHUP");
            syslog(LOG_INFO, "login: %s", getlogin());
            break;

        case SIGTERM:
            syslog(LOG_INFO, "got SIGTERM");
            syslog(LOG_INFO, "exiting");
            exit(0);
        }
    }

    return (void*)0;
}

int main(void)
{
    int err;
    pthread_t tid;
    struct sigaction sa;

    daemonize("mydaemon");

    if (already_running())
    {
        syslog(LOG_ERR, "daemon already running");
        exit(1);
    }

    sa.sa_handler = SIG_DFL;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGHUP, &sa, NULL) < 0)
    {
        printf("restore default SIGHUP failed");
        exit(1);
    }

    sigfillset(&mask);

    if ((err = pthread_sigmask(SIG_BLOCK, &mask, NULL)) != 0)
    {
        printf("SIG_BLOCK error");
        exit(1);
    }

    pthread_create(&tid, NULL, thr_fn, NULL);
    if (tid == -1)
    {
        printf("spawn thread for signal handler failed");
        exit(1);
    }

	while (1)
	{
	    long int cur_time = time(NULL);
		syslog(LOG_INFO, "current time: %s", ctime(&cur_time));
		sleep(3);
	}
}
