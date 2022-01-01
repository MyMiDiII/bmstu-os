#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <windows.h>

#define READERS_NUM 5
#define WRITERS_NUM 3

#define ITERS 7

#define MIN_RSLEEP 1000
#define MAX_RSLEEP 3000

#define MIN_WSLEEP 1000
#define MAX_WSLEEP 3000

HANDLE mutex;
HANDLE can_read;
HANDLE can_write;

int waiting_writers = 0;
int waiting_readers = 0;
int active_readers = 0;
bool active_writer = false;

int counter = 0;

void start_write(void)
{
    InterlockedIncrement(&waiting_writers);

    if (active_writer || WaitForSingleObject(can_read, 0) == WAIT_OBJECT_0)
        WaitForSingleObject(can_write, INFINITE);

    InterlockedDecrement(&waiting_writers);

    active_writer = true;
}

void stop_write(void)
{
    active_writer = false;

    if (waiting_readers == 0)
        SetEvent(can_write);
    else
        SetEvent(can_read);
}

void start_read(void)
{
    InterlockedIncrement(&waiting_readers);

    if (active_writer || (WaitForSingleObject(can_write, 0) == WAIT_OBJECT_0))
        WaitForSingleObject(can_read, INFINITE);

    WaitForSingleObject(mutex, INFINITE);

    InterlockedDecrement(&waiting_readers);
    SetEvent(can_read);
    InterlockedIncrement(&active_readers);

    ReleaseMutex(mutex);
}

void stop_read(void)
{
    InterlockedDecrement(&active_readers);

    if (active_readers == 0)
    {
       ResetEvent(can_read);
       SetEvent(can_write);
    }
}


DWORD WINAPI run_reader(CONST LPVOID lpParams)
{
    int rid = *(int *)lpParams;
    srand(time(NULL) + rid);

    for (size_t i = 0; i < ITERS; i++)
    {
        int stime = rand() % (MAX_RSLEEP - MIN_RSLEEP) + MIN_RSLEEP;
        Sleep(stime);

        start_read();
        printf("\e[1;33mReader #%d  read: %2d (sleep: %dms)\e[0m\n", rid,
                    counter, stime);
        stop_read();
    }

    return 0;
}

DWORD WINAPI run_writer(CONST LPVOID lpParams)
{
    int wid = *(int *)lpParams;
    srand(time(NULL) + wid);

    for (size_t i = 0; i < ITERS + 1; i++)
    {
        int stime = rand() % (MAX_WSLEEP - MIN_WSLEEP) + MIN_WSLEEP;
        Sleep(stime);

        start_write();
        counter++;
        printf("\e[1;34mWriter #%d write: %2d (sleep: %dms)\e[0m\n", wid,
                   counter, stime);
        stop_write();
    }

    return 0;
}

int main()
{
    setbuf(stdout, NULL);

    HANDLE readers_threads[READERS_NUM];
    HANDLE writers_threads[WRITERS_NUM];

    int readers_ids[READERS_NUM];
    int writers_ids[WRITERS_NUM];

    if ((mutex = CreateMutex(NULL, FALSE, NULL)) == NULL)
    {
        perror("Failed to CreateMutex");
        return -1;
    }

    if ((can_read = CreateEvent(NULL, FALSE, FALSE, NULL)) == NULL)
    {
        perror("Failder to CreateEVent can_read");
        return -1;
    }

    if ((can_write = CreateEvent(NULL, FALSE, FALSE, NULL)) == NULL)
    {
        perror("Failder to CreateEVent can_write");
        return -1;
    }

    for (size_t i = 0; i < READERS_NUM; i++)
    {
        readers_ids[i] = i + 1;
        readers_threads[i] = CreateThread(NULL, 0, run_reader,
                                          readers_ids + i, 0, NULL);

        if (readers_threads[i] == NULL)
        {
            perror("Failed to CreateThread");
            return -1;
        }
    }

    for (size_t i = 0; i < WRITERS_NUM; i++)
    {
        writers_ids[i] = i + 1;
        writers_threads[i] = CreateThread(NULL, 0, run_writer,
                                          writers_ids + i, 0, NULL);

        if (writers_threads[i] == NULL)
        {
            perror("Failed to CreateThread");
            return -1;
        }
    }

    WaitForMultipleObjects(READERS_NUM, readers_threads, TRUE, INFINITE);
    WaitForMultipleObjects(WRITERS_NUM, writers_threads, TRUE, INFINITE);

    for (int i = 0; i < READERS_NUM; i++)
        CloseHandle(readers_threads[i]);

    for (int i = 0; i < WRITERS_NUM; i++)
        CloseHandle(writers_threads[i]);

    CloseHandle(mutex);
    CloseHandle(can_read);
    CloseHandle(can_write);

    puts("\e[1;32mOK\e[0m\n");

    return 0;
}
