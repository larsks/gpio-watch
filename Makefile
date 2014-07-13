SRCS = main.c gpio.c fileutil.c logging.c
OBJS = $(SRCS:.c=.o)
LIBS = -lrt

all: gpio-watch

gpio-watch: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

clean:
	rm -f $(OBJS)
	rm -f gpio-watch

