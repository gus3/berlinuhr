#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <ncurses.h>

static WINDOW *uhrwindow = NULL;

/* The text version of this output is 20 chars wide. */
void show_uhr(int h, int m, int s) {
	int slight = s%2,
			h1light = h/5,
			h2light = h%5,
			m1light = m/5,
			m2light = m%5;
	static int x=2, y=2;
	
	if (!uhrwindow)
		uhrwindow = newwin(7, 21, y, x);

	werase(uhrwindow);
	refresh();

#define UHRPUT(y, x, args...) mvwprintw(uhrwindow, y, x, ##args)

	UHRPUT(1, 1, "Second light: %s", slight ? "on " : "off");
	UHRPUT(2, 1, "Hour row 1: %d lit", h1light);
	UHRPUT(3, 1, "Hour row 2: %d lit", h2light);
	UHRPUT(4, 1, "Minute row 1: %d lit", m1light);
	UHRPUT(5, 1, "Minute row 2: %d lit", m2light);
	
	/* a final sanity check */
	UHRPUT(6, 1, "%02d:%02d", h1light*5+h2light,
		m1light*5+m2light);

	mvwin(uhrwindow, ++y, ++x);
	wrefresh(uhrwindow);
}

int main(int argc, char *argv[]) {
	struct timeval tv;
	struct tm *curtime;

	initscr();
	
	while (1==1) {
		gettimeofday(&tv, NULL);
		/* Now parse tv into local HH:MM:SS */
		curtime = localtime(&tv.tv_sec);
		show_uhr(curtime->tm_hour,	curtime->tm_min, curtime->tm_sec);
		sleep(1);
	}

	return 0;
}
