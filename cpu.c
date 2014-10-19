/*
 * Copyright (c) 2014, Basile Maret <basile.maret@gmail.com>
 *
 * This file is part of cpu auto hotplug
 *
 * Cpu auto hotplug is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "cpu_io.h"

#define TIME_INTERVAL		100000		/* in usec */

#define DOWN_THRESHOLD		15		/* in percent */
#define UP_THRESHOLD		70
#define DOWN_DELAY		3		/* times TIME_INTERVAL */

#define DEBUGING_SCREEN		0


static Cpu cores[CPU_COUNT];

static Stat *stats;

static int down_delay = DOWN_DELAY;
static int up_delay = 1;

static FILE *fp;

void unplug(){
	/* Look for an online cpu and put it offline */
	int i;
	for(i = CPU_COUNT - 1; i > 0; i--){
		if(cores[i].online){
			cpu_off(cores, &cores[i]);
			break;
		}
	}
	return;
}
void plug(){
	/* Look for an offline cpu and put it online */
	int i;
	for(i = 1; i < CPU_COUNT; i++){
		if(!cores[i].online){
			cpu_on(cores, &cores[i]);
			break;
		}
	}
	return;
}


double get_load(FILE *fp, Stat *stats){
	/* Returns the cpu load between this function call and the previous call of this function */

	unsigned long long int fields[10],del_total_tick, del_idle;
	int i;

	fseek (fp, 0, SEEK_SET);
	fflush (fp);
	if (!read_fields (fp, fields))
	{ return 0; }

	for (i=0, stats->total_tick = 0; i<10; i++)
	{ stats->total_tick += fields[i]; }
	stats->idle = fields[3]; /* idle ticks index */

	del_total_tick = stats->total_tick - stats->total_tick_old;
	del_idle = stats->idle - stats->idle_old;

	stats->total_tick_old = stats->total_tick;
	stats->idle_old = stats->idle;

	return ((del_total_tick - del_idle) / (double) del_total_tick) * 100; /* 3 is index of idle time */

}

static void termination_handler (int signum)
{
	int i;
	for(i = 1; i < CPU_COUNT; i++){
		cpu_on(cores, &cores[i]);
	}
	free(stats);
	fclose(fp);
	if(DEBUGING_SCREEN){
		printf("program terminated\n");
	}
	exit(signum);
}

int run(){
	int i;
	double load;
	int flag = 1;

	if (signal (SIGINT, termination_handler) == SIG_IGN)
		signal (SIGINT, SIG_IGN);
	if (signal (SIGHUP, termination_handler) == SIG_IGN)
		signal (SIGHUP, SIG_IGN);
	if (signal (SIGTERM, termination_handler) == SIG_IGN)
		signal (SIGTERM, SIG_IGN);

	stats =  malloc(sizeof(Stat));

	for(i = 0; i < CPU_COUNT; i++){
		cores[i] = (Cpu) {
			.cpu = i,
				.load = 0,
				.online = CPU_ONLINE
		};
	}
	update_cpu_status(cores);

	fp = fopen ("/proc/stat", "r");
	if (fp == NULL)
	{
		perror ("Error");
		return 0;
	}

	while (flag)
	{
		load = get_load(fp, stats);

		if(load < DOWN_THRESHOLD){
			if(down_delay){
				down_delay--;
			} else {
				unplug();
				down_delay = DOWN_DELAY;
			}
		}

		if(load > UP_THRESHOLD){
			if(up_delay){
				up_delay = 0;
			} else {
				plug();
				down_delay = DOWN_DELAY;
			}
		} else {
			// If we only had a high load once, we don't hotplug
			if(!up_delay){
				up_delay = 1;
			}
		}

		if(DEBUGING_SCREEN){
			print_cpu_state(cores, load);
		}

		usleep(TIME_INTERVAL);
	}
	return 1;
}

