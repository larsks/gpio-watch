#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include <poll.h>

#define EDGE_RISING 0
#define EDGE_FALLING 1
#define EDGE_BOTH 2
#define EDGESTRLEN 8
#define GPIODIRLEN 8

#define OPTSTRING "D:e:"

#ifndef GPIO_BASE
#define GPIO_BASE "/sys/class/gpio"
#endif

#ifndef DEFAULT_SCRIPT_DIR
#define DEFAULT_SCRIPT_DIR "/etc/gpio-scripts"
#endif

char *script_dir = DEFAULT_SCRIPT_DIR;
int default_edge = EDGE_BOTH;

struct pin {
	int pin;
	int edge;
};

struct pin *pins = NULL;
int num_pins = 0;

int is_dir (const char *path) {
	struct stat buf;
	int err;

	err = stat(path, &buf);
	if (-1 == err) {
		return 0;
	}

	return S_ISDIR(buf.st_mode);
}

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

int pin_export(int pin) {
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
	fclose(fp);
	free(pin_path);
}

int main(int argc, char **argv) {
	struct pollfd *fdlist;
	int numfds = 0;
	int ch;
	int i;

	while (-1 != (ch = getopt(argc, argv, OPTSTRING))) {
		switch (ch) {
			case 'D':
				script_dir = strdup(optarg);
				break;
			case 'e':
				if (-1 == (default_edge = parse_edge(optarg))) {
					fprintf(stderr, "error: invalid edge value: %s\n", optarg);
					exit(1);
				}
				break;
		}
	}

	assert(script_dir != NULL);

	if (! is_dir(script_dir)) {
		fprintf(stderr, "error: script directory \"%s\" does not exist.\n",
				script_dir);
	}

	for (i=optind; i<argc; i++) {
		char *pos,
		     *pinspec;
		struct pin p;

		pinspec = strdup(argv[i]);
		pos = strchr(pinspec, ':');

		if (pos) {
			*pos = '\0';
			pos++;
			p.pin = atoi(pinspec);
			if (-1 == (p.edge = parse_edge(pos))) {
				fprintf(stderr, "error: unknown edge spec: %s\n",
						argv[i]);
				exit(1);
			}
		} else {
			p.pin = atoi(pinspec);
			p.edge = default_edge;
		}

		free(pinspec);

		num_pins++;
		pins = realloc(pins, sizeof(struct pin) * num_pins);
		pins[num_pins-1] = p;
	}

	for (i=0; i<num_pins; i++) {
		pin_export(pins[i].pin);
		pin_set_edge(pins[i].pin, pins[i].edge);
	}

	return 0;
}

