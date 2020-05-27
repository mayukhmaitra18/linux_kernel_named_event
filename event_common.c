//eventcommon.c,userspaceprogram//
#include<unistd.h>
#include<sys/syscall.h>
#include<stdio.h>
#include<sys/wait.h>
#define __NR_EVENT_OPEN 332
#define __NR_EVENT_CLOSE 333
#define __NR_EVENT_WAIT 334
#define __NR_EVENT_SIG 335
long event_open(char* name,int namelen,int* id)
{
	return syscall(__NR_EVENT_OPEN,name,namelen,&id);
}
long event_close(int id)
{
	return syscall(__NR_EVENT_CLOSE,id);
}
long event_sig(int id)
{
	return syscall(__NR_EVENT_SIG,id);
}
long event_wait(int id)
{
	return syscall(__NR_EVENT_WAIT,id);
}

