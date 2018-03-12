#Author:Thomas Heckwolf
#Class:CS7500 Auburn University
#Project: Project 3: AUbatch -­‐ A Pthread-­‐based Batch Scheduling System
CC =gcc 
CC_FLAGS = -std=c99 -pthread -lm
AU_FILES = aubatch.c
JOB_FILES = batch_job.c
OUT_EXE = aubatch
JOB_OUT_EXE = batch_job

all: aubatch job

aubatch: $(AU_FILES)
	$(CC)$(CC_FLAGS) -o $(OUT_EXE) $(AU_FILES) 

job: $(JOB_FILES)
	$(CC)$(CC_FLAGS) -o $(JOB_OUT_EXE) $(JOB_FILES)
clean:
	rm -f *.o aubatch
	rm -f *.o batch_job
