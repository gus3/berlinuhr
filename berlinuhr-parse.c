#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <ncurses.h>

static WINDOW *basewin;

void show_uhr(int h, int m, int s) {
	int slight = s%2,
			h1light = h/5,
			h2light = h%5,
			m1light = m/5,
			m2light = m%5;
	static int first_draw = 1;

	mvprintw(1, 0, "Second light: %s\n", slight==1 ? "on " : "off");
	/* a slight delay may result in a skipped 0-second */
	if ((s < 2) || (first_draw != 0)) {
		mvprintw(4, 0, "Minute row 1: %d lit", m1light);
		mvprintw(5, 0, "Minute row 2: %d lit", m2light);
		if ((m == 0) || (first_draw != 0)) {
			mvprintw(2, 0, "Hour row 1: %d lit", h1light);
			mvprintw(3, 0, "Hour row 2: %d lit", h2light);
		}
	}
	mvprintw(0, 0, "first_draw=%d", first_draw);
	move(0, 0);
	refresh();
	/* change first_draw to allow partial updates */
	if (1 == first_draw) first_draw = 0;
}

int main(int argc, char *argv[]) {
	struct timeval tv;
	struct tm *curtime;

	basewin = initscr();

	while (1==1) {
		gettimeofday(&tv, NULL);
		/* Now parse tv into local HH:MM:SS */
		curtime = localtime(&tv.tv_sec);

		show_uhr(curtime->tm_hour,	curtime->tm_min, curtime->tm_sec);
		sleep(1);
	}

	return 0;
}
