//eventsignal.c,userspaceprogram//
#include "eventcommon.c"
#define ENAME "my_event"
int main()
{
	int id=0;
	printf("open begin\n");
	long res=event_open(ENAME,sizeof(ENAME),&id);
	if(res)
		printf("openres:%ld,id:%d\n",res,id);
	printf("open end\n");
	printf("sig begin\n");
	res=event_sig(id);
	if(res)
		printf("sig res:%ld,id:%d\n",res,id);
	printf("sig done\n");
	printf("close begin\n");
	res=event_close(id);
	if(res)
		printf("close res:%ld,id:%d\n",res,id);
	printf("close end\n");
	
}
