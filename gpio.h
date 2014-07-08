#ifndef _GPIO_H
#define _GPIO_H

#define EDGE_RISING 0
#define EDGE_FALLING 1
#define EDGE_BOTH 2
#define EDGESTRLEN 8

#define DIRECTION_IN 0
#define DIRECTION_OUT 1

#define EDGESTRLEN 8
#define DIRSTRLEN 4
#define GPIODIRLEN 8

#ifndef GPIO_BASE
#define GPIO_BASE "/sys/class/gpio"
#endif

struct pin {
	int pin;
	int edge;
};

#endif // _GPIO_H

