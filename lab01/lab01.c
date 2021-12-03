#include <stdio.h>
#include <math.h>
#include "tpl_os.h"

int main(void)
{
    StartOS(OSDEFAULTAPPMODE);
    return 0;
}

/*DeclareAlarm(a500_msec);
DeclareAlarm(a750_msec);
DeclareTask(TaskA);
DeclareTask(TaskB);
*/
TASK(TaskA){
    static int counter_task_A = 0;
    printf("Counter value task A: %d\r\n",counter_task_A);
    counter_task_A += 500;
    
    TerminateTask();
}
TASK(TaskB){
    static int counter_task_B = 1500;
    printf("Counter value task B: %d\r\n",counter_task_B);
    counter_task_B += 750;
    
    TerminateTask();


}

TASK(stop){
    CancelAlarm(a500_msec);
    CancelAlarm(a750_msec);
    printf("Shutdown the system.Bye Bye.\r\n");
    ShutdownOS(E_OK);
    TerminateTask();
}