/*
 * Copyright (c) 2014, Basile Maret <basile.maret@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
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

#define BUF_MAX			1024
#define PATH_SIZE		900
#define TIME_INTERVAL		100000		/* in usec */
#define CPU_COUNT		4

#define CPU_ONLINE		1
#define CPU_OFFLINE		0

#define DOWN_THRESHOLD		15		/* in percent */
#define UP_THRESHOLD		70
#define DOWN_DELAY		3		/* times TIME_INTERVAL */

#define DEBUGING_SCREEN		1

typedef struct cpu_st{
	int cpu;
	double load;
	int online;
} Cpu;

typedef struct stat_st{
	unsigned long long int total_tick;
	unsigned long long int total_tick_old;
	unsigned long long int idle;
	unsigned long long int idle_old;
} Stat;

char sysfs_path[] = "/sys/devices/system/cpu/cpu";

Cpu main_cpu;
Cpu cores[CPU_COUNT];

Stat *stats;

int down_delay = DOWN_DELAY;
int up_delay = 1;

void print_cpu_state(double load){
	/* Print the cpu information */
	system("clear");
	printf("======================================\n");
	printf("=         cpu load : %-16.2lf=\n", load);
	printf("=------------------------------------=\n");
	printf("=          [ %d  %d  %d  %d ]            =\n", cores[0].online, cores[1].online, cores[2].online, cores[3].online);
	printf("======================================\n");
}

void cpu_off(Cpu * cpu){
	/* Put the core of `cpu' offline */
	FILE *fp;
	char path[PATH_SIZE];
	char cpu_number[1];

	sprintf(cpu_number, "%d", cpu->cpu);

	strcpy(path, sysfs_path);
	strcat(path, cpu_number);
	strcat(path, "/online");

	fp = fopen (path, "w");
	if (fp == NULL)
	{
		perror ("Error");
		return;
	}

	fprintf(fp, "%d", CPU_OFFLINE);
	cpu->online = CPU_OFFLINE;

	fclose(fp);
	return;
}
void cpu_on(Cpu * cpu){
	/* Put the core of `cpu' online */
	FILE *fp;
	char path[PATH_SIZE];
	char cpu_number[1];

	sprintf(cpu_number, "%d", cpu->cpu);

	strcpy(path, sysfs_path);
	strcat(path, cpu_number);
	strcat(path, "/online");

	fp = fopen (path, "w");
	if (fp == NULL)
	{
		perror ("Error");
		return;
	}

	fprintf(fp, "%d", CPU_ONLINE);
	cpu->online = CPU_ONLINE;


	fclose(fp);
	return;
}

void unplug(){
	/* Look for an online cpu and put it offline */
	int i;
	for(i = CPU_COUNT - 1; i > 0; i--){
		if(cores[i].online){
			cpu_off(&cores[i]);
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
			cpu_on(&cores[i]);
			break;
		}
	}
	return;
}

int read_fields (FILE *fp, unsigned long long int *fields)
	/* Reads the fields of the stat file `fp' and put its content into `fields' */
{
	int retval;
	char buffer[BUF_MAX];


	if (!fgets (buffer, BUF_MAX, fp))
	{ perror ("Error"); }
	retval = sscanf (buffer, "cpu %Lu %Lu %Lu %Lu %Lu %Lu %Lu %Lu %Lu %Lu",
			&fields[0],
			&fields[1],
			&fields[2],
			&fields[3],
			&fields[4],
			&fields[5],
			&fields[6],
			&fields[7],
			&fields[8],
			&fields[9]);
	if (retval < 4) /* Atleast 4 fields is to be read */
	{
		fprintf (stderr, "Error reading /proc/stat cpu field\n");
		return 0;
	}
	return 1;
}

double get_load(FILE *fp, void *a_cpu){
	/* Returns the cpu load between this function call and the previous call of this function */

	unsigned long long int fields[10],del_total_tick, del_idle;
	int update_cycle = 0, i;
	Cpu *cpu = (Cpu *) a_cpu;

	fseek (fp, 0, SEEK_SET);
	fflush (fp);
	if (!read_fields (fp, fields))
	{ return; }

	for (i=0, stats->total_tick = 0; i<10; i++)
	{ stats->total_tick += fields[i]; }
	stats->idle = fields[3]; /* idle ticks index */

	del_total_tick = stats->total_tick - stats->total_tick_old;
	del_idle = stats->idle - stats->idle_old;

	stats->total_tick_old = stats->total_tick;
	stats->idle_old = stats->idle;

	return ((del_total_tick - del_idle) / (double) del_total_tick) * 100; /* 3 is index of idle time */

}

int main(int argc, char **argv){
	int err;
	int i;
	double load;
	int flag = 1;
	FILE *fp;
	main_cpu = (Cpu) {
		0,
			0,
			CPU_ONLINE
	};

	stats =  malloc(sizeof(Stat));

	for(i = 0; i < CPU_COUNT; i++){
		cores[i] = (Cpu) {
			.cpu = i,
				.load = 0,
				.online = CPU_ONLINE
		};
	}

	fp = fopen ("/proc/stat", "r");
	if (fp == NULL)
	{
		perror ("Error");
		return;
	}

	while (flag)
	{
		load = get_load(fp, &main_cpu);

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
			print_cpu_state(load);
		}

		usleep(TIME_INTERVAL);
	}
}


