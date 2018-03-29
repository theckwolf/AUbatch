//Author:Thomas Heckwolf
//Class:CS7500 Auburn University
//Project: Project 3: AUbatch -­‐ A Pthread-­‐based Batch Scheduling System
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "aubatch.h"

/************ Main Function *********************/
int main(int argc, char *argv[]) {
    //int i;
    //JobPtr RunningJobPtr = (JobPtr) malloc(sizeof(struct Job));
    initReadyQueue();
    pthread_t threads[3];
    pthread_attr_t attr;
/* Initialize mutex and condition variable objects */
    pthread_mutex_init(&queue_mutex, NULL);
    pthread_cond_init(&queue_threshold_cv, NULL);
    pthread_mutex_init(&test_mutex, NULL);
    pthread_cond_init(&test_threshold_cv, NULL);
/* For portability, explicitly create threads in a joinable state */
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    pthread_create(&threads[1], &attr, scheduler, NULL);
    pthread_create(&threads[2], &attr, dispatcher, NULL);

    pthread_join(threads[1], NULL);
    pthread_join(threads[2], NULL);

/* Clean up and exit */
    pthread_attr_destroy(&attr);
    pthread_mutex_destroy(&queue_mutex);
    pthread_cond_destroy(&queue_threshold_cv);
    pthread_exit(NULL);
}

/************ Program Functions *********************/
/*Helper Functions*/
void initReadyQueue() {
    pthread_mutex_lock(&queue_mutex);
    for (int i = 0; i < MAX_JOB_NUMBER; i++) {
        strcpy(ready_queue[i].name, "init");
        ready_queue[i].cputime = 99999;
        ready_queue[i].priority = 99999;
    }
    pthread_mutex_unlock(&queue_mutex);
}

void calculateJobStats() {
    int tempCount = 0;
    float tempTurn = 0;
    float tempWait = 0;
    float tempCpu = 0;

    struct complete_queue *temp = chead;
    if (temp == NULL) {}
    else {
        while (temp != NULL) {
            tempCpu += temp->job.cputime;
            tempWait += temp->job.wait_time;
            tempTurn += temp->job.turnarount_time;
            tempCount++;
            temp = temp->next_job;
        }
        totalcount = tempCount;
        if (totalcount == 0) {}
        else {
            avgTurnaround = tempTurn / totalcount;
            avgWaitTime = tempWait / totalcount;
            avgCpuTime = tempCpu / totalcount;
            totoalCompletedJobs = totalcount;
            avgThroughput = (1 / (avgTurnaround / totalcount));
        }
    }


}

void clearJob(struct Job *job) {
    strcpy(job->name, "");
    job->cputime = 99999;
    job->priority = 99999;
    job->arrival_time = 0;
    job->turnarount_time = 0;
    job->wait_time = 0;
    strcpy(job->status, "init");
    job->number = 0;
};

void clearStats() {
    avgTurnaround = 0;
    avgWaitTime = 0;
    avgCpuTime = 0;
    totoalCompletedJobs = 0;
    avgThroughput = 0;
    totalcount = 0;
}

void clearComplete_queue() {
    struct complete_queue *next;
    struct complete_queue *current = chead;
    struct complete_queue *temp = NULL;
    while (current != NULL) {
        next = current->next_job;
        free(current);
        current = next;
    }
    chead = temp;
};

void inc_reset_tail() {
    tail++;
    if (tail == MAX_JOB_NUMBER) {
        tail = 0;
    }
}

