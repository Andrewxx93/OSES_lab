#include <stdio.h>
#include <math.h>
#include "tpl_os.h"


int main(void) {
    StartOS(OSDEFAULTAPPMODE);
    return 0;
}

DeclareAlarm(a500_msec);
DeclareTask(TaskBasic);
DeclareTask(TaskExtended);
DeclareEvent(LampOn);
DeclareEvent(LampOff);


TASK(TaskBasic){
    static int counter = 0;
    if ((counter%2)==0){
        printf("Even occurance: #%d\r\n",counter);
        SetEvent(TaskExtended,LampOn);
    }
    else{
        printf("Odd occurance: #%d\r\n",counter);
        SetEvent(TaskExtended,LampOff);
    }
    counter++;
    TerminateTask();
}


TASK(TaskExtended){

    EventMaskType received_mask;
    static int lamp = 0;
    while(1){
        WaitEvent(LampOn|LampOff);
        GetEvent(TaskExtended,&received_mask);
        if(received_mask & LampOn){
            ClearEvent(LampOn);
            lamp = 1;
            printf("The value is: %d",lamp);
        }
        if(received_mask & LampOff){
            ClearEvent(LampOff);
            lamp = 0;
            printf("The value is: %d",lamp);
        }

    }
    TerminateTask();

}


TASK(stop){
    CancelAlarm(a500_msec);
    printf("Shutting down the OS.Goodbye.");
    ShutdownOS(E_OK);
    TerminateTask();
}