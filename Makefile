SRCS = main.c gpio.c fileutil.c logging.c
OBJS = $(SRCS:.c=.o)
LIBS = -lrt
# _DEFAULT_SOURCE (glibc >= 2.19) for clock_gettime(),
# strdup(), and daemon().
CFLAGS = -Wall -Wpedantic -D_DEFAULT_SOURCE --std=c99
INSTALL = install

prefix = /usr
bindir = $(prefix)/bin

all: gpio-watch

gpio-watch: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

clean:
	rm -f $(OBJS)
	rm -f gpio-watch

install: all
	$(INSTALL) -m 755 -d $(DESTDIR)$(bindir)
	$(INSTALL) -m 755 gpio-watch $(DESTDIR)$(bindir)
