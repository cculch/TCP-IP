#include <stdio.h>
#include <sys/time.h> 
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int lat_cal(int *ser, int *cli, int *diff);

int main(int argc, char* argv[]){
	int ser,cli,diff;
	ser = 10;
	cli = 3;
	diff = 0;

	lat_cal(&ser,&cli,&diff);

	printf("Answer: %d\n",diff);

	return 0;
}


int lat_cal(int *ser, int *cli, int *diff){
	*diff = 4;
	return 0;
}
