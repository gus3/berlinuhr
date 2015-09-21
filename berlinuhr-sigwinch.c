#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <curses.h>

static WINDOW *uhrwindow = NULL;
static void sigwinch(int sig); /* forward declaration */
static int resized = 1; /* this will force a screen size fetch on start */
static int y, x, dy = 1, dx = 1;
static int ymax, xmax;
const static int uhrw = 21, uhrh = 6;

static void adjust(void) {
/* In a sense, this is the bottom half of the handler. */
  struct winsize size;

  if (ioctl(fileno(stdout), TIOCGWINSZ, &size) == 0) {
		ymax = size.ws_row;
		xmax = size.ws_col;
    resize_term(ymax, xmax);
    wrefresh(curscr);
	}
	resized = 0;
	/* restart to top left */
	y = 1; x = 1;
	dy = 1; dx = 1;
}

static void sigwinch(int sig) {
/* And this is the top half of the handler. */
	if (sig) resized = 1; 			/* for later */
	(void) signal(SIGWINCH, sigwinch);    /* some systems need this */
}

/* The text version of this output is 20 chars wide. */
void show_uhr(int h, int m, int s) {
	int slight = s%2,
			h1light = h/5,
			h2light = h%5,
			m1light = m/5,
			m2light = m%5;
	
	if (!uhrwindow)
		uhrwindow = newwin(uhrh, uhrw, y, x);
	
	if (1 == resized) adjust(); /* handle window resize (or launch) */

	werase(uhrwindow);
	wrefresh(uhrwindow);

#define UHRPUT(y, x, args...) mvwprintw(uhrwindow, y, x, ##args)

	UHRPUT(1, 1, "Second light: %s", slight ? "on " : "off");
	UHRPUT(2, 1, "Hour row 1: %d lit", h1light);
	UHRPUT(3, 1, "Hour row 2: %d lit", h2light);
	UHRPUT(4, 1, "Minute row 1: %d lit", m1light);
	UHRPUT(5, 1, "Minute row 2: %d lit", m2light);
	
	/* check boundaries */
	if ((0 == y) || (y + uhrh + 2 > ymax))
		dy = -dy;
	if ((0 == x) || (x + uhrw + 2 > xmax))
		dx = -dx;
	/* adjust position */
	y += dy;
	x += dx;
	mvwin(uhrwindow, y, x);
	wrefresh(uhrwindow);
}

int main(int argc, char *argv[]) {
	struct timeval tv;
	struct tm *curtime;
	
	/* install the signal handler */
	signal(SIGWINCH, sigwinch);

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
