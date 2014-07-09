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

#ifndef _LOGGING_H
#define _LOGGING_H

#define LEVEL_WARN  1
#define LEVEL_INFO  2
#define LEVEL_DEBUG 3

#define LOG_ERROR(fmt, ...) \
	logprint("%s %s:%d " fmt "\n", \
	logtime(), __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...)  if (loglevel >= LEVEL_WARN)  \
	logprint("%s %s:%d " fmt "\n", logtime(), __FUNCTION__, \
	__LINE__, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...)  if (loglevel >= LEVEL_INFO)  \
	logprint("%s %s:%d " fmt "\n", logtime(), __FUNCTION__, \
	__LINE__, ##__VA_ARGS__)
#define LOG_DEBUG(fmt, ...) if (loglevel >= LEVEL_DEBUG) \
	logprint("%s %s:%d " fmt "\n", logtime(), __FUNCTION__, \
	__LINE__, ##__VA_ARGS__)

extern int loglevel;

/* logging.c */
void logprint(const char *fmt, ...);
const char *logtime(void);

#endif // _LOGGING_H

