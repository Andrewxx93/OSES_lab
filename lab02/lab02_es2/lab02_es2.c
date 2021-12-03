#include "tpl_os.h"
#include <stdio.h>
#include <sys/time.h>
DeclareAlarm(hmi_a);
DeclareAlarm(engine_a);
DeclareMessage(tp_send);
DeclareMessage(es_send);
DeclareMessage(tp_receive);
DeclareMessage(es_receive);
DeclareTask(hmi);
DeclareTask(engine);

int main(void)
{
  StartOS(OSDEFAULTAPPMODE);
  return 0;
}
/* Keep the caller busy for howmany_us microseconds */
void do_other_things(int howmany_us)
{
  struct timeval start, end;
  long d = 0;
  gettimeofday(&start, NULL);
  while (d < howmany_us)
  {
    gettimeofday(&end, NULL);
    d = (long)((end.tv_sec - start.tv_sec) * 1000000 +(end.tv_usec - start.tv_usec));
  }
}
TASK(hmi)
{
  static unsigned tp = 0;
  unsigned es=0;
  if (SendMessage(tp_send,&tp) != E_OK) //Send the tp value
    printf("ERROR (1)\r\n");
  else
  {
    if (ReceiveMessage(es_receive,&es) != E_OK)
      printf("ERROR (2)\r\n");
    else{
      printf("HMI Display: tp=%d es=%d\r\n", tp, es);
    }
    
    
  }
  /* Generate a ramp on tp */
  if (tp < 100)
    tp += 10;
  do_other_things(80000);
  TerminateTask();
}
TASK(engine)
{
  static unsigned int es=0;
  unsigned int tp = 0;
  if (ReceiveMessage(tp_receive,&tp) != E_OK)
    printf("ERROR (3)\r\n");
  else
  {
    if (es < tp * 100)
      es += 100; /* Engine response */
      
    if (SendMessage(es_send,&es) != E_OK)
      printf("ERROR (4)\r\n");
      
  }
  do_other_things(1000);
  TerminateTask();
}
TASK(stop)
{
  printf("STOP\r\n");
  CancelAlarm(hmi_a);
  CancelAlarm(engine_a);
  ShutdownOS(E_OK);
  TerminateTask();
}