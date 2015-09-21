#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <curses.h>

static WINDOW *uhrwindow = NULL;
static int resized = 1; /* this will force a screen size fetch on start */
static int y, x, dy = 1, dx = 1;
static int ymax, xmax;
#define uhrw 11
#define uhrh 5

static void sigwinch(int sig) {
/* In a sense, this is the top half of the handler. */
	if (sig) resized = 1; 			/* for later */
	(void) signal(SIGWINCH, sigwinch);    /* some systems need this */
}

static void adjust(void) {
/* In a sense, this is the bottom half of the signal handler. */
  struct winsize size;

  if (ioctl(fileno(stdout), TIOCGWINSZ, &size) == 0) {
		ymax = size.ws_row;
		xmax = size.ws_col;
    resize_term(ymax, xmax);
    wrefresh(curscr);
	}
	resized = 0;
	/* reposition near top left */
	y = 1; x = 1;
	dy = 1; dx = 1;
}

static void cleanup(void) {
	endwin();
}

/* color pair symbolic names, don't touch lowest batch just in case */
#define RED (COLOR_PAIRS - 1)
#define YELLOW (COLOR_PAIRS - 2)
#define OFF (COLOR_PAIRS - 3)

static void init_colors(void) {
	bool color = has_colors();

	if (start_color() != OK) {
		fprintf(stderr, "Could not initialize colors, exiting.\n");
		exit(2);
	}

	/* make COLOR_WHITE a 15% grey (when supported) */
	init_color(COLOR_WHITE, 150, 150, 150);
	
	if (color) {
		init_pair(RED, COLOR_RED, COLOR_BLACK);
		init_pair(YELLOW, COLOR_YELLOW, COLOR_BLACK);
	} else { /* monochrome pairs for monochrome displays */
		init_pair(RED, COLOR_WHITE, COLOR_BLACK);
		init_pair(YELLOW, COLOR_WHITE, COLOR_BLACK);
	}
	/* The following makes sense when you consider:
	   There's black, white, and bright white. But what about
		 "bright black"? That's grey. When we use the OFF color
		 pair, it'll be grey.
	*/
	init_pair(OFF, COLOR_BLACK, COLOR_BLACK);
	/* finally, the cursor color: none */
	curs_set(0);
}

/* The final version of this output is 11 chars wide. */
static void show_uhr(int h, int m, int s) {
	int slight = (s + 1) % 2, /* lit: even */
			h1light = h / 5,
			h2light = h % 5,
			m1light = m / 5,
			m2light = m % 5;
	int i;	/* generic counter */
	
	if (!uhrwindow)
		uhrwindow = newwin(uhrh, uhrw, y, x);
	
	if (1 == resized) adjust(); /* handle window resize (or launch) */

	werase(uhrwindow);
	wrefresh(uhrwindow);

#define UHRCOLOR(c) wattrset(uhrwindow, COLOR_PAIR((c)) | A_BOLD)
#define UHRPUT(y, x, args...) mvwprintw(uhrwindow, y, x, ##args)

	UHRCOLOR(slight ? YELLOW : OFF);
	UHRPUT(0, 5, "o");

	for (i = 0; i < 4; i++) {
		UHRCOLOR(i + 1 > h1light ? OFF : RED);
		UHRPUT(1, i * 3, "[]");
	}
	for (i = 0; i < 4; i++) {
		UHRCOLOR(i + 1 > h2light ? OFF : RED);
		UHRPUT(2, i * 3, "[]");
	}
	/* The five-minute line varies in color */
	for (i = 0; i < 11; i++) {
		UHRCOLOR(i + 1 > m1light ? OFF : (i + 1) % 3 == 0 ? RED : YELLOW);
		/* Yes, the nested conditionals actually work in this macro! */
		UHRPUT(3, i, "|");
	}
	for (i = 0; i < 4; i++) {
		UHRCOLOR(i + 1 > m2light ? OFF : YELLOW);
		UHRPUT(4, i * 3, "[]");
	}
	/* check boundaries */
	if ((0 == y) || (y + uhrh + 1 > ymax))
		dy = -dy;
	if ((0 == x) || (x + uhrw + 1 > xmax))
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
	
	/* First, initialize ncurses. The signal and atexit handlers
	   assume this has already been done. */
	initscr();

	/* install the signal handler */
	signal(SIGWINCH, sigwinch);

	/* clean up ncurses before exit (and restore the known screen
	   state */
	atexit(cleanup);
	
	init_colors();
	
	for (;;) { /* forever */
		gettimeofday(&tv, NULL);
		/* Now parse tv into local HH:MM:SS */
		curtime = localtime(&tv.tv_sec);
		show_uhr(curtime->tm_hour,	curtime->tm_min, curtime->tm_sec);
		sleep(1);
	}

	return 0;
}
