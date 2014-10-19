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
#include "cpu.h"

#define BUF_MAX			1024
#define PATH_SIZE		900


char sysfs_path[] = "/sys/devices/system/cpu/cpu";

void print_cpu_state(Cpu *cores, double load){
	/* Print the cpu information */
	system("clear");
	printf("======================================\n");
	printf("=         cpu load : %-16.2lf=\n", load);
	printf("=------------------------------------=\n");
	printf("=          [ %d  %d  %d  %d ]            =\n", cores[0].online, cores[1].online, cores[2].online, cores[3].online);
	printf("======================================\n");
}

int update_cpu_status(Cpu *cores){
	/* Read the sysfs files to update the status of the cores */
	FILE *fp;
	Cpu *cpu;
	char path[PATH_SIZE];
	char cpu_number[1];
	int i, cpu_status;

	for(i=1;i<CPU_COUNT; i++){
		cpu = &cores[i];
		sprintf(cpu_number, "%d", cpu->cpu);

		strcpy(path, sysfs_path);
		strcat(path, cpu_number);
		strcat(path, "/online");

		fp = fopen (path, "r");
		if (fp == NULL)
		{
			perror ("Error");
			return 0;
		}

		if(!fscanf(fp, "%d", &cpu_status)){
			printf("Error : Couldn't read %s", path);
			return 0;
		}
		cpu->online = cpu_status;

		fclose(fp);
	}
	return 1;
}

int cpu_off(Cpu *cores, Cpu * cpu){
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
		return 0;
	}

	if(!fprintf(fp, "%d", CPU_OFFLINE)){
		printf("Error : Couldn't write %s", path);
		return 0;
	}
	update_cpu_status(cores);

	fclose(fp);
	return 1;
}
int cpu_on(Cpu *cores, Cpu * cpu){
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
		return 0;
	}

	if(!fprintf(fp, "%d", CPU_ONLINE)){
		printf("Error : Couldn't write %s", path);
		return 0;
	}
	update_cpu_status(cores);

	fclose(fp);
	return 1;
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

