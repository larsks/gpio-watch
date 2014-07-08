/*
 * gpio-watch, a tool for running scripts in response to gpio events
 * Copyright (C) 2014 Lars Kellogg-Stedman <lars@oddbit.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <assert.h>
#include <fcntl.h>
#include <getopt.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include "gpio.h"
#include "fileutil.h"

#define OPTSTRING "D:e:d"

#ifndef DEFAULT_SCRIPT_DIR
#define DEFAULT_SCRIPT_DIR "/etc/gpio-scripts"
#endif

char *script_dir = DEFAULT_SCRIPT_DIR;
int default_edge = EDGE_BOTH;
int detach = 0;

struct pin *pins = NULL;
int num_pins = 0;

// Run a script in response to an event.
void run_script (int pin, int value) {
	char *script_path,
	     pin_str[GPIODIRLEN],
	     value_str[2];
	int script_path_len;
	pid_t pid;
	int status;

	script_path_len = strlen(script_dir)
			+ GPIODIRLEN
			+ 2;
	script_path = (char *)malloc(script_path_len);

	snprintf(script_path, script_path_len,
			"%s/%d", script_dir, pin);

	if (! is_file(script_path)) {
		fprintf(stderr, "error: script \"%s\" does not exist\n",
				script_path);
		return;
	}

	snprintf(pin_str, GPIODIRLEN, "%d", pin);
	sprintf(value_str, "%d", value);

	if (0 == (pid = fork())) {
		int res;
		res = execl(script_path, script_path, pin_str, value_str, (char *)NULL);
		if (-1 == res) exit(255);
	}

	wait(&status);
	free(script_path);

}

// Loop forever monitoring pins for activity.
int watch_pins() {
	struct pollfd *fdlist;
	int i;
	char *pin_path;
	int pin_path_len;
	char valbuf[3];

	valbuf[2] = '\0';

	pin_path_len = strlen(GPIO_BASE) + GPIODIRLEN + strlen("value") + 3;
	pin_path = (char *)malloc(pin_path_len);

	fdlist = (struct pollfd *)malloc(sizeof(struct pollfd) * num_pins);
	for (i=0; i<num_pins; i++) {
		int fd;

		snprintf(pin_path, pin_path_len,
				"%s/gpio%d/value", GPIO_BASE, pins[i].pin);
		fd = open(pin_path, O_RDONLY);
		read(fd, valbuf, 2);
		fdlist[i].fd = fd;
		fdlist[i].events = POLLPRI;
	}

	while (1) {
		int err;

		err = poll(fdlist, num_pins, -1);
		if (-1 == err) {
			perror("poll");
			exit(1);
		}

		for (i=0; i<num_pins; i++) {
			printf("%d %d %d\n",
					pins[i].pin,
					fdlist[i].fd,
					fdlist[i].revents);
			if (fdlist[i].revents & POLLPRI) {
				lseek(fdlist[i].fd, 0, SEEK_SET);
				read(fdlist[i].fd, valbuf, 2);
				run_script(pins[i].pin,
						valbuf[0] == '1' ? 1 : 0);
			}
		}
	}
}

int main(int argc, char **argv) {
	struct pollfd *fdlist;
	int numfds = 0;
	int ch;
	int i;

	while (-1 != (ch = getopt(argc, argv, OPTSTRING))) {
		switch (ch) {
			case 'd':
				detach = 1;
				break;
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
		pin_set_direction(pins[i].pin, DIRECTION_IN);
	}

	if (detach) daemon(1, 0);

	watch_pins();

	return 0;
}