/*Print Functions*/
void printReadyQueue() {
    int tempCount;
    printf("Total number of jobs in the queue: %d\n", count);
    char tempStr[5];
    switch (scheduleType) {
        case FCFS:
            strcpy(tempStr, "FCFS");
            break;
        case SJF:
            strcpy(tempStr, "SJF");
            break;
        case PRI:
            strcpy(tempStr, "PRI");
            break;
    }
    printf("Scheduling Policy: %s.\n", tempStr);


    if (RunningJob != NULL) {
        if (strcmp(RunningJob->status, "complete") != 0) {
            printf("Running Job:\n");
            printf("Name \t\tCPU_Time \tPri \tProgress\n");
            printf("%s %10d %14d %12s\n", RunningJob->name, RunningJob->cputime,
                   RunningJob->priority,
                   RunningJob->status);
            printf("\n");
        } else {

        }
    }

    for (int i = 0; i < MAX_JOB_NUMBER; i++) {
        if (strcmp(ready_queue[i].status, "") == 0 || strcmp(ready_queue[i].status, "complete") == 0 ||
            strcmp(ready_queue[i].status, "init") == 0) {
            continue;
        } else {
            printf("Ready Queue:\n");
            printf("Name \t\t\tCPU_Time \tPri \tArrival_time \tProgress\n");
            printf("%s %20d %10d %10d:%d:%d %18s\n", ready_queue[i].name, ready_queue[i].cputime,
                   ready_queue[i].priority,
                   ready_queue[i].time.tm_hour, ready_queue[i].time.tm_min, ready_queue[i].time.tm_sec,
                   ready_queue[i].status);
        }
    }
    tempCount++;

}

void printFileStats(FILE *fp) {
    fprintf(fp, "Total number of job submitted: %d\n", totalcount);
    fprintf(fp, "Average turnaround time: %f seconds\n", avgTurnaround);
    fprintf(fp, "Average CPU time: %f seconds\n", avgCpuTime);
    fprintf(fp, "Average waiting time: %f seconds\n", avgWaitTime);
    fprintf(fp, "Throughput: %f No./second*/\n", avgThroughput);
}

void printCompleteQueue() {
    int tempCount;
    struct complete_queue *temp = chead;
    if (temp == NULL) {}
    else {
        printf("Completed Jobs:\n");
        printf("Name \t\t\tCPU_Time \tPri \tArrival_time \tProgress\n");
        while (temp != NULL) {
            printf("%s %20d %10d %10d:%d:%d %12s\n", temp->job.name, temp->job.cputime,
                   temp->job.priority,
                   temp->job.time.tm_hour, temp->job.time.tm_min, temp->job.time.tm_sec,
                   temp->job.status);
            temp = temp->next_job;
        }
    }
}

void printStats() {
    printf("\nTotal number of job submitted: %d\n", totalcount);
    printf("Total number of job completed: %d\n", totoalCompletedJobs);
    printf("Average turnaround time: %f seconds\n", avgTurnaround);
    printf("Average CPU time:%f seconds\n", avgCpuTime);
    printf("Average waiting time:%f seconds\n", avgWaitTime);
    printf("Throughput: %f No./second\n", avgThroughput);
    return;
}

/*Producder*/
void *scheduler() {
    printf("Welcome to Thomas Heckwolfs's batch job scheduler Version 1.0 \n Type ‘help’ to find more about AUbatch commands.\n");
    commandline();
    pthread_exit(NULL);
}

void schedule(struct Job *job) {
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    int tempHead;
    job->arrival_time = t;
    job->time = *tm;
    pthread_mutex_lock(&queue_mutex);
    pthread_cond_signal(&queue_threshold_cv);
    count++;
    ready_queue[head] = *job;
    head++;
    if (head == MAX_JOB_NUMBER) {
        head = 0;
    }
    switch (scheduleType) {
        case SJF:
            tail = 0;
            insertionSortSJF(&tempHead);
            head = tempHead;
            break;
        case PRI:
            insertionSortPri(&tempHead);
            tail = 0;
            head = tempHead;
            break;
        case FCFS:
        default:
            break;
    }

    pthread_mutex_unlock(&queue_mutex);
    return;


}

void helpInfo() {
    command = help;
    printf("Run JOB:\n");
    printf("run <batch_job | job> <time> <pri>: submit a job named <job>,\n");
    printf("\texecution time is <time>,\n");
    printf("\tpriority is <pri>.\n");
    printf("list: display the job status.\n");
    printf("Scheduling policy:\n");
    printf("\tfcfs: change the scheduling policy to FCFS.\n");
    printf("\tsjf: change the scheduling policy to SJF.\n");
    printf("\tpriority: change the scheduling policy to priority.\n");
    printf("test <benchmark> <policy> <num_of_jobs> <priority_levels>\n\t<min_CPU_time> <max_CPU_time>\n");
    printf("Quit Program:\n");
    printf("quit");
    return;
}

