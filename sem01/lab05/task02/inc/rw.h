#ifndef RW_H
#define RW_H 

void create_writer(int *const counter, const int sid, const int wid);
void writer_run(int * const counter, const int sid, const int wid);

void create_reader(int *const counter, const int sid, const int rid);
void run_reader(int *const counter, const int sid, const int rid);

#endif
