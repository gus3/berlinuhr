#include <stdio.h>
#include <time.h>
#include <sys/time.h>

int main(int argc, char *argv[]) {

struct timeval tv;
struct tm *curtime;
gettimeofday(&tv, NULL);

/* Now parse tv into local HH:MM:SS */

curtime = localtime(&tv.tv_sec);

printf("HH:MM:SS = %02d:%02d:%02d\n", curtime->tm_hour,
	curtime->tm_min, curtime->tm_sec);

return 0;
}
