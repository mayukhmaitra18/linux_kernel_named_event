//eventwait.c,userspaceprogram//
#include "eventcommon.c"
#define ENAME "my_event"
int main()
{
	int i;
	int id=0;
	long res=event_open(ENAME,sizeof(ENAME),&id);
	if(res)
		printf("open res:%ld,id:%d\n",res,id);
	for(i=0;i<5;i++)
		{
			pid_t pid;
			if((pid =fork())==0)
				{
					printf("[%d]:callingwait\n",i);
					res=event_wait(id);//wait
					if(res)
						printf("wait%d:res:%ld,id:%d\n",i,res,id);
					printf("[%d]:return from wait\n",i);
					return 0;
					
				}
			
		}
		sleep(1);
	
}
