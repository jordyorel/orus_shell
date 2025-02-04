#include "jobs.h"

static Job jobs[MAX_JOBS];
static int job_count = 0;


/* Job Control Functions */
void add_job(pid_t pid, char *cmd, int is_running) {
    if (job_count < MAX_JOBS) {
        jobs[job_count].pid = pid;
        jobs[job_count].cmd = strdup(cmd);
        jobs[job_count].is_running = is_running;
        job_count++;
    } else {
        fprintf(stderr, COLOR_RED "Job list is full.\n" COLOR_RESET);
    }
}

void update_job_status() {
    for (int i = 0; i < job_count; i++) {
        if (jobs[i].is_running) {
            int status;
            if (waitpid(jobs[i].pid, &status, WNOHANG) > 0) {
                jobs[i].is_running = 0;
                printf("[%d] Done: %s\n", i + 1, jobs[i].cmd);
            }
        }
    }
}

void list_jobs() {
    for (int i = 0; i < job_count; i++) {
        printf("[%d] %s %s\n", i + 1, jobs[i].is_running ? "Running" : "Done",
               jobs[i].cmd);
    }
}