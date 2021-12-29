#include "jobs.h"

lklist jobs={NULL,NULL};

void addjob(job *j) {
    lkappend(&jobs,j);
}