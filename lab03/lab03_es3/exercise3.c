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

        for(i=0; i<mul; i++);

        printf("Do other things i=%ld\r\n\n",i);

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
DeclareAlarm(initialization_alarm);
DeclareTask(tau_A);
DeclareTask(tau_B);
DeclareTask(tau_C);
DeclareTask(initialization_task);
DeclareMessage(message);
DeclareMessage(object);

  


int main(void)
{

    calibrate();
    StartOS(OSDEFAULTAPPMODE);
    return 0;
}


TASK(initialization_task)
{
    static unsigned int critical_section=0;
    SendMessage(message, &critical_section);
    printf("tsk init... counter=%d\r\n\n",critical_section);
    TerminateTask();
}


TASK(tau_A)
{
    
    unsigned int critical_section_A;
    static int C_A = 200000; //200 msec = 200 000 us
    long response;
    long tic;
    long dispatch;
    struct timeval start;
    struct timeval end;
    
    printf("#Starting task A\r\n\n");

    GetAlarm(A, &tic);
    dispatch=1000-tic*10;
    
    ReceiveMessage(object, &critical_section_A);

    if(critical_section_A==0){
        DisableAllInterrupts();
        critical_section_A=1;
        SendMessage(message, &critical_section_A);

        printf("CRITICAL_SECTION=%d Task A enters in the 'critical section'.  \r\n\n",critical_section_A);

        gettimeofday(&start, NULL);
        
        do_other_things(C_A);
        

        gettimeofday(&end, NULL);

        response = (long)((end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec)/1000 + dispatch);

        critical_section_A=0;
        
        printf("CRITICAL_SECTION=%d Task A exits from critical section.",critical_section_A);
        printf(" Response time=%ld \r\n\n", response);
        SendMessage(message, &critical_section_A);
        EnableAllInterrupts();

    }
    else {
        while(critical_section_A == 3) ReceiveMessage(object,&critical_section_A);

        if(critical_section_A==0)
        {
            DisableAllInterrupts();
            critical_section_A=1;
            SendMessage(message, &critical_section_A);

            printf("CRITICAL_SECTION=%d  Task A enters in the 'critical section'.  \r\n\n",critical_section_A);

            gettimeofday(&start, NULL);
            
            do_other_things(C_A);
            
            gettimeofday(&end, NULL);

            response = (long)((end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec)/1000 + dispatch);

            
            critical_section_A=0;
            printf("CRITICAL_SECTION=%d Task A exits from critical section.",critical_section_A);
            printf(" Response time=%ld \r\n\n", response);

            
            SendMessage(message, &critical_section_A);
            EnableAllInterrupts();
        }
    }

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
    printf("#Starting task B\r\n\n");
    do_other_things(C_B);

    gettimeofday(&end, NULL);

    response = (long)((end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec)/1000 + dispatch);
    printf("Task B completed. Response time=%ld \r\n\n", response);
    
    TerminateTask();
}

TASK(tau_C)
{
    
    unsigned int critical_section_C;
    static int C_C = 300000; //300 msec = 300 000 us 
    long response;
    long tic;
    long dispatch;
    struct timeval start;
    struct timeval end;

    GetAlarm(C, &tic);
    dispatch=2800-tic*10;
    
    printf("#Starting Task C\r\n\n");

    gettimeofday(&start, NULL);
    
    do_other_things(100000);

    ReceiveMessage(object, &critical_section_C);

    if(critical_section_C==0){
        DisableAllInterrupts();
        critical_section_C=3;
        SendMessage(message, &critical_section_C);
        printf("CRITICAL_SECTION=%d Task C enters in the 'critical section'.  \r\n\n",critical_section_C);

        
        do_other_things(C_C-100000);
        
        gettimeofday(&end, NULL);

        response = (long)((end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec)/1000 + dispatch);
        
        critical_section_C=0;
        printf("CRITICAL_SECTION=%d Task C exits from 'critical section'. ",critical_section_C);
        printf(" Response time=%ld \r\n\n", response);

        SendMessage(message, &critical_section_C);
        EnableAllInterrupts();
    }
    else {
        while(critical_section_C == 1) ReceiveMessage(object,&critical_section_C);

        if(critical_section_C==0)
        {
            DisableAllInterrupts();
            critical_section_C=3;
            SendMessage(message, &critical_section_C);
            printf("CRITICAL_SECTION=%d Task C enters in the 'critical section'.  \r\n\n",critical_section_C);
            
            do_other_things(C_C-100000);
            
            gettimeofday(&end, NULL);

            response = (long)((end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec)/1000 + dispatch);
            
            critical_section_C=0;
            printf("CRITICAL_SECTION=%d Task C exits from 'critical section'. ",critical_section_C);
            printf(" Response time=%ld \r\n\n", response);
            
            
            SendMessage(message, &critical_section_C);
            EnableAllInterrupts();
        }
    }
       
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