#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "gpio.h"

// Parse a string ("rising", "falling", "both") and return
// the corresponding EDGE_* constant, or -1 if the string
// is invalid.
int parse_edge(const char *edge) {
	if (0 == strncmp(edge, "rising", EDGESTRLEN))
		return EDGE_RISING;
	else if (0 == strncmp(edge, "falling", EDGESTRLEN))
		return EDGE_FALLING;
	else if (0 == strncmp(edge, "both", EDGESTRLEN))
		return EDGE_BOTH;
	else
		return -1;
}

// Export a pin by writing to /sys/class/gpio/export.
void pin_export(int pin) {
	char *export_path;
	char *pin_path;

	export_path = (char *)malloc(strlen(GPIO_BASE) + strlen("export") + 2);
	sprintf(export_path, "%s/export", GPIO_BASE);

	pin_path = (char *)malloc(strlen(GPIO_BASE) + GPIODIRLEN + 2);
	snprintf(pin_path,
			strlen(GPIO_BASE) + GPIODIRLEN + 1,
			"%s/gpio%d", GPIO_BASE, pin);

	if (! is_dir(pin_path)) {
		FILE *fp;
		fp = fopen(export_path, "w");
		fprintf(fp, "%d\n", pin);
		fclose(fp);
	}

	free(pin_path);
	free(export_path);
}
 
// Set which signal edges to detect.
int pin_set_edge(int pin, int edge) {
	char *pin_path;
	int pin_path_len;
	FILE *fp;

	pin_path_len = strlen(GPIO_BASE) + GPIODIRLEN + strlen("edge") + 3;
	pin_path = (char *)malloc(pin_path_len);
	snprintf(pin_path, pin_path_len,
			"%s/gpio%d", GPIO_BASE, pin);

	if (! is_dir(pin_path)) {
		fprintf(stderr, "error: pin %d is not exported.\n", pin);
		exit(1);
	}

	snprintf(pin_path, pin_path_len,
			"%s/gpio%d/edge", GPIO_BASE, pin);

	fp = fopen(pin_path, "w");
	if (EDGE_RISING == edge)
		fprintf(fp, "rising\n");
	else if (EDGE_FALLING == edge)
		fprintf(fp, "falling\n");
	else if (EDGE_BOTH == edge)
		fprintf(fp, "both\n");
	else {
		fprintf(stderr, "error: pin %d: invalid edge mode",
				pin);
		exit(1);
	}

	fclose(fp);
	free(pin_path);
}

// Set pin as input or output.
int pin_set_direction(int pin, int direction) {
	char *pin_path;
	int pin_path_len;
	FILE *fp;

	pin_path_len = strlen(GPIO_BASE) + GPIODIRLEN + strlen("direction") + 3;
	pin_path = (char *)malloc(pin_path_len);
	snprintf(pin_path, pin_path_len,
			"%s/gpio%d", GPIO_BASE, pin);

	if (! is_dir(pin_path)) {
		fprintf(stderr, "error: pin %d is not exported.\n", pin);
		exit(1);
	}

	snprintf(pin_path, pin_path_len,
			"%s/gpio%d/direction", GPIO_BASE, pin);

	fp = fopen(pin_path, "w");
	if (DIRECTION_IN == direction)
		fprintf(fp, "in\n");
	else if (DIRECTION_OUT == direction)
		fprintf(fp, "out\n");
	else {
		fprintf(stderr, "error: pin %d: invalid direction\n",
				pin);
		exit(1);
	}

	fclose(fp);
	free(pin_path);
}

