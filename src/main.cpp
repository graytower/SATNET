#include <string.h>
#include <stdio.h>
#include <iostream>
#include <unistd.h>

#include "common.h"
#include "cmd.h"
#include "timer.h"


using namespace std;


int main(int argc,char *argv[])
{
	
	TopoInitialize();
	string s;
	cout << "Waiting for start..." << endl;
	do{
		cin >> s;
		//sys_stat=s;
	}while(s != "start");
	
	StartTimer();
	do{
		
		cin >> s;
		if(s == "stop"){
			EndTimer();	
			cout<<"task down!"<<"\n";	
		}
		else if(s == "start"){
			StartTimer();
		}
	}while(s != "exit");
	EndTimer();
	
	TopoFinalize();



	return 0;
}