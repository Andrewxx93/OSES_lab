#include "tpl_os.h"
#include <stdio.h>
#include <sys/time.h>
DeclareAlarm(hmi_a);
DeclareAlarm(engine_a);
DeclareResource(hmi_engine);
DeclareTask(hmi);
DeclareTask(engine);
struct hmi_engine_shmem
{
  int tp;
  int es;
} shmem;
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
    d = (long)((end.tv_sec - start.tv_sec) * 1000000 +
               (end.tv_usec - start.tv_usec));
  }
}
TASK(hmi)
{
  static int tp = 0;
  int es;
  if (GetResource(hmi_engine) != E_OK)
    printf("ERROR (1)\r\n");
  else
  {

    shmem.tp = tp; /* Send tp to ENGINE */
    es = shmem.es; /* Get es from ENGINE */
    do_other_things(80000);
    
    
    if (ReleaseResource(hmi_engine) != E_OK)
      printf("ERROR (2)\r\n");
      printf("HMI Display: tp=%d es=%d\r\n", tp, es);
  }
  /* Generate a ramp on tp */
    if (tp < 100)
      tp += 10;

  TerminateTask();
}
TASK(engine)
{
  int tp;
  if (GetResource(hmi_engine) != E_OK)
    printf("ERROR (3)\r\n");
  else
  {
    tp = shmem.tp;
    if (shmem.es < tp * 100)
      shmem.es += 100; /* Engine response */
    do_other_things(1000);
    if (ReleaseResource(hmi_engine) != E_OK)
      printf("ERROR (4)\r\n");
  }
  
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