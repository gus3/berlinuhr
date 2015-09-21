#CFLAGS=-g
CFLAGS=-mtune=native
LDFLAGS=-lcurses

TARGETS = sigaction test berlinuhr-parse berlinuhr-window berlinuhr-sigwinch berlinuhr-final
TARGETS += colors colors-changing

all: $(TARGETS)

%: %.o
	ld -o $@ /lib/crt0.o -lc $(LDFLAGS) $<
#	gcc $(CFLAGS) -o $@ $(LDFLAGS) $<

%.o: %.c
	gcc $(CFLAGS) -c -o $@ $(CFLAGS) $<

clean:
	-rm -fv *.o $(TARGETS) cscope*