/*Command*/
void fcfs_command(enum Commands *command, int *tempHead) {
    (*command) = fcfs;
    scheduleType = FCFS;
    insertionSortFCFS(tempHead);
    tail = 0;
    head = (*tempHead);
}

void pri_command(enum Commands *command, int *tempHead) {
    (*command) = pri;
    scheduleType = PRI;
    insertionSortPri(tempHead);
    tail = 0;
    head = (*tempHead);
}

void sjf_command(enum Commands *command, int *tempHead) {
    (*command) = sjf;
    scheduleType = SJF;
    insertionSortSJF(tempHead);
    tail = 0;
    head = (*tempHead);
}

void quit_command() {
    calculateJobStats();
    printf("Total number of job submitted: %d\n", totalcount);
    printf("Total number of job completed: %d\n", totoalCompletedJobs);
    printf("Average turnaround time: %f seconds\n", avgTurnaround);
    printf("Average CPU time:%f seconds\n", avgCpuTime);
    printf("Average waiting time:%f seconds\n", avgWaitTime);
    printf("Throughput: %f No./second\n", avgThroughput);
    command = end;
    pthread_cond_signal(&queue_threshold_cv);
    pthread_exit(NULL);
}

void run_command(char *const *commandv, int commandc) {
    command = run;
    if (commandc < 4 || commandc > 4) {
        printf("Error: Number of Run %d of 4 parameters entered.\n", commandc);
        printf("run <job> <time> <pri>: submit a job named <job>,\n");
    } else {
        send_job(commandv);
    }
}

void send_job(char *const *commandv) {
    JobPtr job = createJob(commandv);
    strcpy(job->status, "wait");
    schedule(job);
}

JobPtr createJob(char *const *commandv) {
    JobPtr job = (JobPtr) malloc(sizeof(struct Job));
    char my_job[10];
    strcpy(job->name, commandv[1]);
    job->cputime = atoi(commandv[2]);
    job->priority = atoi(commandv[3]);
    if (strcmp(job->name, "batch_job") == 0) {
        sprintf(my_job, "%s", job->name);
    } else {
        if (fp) {
            fprintf(fp, "Dispatcher only supports the ./batch_job program\n");
            fprintf(fp, "%s replaced with ./batch_job program\n", job->name);
        }
        printf("Dispatcher only supports the ./batch_job program\n");
        printf("%s replaced with ./batch_job program\n", job->name);
        sprintf(my_job, "%s", "batch_job");
    }
    job->wait_time = 0;
    job->arrival_time = 0;
    job->turnarount_time = 0;
    job->number = totalcount++;
    return job;
}

void list_command(char *commandv, int commandc) {
    command = list;
    if (commandc == 2) {
        if (strcmp(commandv, "-r") == 0) {
            while (count > 0) {
                printReadyQueue();
                printCompleteQueue();
                sleep(2);
            }
        } else {
            printReadyQueue();
            printCompleteQueue();
        }
    } else {
        printReadyQueue();
        printCompleteQueue();
    }
}

