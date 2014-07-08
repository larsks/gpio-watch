SRCS = gpio-watch.c
OBJS = $(SRCS:.c=.o)

all: gpio-watch

gpio-watch: $(OBJS)
	$(CC) -o $@ $(OBJS)

