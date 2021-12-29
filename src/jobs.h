#ifndef _JOBS_H
#define _JOBS_H

#include "lklist.h"
#include "types.h"

//List with current cmd handlers of commands running in the background
extern lklist jobs;

//Add job to the job list
void addjob(job *j);

#endif