/*Command Line and Parser*/
void commandline() {
    printf("\n>");
    char *buffer;
    char *commandv[7] = {NULL};
    size_t bufsize = 32;
    int commandc = -1;
    buffer = (char *) malloc(bufsize * sizeof(char));
    enum Commands command = init;
    int tempHead;
    do {
        if (testRunning == 1 && count > 0) {
            printf("\nBenchmark Test Currently Running\n");
            printf("Limited Commands Allowed\n");
            printf("list , help , quit\n");
            printf("\n>");
        }
        if (testDone == 1) {
            printf("Benchmark Test Done Running\n");
            printStats();
            printf("\n>");
            testDone = 0;
        }
        fgets(buffer, bufsize, stdin);
        commandParser(buffer, commandv, &commandc);

        if (commandv[0] != NULL) {
            if (strcmp(commandv[0], "help") == 0) {
                help_command(commandv[1], commandc);
            } else if (strcmp(commandv[0], "run") == 0) {
                if (testRunning != 1) {
                    run_command(commandv, commandc);
                } else { printf("Test Running: Command not accepted;"); }
            } else if (strcmp(commandv[0], "quit") == 0 || strcmp(commandv[0], "exit") == 0) {
                quit_command();
                testRunning = 0;
            } else if (strcmp(commandv[0], "sjf") == 0) {
                if (testRunning != 1) {
                    sjf_command(&command, &tempHead);
                } else { printf("Test Running: Command not accepted;"); }
            } else if (strcmp(commandv[0], "pri") == 0) {
                if (testRunning != 1) {
                    pri_command(&command, &tempHead);
                } else { printf("Test Running: Command not accepted;"); }
            } else if (strcmp(commandv[0], "list") == 0) {
                list_command(commandv[1], commandc);
            } else if (strcmp(commandv[0], "fcfs") == 0) {
                if (testRunning != 1) {
                    fcfs_command(&command, &tempHead);
                } else { printf("Test Running: Command not accepted;"); }
            } else if (strcmp(commandv[0], "test") == 0) {
                test_command(commandv, bufsize, commandc);
            } else if (strcmp(commandv[0], "job") == 0 || strcmp(commandv[0], "job") == 0) {
                printf("Error: No Command Job.\n");
                printf("run <job> <time> <pri>: submit a job named <job>,\n");
            } else {
                printf("Error: Command Error Please Check Command Parameters.\n");
            }
        }
        if (count == 0 && testDone == 1) {
            testRunning = 0;
            calculateJobStats();
            printFileStats(fp);
            command = test;
            if (fp) {
                fclose(fp);
            }
        }
        printf("\n>");
    } while (command != end);
    return;
}

void help_command(char *commandv, int commandc) {
    if (commandc == 1) {
        helpInfo();
    } else if (commandc == 2) {
        if (strcmp(commandv, "-list") == 0 || strcmp(commandv, "list") == 0 || strcmp(commandv, "-l") == 0) {
            printf("list: display the job status.\n");
        } else if (strcmp(commandv, "run") == 0 || strcmp(commandv, "-run") == 0 || strcmp(commandv, "-r") == 0) {
            printf("run <job> <time> <pri>: submit a job named <job>,\n");
        } else if (strcmp(commandv, "pri") == 0 || strcmp(commandv, "-p") == 0 || strcmp(commandv, "-pri") == 0) {
            printf("Scheduling policy:\n");
            printf("\tpriority: change the scheduling policy to priority.\n");
        } else if (strcmp(commandv, "fcfs") == 0 || strcmp(commandv, "-f") == 0 || strcmp(commandv, "-fcfs") == 0) {
            printf("Scheduling policy:\n");
            printf("\tfcfs: change the scheduling policy to FCFS.\n");
        } else if (strcmp(commandv, "sjf") == 0 || strcmp(commandv, "-s") == 0 || strcmp(commandv, "-sjf") == 0) {
            printf("Scheduling policy:\n");
            printf("\tsjf: change the scheduling policy to SJF.\n");
        } else if (strcmp(commandv, "test") == 0 || strcmp(commandv, "-test") == 0 || strcmp(commandv, "-t") == 0) {
            printf("Test Command:\n");
            printf("test <benchmark> <policy> <num_of_jobs> <priority_levels>\n\t<min_CPU_time> <max_CPU_time>");
        } else if (strcmp(commandv, "quit") == 0 || strcmp(commandv, "-quit") == 0 || strcmp(commandv, "-q") == 0) {
            printf("Quit Command:Exit the Program.\n");
        }
    } else {
        printf("Error: Help Command.\n");
        printf("help list, || run , sjf , pri , fcfs");
        printf("help -l,   || run , sjf , pri , fcfs");
    }
}

