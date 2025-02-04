#ifndef JOBS_H
#define JOBS_H

#include "orus.h"

void add_job(pid_t pid, char *cmd, int is_running);
void list_jobs();
void update_job_status();

#endif
