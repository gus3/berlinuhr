#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

/* Use the ncurses library's SIGWINCH handler to modify LINES and
	 COLUMNS. These are global to the ncurses library, so we can
	 read them anytime. */
#include <curses.h>
#include <sys/ioctl.h>

#undef TRUE
#undef FALSE

#define TRUE (1==1)
#define FALSE !(TRUE)

static void sigwinch(int sig); /* forward declaration */
static int resized = FALSE;

static void adjust(void) {
/* In a sense, this is the bottom half of the handler. */
    struct winsize size;

    if (ioctl(fileno(stdout), TIOCGWINSZ, &size) == 0) {
        resize_term(size.ws_row, size.ws_col);
        wrefresh(curscr);
		}
		resized = FALSE;
}

static void sigwinch(int sig) {
/* And this is the top half of the handler. */
		if (sig)
        resized = TRUE; 			/* for later */
				
    (void) signal(SIGWINCH, sigwinch);    /* some systems need this */
}

static WINDOW *blah;

void print_xy() {
	int x,y;
	char *t;
	
	if (resized)
		adjust(); /* update window size in ncurses */
	fflush(stdout);
	getmaxyx(blah, y, x);
	printf("lines = %d, columns = %d\n\r", y, x);
}

int main(int argc, char *argv[]) {
	signal(SIGWINCH, sigwinch); /* install the hook */
	/* don't assume envvars are correct; use internal instead */
//	use_env(FALSE);
	unsetenv("LINES");
	unsetenv("COLUMNS");
	use_env(TRUE);
	blah = initscr();
	
	printf("Starting the program:\r\n");
	print_xy();
	
	while(0==0) {
		print_xy();
		sleep(1);
	}
	return 0;
}
