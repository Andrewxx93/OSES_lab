#include <time.h>
#include <sys/time.h>
#include "tpl_os.h"
#include <stdio.h>
    long sf = 1;
    

    /* Keep the caller busy for howmany_us microseconds */
    void do_other_things(int howmany_us)
    {
        long mul = howmany_us * sf;
        long i;

        for(i=0; i<mul; i++) time(NULL);

    }

    #define CAL_US 1000000
    #define CAL_T 5

    void calibrate(void)
    {
        struct timeval now;
        long nsf = 0;

        printf("Calibrating delay... \r\n");
        
        while(nsf == 0 || nsf - sf > CAL_T || sf - nsf > CAL_T)
        {
            struct timeval old, new;

            long l;

            gettimeofday(&old, NULL);

            do_other_things(CAL_US);

            gettimeofday(&new, NULL);

            l = (long)((new.tv_sec - old.tv_sec) * 1000000 + (new.tv_usec - old.tv_usec));

            nsf = CAL_US * sf / l;

            sf = (sf + nsf)/2;

            printf("... l= %ld sf= %ld\r\n", l, sf);
        }
    }

DeclareAlarm(A);
DeclareAlarm(B);
DeclareAlarm(C);
DeclareTask(tau_A);
DeclareTask(tau_B);
DeclareTask(tau_C);
DeclareResource(pippo);

int counter;


int main(void)
{

    calibrate();
    StartOS(OSDEFAULTAPPMODE);
    return 0;
}





TASK(tau_A)
{
    static int C_A = 200000; //200 msec = 200 000 us
    long response;
    long tic;
    long dispatch;
    struct timeval start;
    struct timeval end;
    
    GetAlarm(A, &tic);
    dispatch=1000-tic*10;

    gettimeofday(&start, NULL);

    GetResource(pippo);
    printf("task A entra nella critical section\r\n\n");
    counter=1;

    do_other_things(C_A);


    gettimeofday(&end, NULL);

    response = (long)((end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec)/1000 + dispatch);
    printf("task A esce dalla critical section. Task A completata. Tempo impiegato=%ld \r\n\n", response);

    ReleaseResource(pippo);

    TerminateTask();
}


TASK(tau_B)
{
    static int C_B = 700000; //700 msec = 700 000 us
    long response;
    long tic;
    long dispatch;
    struct timeval start;
    struct timeval end;

    GetAlarm(B, &tic);
    dispatch=1500-tic*10;

    gettimeofday(&start, NULL);
        
    do_other_things(C_B);
    
    gettimeofday(&end, NULL);

    response = (long)((end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec)/1000 + dispatch);
    printf("Task B completata. Tempo impiegato=%ld \r\n\n", response);
    
    TerminateTask();
}

TASK(tau_C)
{
    static int C_C = 300000; //300 msec = 300 000 us 
    long response;
    long tic;
    long dispatch;
    struct timeval start;
    struct timeval end;

    GetAlarm(C, &tic);
    dispatch=2800-tic*10;
    

    gettimeofday(&start, NULL);

    do_other_things(100000);

    printf("task C entra nella critical section\r\n\n");
    GetResource(pippo);
        
    counter=3;

    do_other_things(200000);

       
    gettimeofday(&end, NULL);

    response = (long)((end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec)/1000 + dispatch);
    printf("task C esce dalla critical section. Task C completata. Tempo impiegato=%ld \r\n\n", response);

    ReleaseResource(pippo);
          
    TerminateTask();
}

TASK(stop)
{
  CancelAlarm(A);
  CancelAlarm(B);
  CancelAlarm(C);
  printf("Shutdown\r\n");
  ShutdownOS(E_OK);
  TerminateTask();
}