void test_command(char *const *commandv, size_t bufsize, int commandc) {
    command = test;
    if (commandc < 7 || commandc > 7) {
        printf("Error: Test Command.\n");
        printf("test <benchmark> <policy> <num_of_jobs> <priority_levels>\n");
        printf("\t<min_CPU_time> <max_CPU_time>");
    } else {
        printf("Warning: Test Command.\n");
        printf("Previous Statistics and Queue will be cleared.\n");
        printf("Only list and Help command will be allowed to be entered.\n");
        testRunning = 1;
        clearComplete_queue();
        initReadyQueue();
        clearStats();
        char filename[25];
        struct Benchmark *benchmark;
        JobPtr job = (JobPtr) malloc(sizeof(struct Job));
        benchmark = (struct Benchmark *) malloc(bufsize * sizeof(struct Benchmark));
        strcpy(benchmark->name, commandv[1]);
        strcpy(benchmark->sType, commandv[2]);
        sprintf(filename, "%s_%s.txt", benchmark->name, benchmark->sType);
        fp = fopen(filename, "w");
        if (strcmp(benchmark->sType, "fcfs") == 0) {
            scheduleType = FCFS;
        } else if (strcmp(benchmark->sType, "pri") == 0) {
            scheduleType = PRI;
        } else if (strcmp(benchmark->sType, "sjf") == 0) {
            scheduleType = SJF;
        }
        benchmark->num_of_jobs = atoi(commandv[3]);
        benchmark->priority_levels = atoi(commandv[4]);
        benchmark->min_CPU_time = atoi(commandv[5]);
        benchmark->max_CPU_time = atoi(commandv[6]);
        int pri_level = benchmark->priority_levels;
        int max_cpu = benchmark->max_CPU_time;
        int min_cpu = benchmark->min_CPU_time;
        int randBurst;
        srand(time(NULL));
        fprintf(fp, "Submitted Test Jobs: %s\n",benchmark->sType);
        fprintf(fp, "Number of Jobs:%d,Priority Levels:%d,Min CPU:%d Max CPU:%d\n",benchmark->num_of_jobs,benchmark->priority_levels,benchmark->min_CPU_time,benchmark->max_CPU_time);
        fprintf(fp, "Name \t\tCPU_Time \tPri \tProgress\n");
        for (int i = 0; i < benchmark->num_of_jobs; i++) {
            if ((max_cpu - min_cpu) == 0) {
                randBurst = rand() % max_cpu + 1;
            } else {
                randBurst = min_cpu + rand() % (max_cpu - min_cpu);
            }
            int randPri = rand() % pri_level + 1;
            strcpy(job->name, "batch_job");
            job->cputime = randBurst;
            job->priority = randPri;
            job->wait_time = 0;
            job->arrival_time = 0;
            job->turnarount_time = 0;
            job->number = totalcount++;
            strcpy(job->status, "wait");
            schedule(job);
            fprintf(fp, "%s %10d %14d %12s\n", job->name, job->cputime, job->priority, job->status);

        }
        free(benchmark);


    }


}

void commandParser(char *argument, char *param[], int *paramSize) {
    char *arg;
    int i = 0;
    argument[strcspn(argument, "\r\n")] = 0;
    arg = strtok(argument, " ");
    param[i] = arg;
    while (arg != NULL) {
        i++;
        arg = strtok(NULL, " ");
        param[i] = arg;
    }
    *paramSize = i;
    return;

}

void insertionSortSJF(int *currentHead) {
    int j;
    JobPtr tempJob = (JobPtr) malloc(sizeof(struct Job));
    for (int i = 0; i < MAX_JOB_NUMBER; i++) {
        j = i;
        while (j > 0 && ready_queue[j].cputime < ready_queue[j - 1].cputime) {
            *tempJob = ready_queue[j];
            ready_queue[j] = ready_queue[j - 1];
            ready_queue[j - 1] = *tempJob;
            j--;
        }
    }
    j = 0;
    for (int i = 0; i < MAX_JOB_NUMBER; i++) {
        if (ready_queue[i].cputime != 99999) {
            j++;
        }
    }
    *currentHead = j;

    free(tempJob);
}

void insertionSortPri(int *currentHead) {
    int j;
    JobPtr tempJob = (JobPtr) malloc(sizeof(struct Job));
    for (int i = 0; i < MAX_JOB_NUMBER; i++) {
        j = i;
        while (j > 0 && ready_queue[j].priority < ready_queue[j - 1].priority) {
            *tempJob = ready_queue[j];
            ready_queue[j] = ready_queue[j - 1];
            ready_queue[j - 1] = *tempJob;
            j--;
        }
    }
    j = 0;
    for (int i = 0; i < MAX_JOB_NUMBER; i++) {
        if (ready_queue[i].priority != 99999) {
            j++;
        }
    }
    *currentHead = j;

    free(tempJob);
}

