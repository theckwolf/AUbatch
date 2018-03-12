//Author:Thomas Heckwolf
//Class:CS7500 Auburn University
//Project: Project 3: AUbatch - A Pthread-based Batch Scheduling System

#ifndef AUBATCH_AUBATCH_H
#define AUBATCH_AUBATCH_H
/*Structure Definitions*/
#define MAX_JOB_NUMBER  400

enum Commands {
    init, help, run, list, fcfs, sjf, pri, test, end
};
enum Scheduler {
    FCFS, SJF, PRI
};
enum Commands command = init;
enum Scheduler prev_scheduleType = FCFS;
enum Scheduler scheduleType = FCFS;

struct Job {
    char name[50];
    int cputime;
    int actualCpuTime;
    int priority;
    time_t arrival_time;
    float turnarount_time;
    float wait_time;
    struct tm time;
    char status[10];
    int number;
};
struct Benchmark {
    char name[50];
    char sType[50];
    int num_of_jobs;
    int priority_levels;
    int min_CPU_time;
    int max_CPU_time;
};
struct Job ready_queue[MAX_JOB_NUMBER];
struct complete_queue {
    struct Job job;
    struct complete_queue *next_job;
};
struct complete_queue *chead = NULL;
struct Job *RunningJob = NULL;
typedef struct Job *JobPtr;
/*Global Definitions*/
int totalcount = 0;
int testDone = 0;
int count = 0;
int head = 0;
int tail = 0;
int testRunning = 0;
float avgTurnaround = 0;
float avgWaitTime = 0;
float avgCpuTime = 0;
int totoalCompletedJobs = 0;
float avgThroughput = 0;
FILE *fp;

/*Pthread Condition and Mutex variables*/
pthread_mutex_t queue_mutex;
pthread_cond_t queue_threshold_cv;
pthread_mutex_t test_mutex;
pthread_cond_t test_threshold_cv;
/************ Function Prototypes *********************/
/*Helper Functions*/
void initReadyQueue();
void calculateJobStats();
void clearJob(struct Job *job);
void clearComplete_queue();
void clearStats();
void inc_reset_tail();
/*Print Functions*/
void printReadyQueue();
void printStats();
void printCompleteQueue();
void printFileStats(FILE *fp);

/*Producer - Scheduler*/
void *scheduler();

void schedule(struct Job *job);

void helpInfo();

void commandline();

void commandParser(char *argument, char *param[], int *paramSize);

void insertionSortSJF(int *currentHead);

void insertionSortPri(int *currentHead);

void insertionSortFCFS(int *currentHead);

/*Consumer - Dispatcher*/
void *dispatcher();

void dispatch(struct Job *job);

void insertCompleteQueue(struct Job *job);

void execute(JobPtr currentJobPtr, struct Job *completedJobPtr, JobPtr jobPtr);

/*Commands*/
void run_command(char *const *commandv, int commandc);

void quit_command();

void sjf_command(enum Commands *command, int *tempHead);

void pri_command(enum Commands *command, int *tempHead);

void fcfs_command(enum Commands *command, int *tempHead);

void list_command(char *commandv, int commandc);

void help_command(char *commandv, int commandc);

/*Helper Function*/
void inc_reset_tail();

/************ End Function Prototypes *********************/

void test_command(char *const *commandv, size_t bufsize, int commandc);

void send_job(char *const *commandv);

JobPtr createJob(char *const *commandv);


#endif //AUBATCH_AUBATCH_H
