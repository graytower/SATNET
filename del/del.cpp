#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <cstring>
#include <sys/wait.h>
#define runCmd_noReply(m) run_cmd(m,"1> /dev/null 2> /dev/null") 

using namespace std;

void run_cmd(char *_cmd,const char *attr)
{
	char cmd[100];
	sprintf(cmd,"%s %s",_cmd,attr);
	if(system(cmd) < 0){
		printf("%s error:%s\n",cmd,strerror(errno));
	}
}
int main()
{
	int i,j;
	for(i=1;i<49;i++)
	{
		 for(j=0;j<6;j++)
		 {
		 	char cmd[100];
			sprintf(cmd,"ip link del sat%dp%d",i,j);
		 	runCmd_noReply(cmd);
		 }
			char cmd[100];
			sprintf(cmd,"ip netns del sat%d",i);
			runCmd_noReply(cmd);
			sprintf(cmd,"ip link del satbr%d",i);
			runCmd_noReply(cmd);
			sprintf(cmd,"ip link del sbr%d",i);
			runCmd_noReply(cmd);
	}

        for(int i=1;i<3;i++)
	{
		char cmd[100];
		sprintf(cmd,"ip link del hbr%d",i);
		runCmd_noReply(cmd);
		for(int j=0;j<6;j++)
		{
			char cmd[100];
			sprintf(cmd,"ip link del sbr%dp%d",i,j);
		 	runCmd_noReply(cmd);
			sprintf(cmd,"ip link del hbr%dp%d",i,j);
		 	runCmd_noReply(cmd);
		}
	}

	for(int i=1;i<3;i++)
	{
		char cmd[100];
		sprintf(cmd,"ip netns del host%d",i);
		runCmd_noReply(cmd);	
	}

	for(int i=1;i<=4;i++)
	{
		char cmd[100];
		int attackNum;
		switch(i)
		{
			case 1:
			{
				attackNum = 2;
				break;
			}
			case 2:
			{
				attackNum = 4;
				break;
			}
			case 3:
			{
				attackNum = 6;
				break;
			}
			case 4:
			{
				attackNum = 7;
				break;
			}			
		}
		sprintf(cmd,"ip netns del attack%d",attackNum);
		runCmd_noReply(cmd);
		sprintf(cmd,"ovs-vsctl del-br sbr%d",attackNum);
		runCmd_noReply(cmd);
		
	}

}

