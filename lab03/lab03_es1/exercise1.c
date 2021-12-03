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

    

int main(void)
{

    calibrate();
    StartOS(OSDEFAULTAPPMODE);
    return 0;
}

DeclareAlarm(A);
DeclareAlarm(B);
DeclareAlarm(C);
DeclareTask(tau_A);
DeclareTask(tau_B);
DeclareTask(tau_C);



TASK(tau_A)
{
    static int C_A = 200000; //200 msec = 200 000 us
    long time;
    struct timeval start;
    struct timeval end;

    gettimeofday(&start, NULL);
    do_other_things(C_A);
    gettimeofday(&end, NULL);

    // time = (long)((end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec)/1000);
    // printf(" task A eseguita.... tempo impiegato=%ld \r\n\n", time);

    TerminateTask();
}


TASK(tau_B)
{
    static int C_B = 700000; //700 msec = 700 000 us
    long time;
    struct timeval start;
    struct timeval end;

    gettimeofday(&start, NULL);   
    do_other_things(C_B);
    gettimeofday(&end, NULL);
    
    // time = (long)((end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec)/1000);
    // printf(" task B eseguita.... tempo impiegato=%ld \r\n\n", time);

    TerminateTask();
}

TASK(tau_C)
{

    static int C_C = 300000; //300 msec = 300 000 us
    long time;
    long tic;
    long dispatch = 0;
    struct timeval start;
    struct timeval end;
    GetAlarm(C,&tic);
    gettimeofday(&start, NULL);
    do_other_things(C_C);
    gettimeofday(&end, NULL);
    
    dispatch = 2800-tic*10;
    time = (long)((end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec)/1000);
    printf("#Activation Task C\t|execution time=%ld\t|response_time = %ld\r\n\n", time,dispatch+time);
    //printf("n. tics = %ld response_time = %ld\r\n\n",tic,dispatch+time);
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