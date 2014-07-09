SRCS = main.c gpio.c fileutil.c logging.c
OBJS = $(SRCS:.c=.o)

all: gpio-watch

gpio-watch: $(OBJS)
	$(CC) -o $@ $(OBJS)

clean:
	rm -f $(OBJS)
	rm -f gpio-watch