void insertionSortFCFS(int *currentHead) {
    int j;
    JobPtr tempJob = (JobPtr) malloc(sizeof(struct Job));
    for (int i = 0; i < MAX_JOB_NUMBER; i++) {
        j = i;
        while (j > 0 && ready_queue[j].number < ready_queue[j - 1].number) {
            *tempJob = ready_queue[j];
            ready_queue[j] = ready_queue[j - 1];
            ready_queue[j - 1] = *tempJob;
            j--;
        }
    }
    j = 0;
    for (int i = 0; i < MAX_JOB_NUMBER; i++) {
        if (ready_queue[i].number != 99999) {
            j++;
        }
    }
    *currentHead = j;
    free(tempJob);
}

/*Consumer*/
void *dispatcher() {
    JobPtr currentJobPtr = (JobPtr) malloc(sizeof(struct Job));
    JobPtr completedJobPtr = (JobPtr) malloc(sizeof(struct Job));
    JobPtr jobPtr = (JobPtr) malloc(sizeof(struct Job));
    pthread_mutex_lock(&queue_mutex);
    while (command != end) {
        if (count == 0) {
            pthread_cond_wait(&queue_threshold_cv, &queue_mutex);
            if (count == 0) {
                pthread_exit(NULL);
            }
        }
        count--;
        if (strcmp(ready_queue[tail].status, "wait") == 0) {
            execute(currentJobPtr, completedJobPtr, jobPtr);
        } else {
            inc_reset_tail();
        }
        if (testRunning == 1 && count == 0) {
            printf("Benchmark Test Done Running\n");
            printf("Please Press Enter for Statistics\n");
            testDone = 1;
        }
    }
    pthread_exit(NULL);
}

void execute(JobPtr currentJobPtr, struct Job *completedJobPtr, JobPtr jobPtr) {
    strcpy(ready_queue[tail].status, "run");
    RunningJob = currentJobPtr;
    *jobPtr = ready_queue[tail];
    memcpy(currentJobPtr, jobPtr, sizeof(struct Job));
    clearJob(jobPtr);
    ready_queue[tail] = *jobPtr;
    inc_reset_tail();

    pthread_mutex_unlock(&queue_mutex);

    time_t start = time(NULL);
    dispatch(currentJobPtr);
    time_t end = time(NULL);
    int cpu = end - start;

    strcpy(currentJobPtr->status, "complete");
    currentJobPtr->actualCpuTime = cpu;
    currentJobPtr->turnarount_time = end - currentJobPtr->arrival_time;
    currentJobPtr->wait_time = currentJobPtr->turnarount_time - cpu;
    memcpy(completedJobPtr, currentJobPtr, sizeof(struct Job));

    insertCompleteQueue(completedJobPtr);
}

void dispatch(struct Job *job) {
    pid_t pid, c_pid;
    int status;
    char my_job[10];
    char buffer[3]; // 3 digit buffer
    sprintf(buffer, "%d", job->cputime);

    if (strcmp(job->name, "batch_job") == 0) {
        sprintf(my_job, "%s", job->name);
    } else {
        sprintf(my_job, "%s", "batch_job");
    }
    char *const parmList[] = {my_job, buffer, NULL};
    if ((pid = fork()) == -1)
        perror("fork error");
    else if (pid == 0) {
        execv(my_job, parmList);
    } else if (pid > 0) {
        //from manual page of https://linux.die.net/man/2/wait
        do {
            c_pid = waitpid(pid, &status, WUNTRACED | WCONTINUED);
            if (c_pid == -1) {
                perror("waitpid");
                exit(EXIT_FAILURE);
            }
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }


}

void insertCompleteQueue(struct Job *job) {
    struct complete_queue *job_node = (struct complete_queue *) malloc(sizeof(struct complete_queue));
    struct complete_queue *last = chead;
    job_node->job = *job;
    job_node->next_job = NULL;
    if (chead == NULL) {
        chead = job_node;
        return;
    }
    while (last->next_job != NULL) {
        last = last->next_job;
    }

    last->next_job = job_node;
    return;
}
