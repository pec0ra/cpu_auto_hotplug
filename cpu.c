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


char sysfs_path[] = "/sys/devices/system/cpu/cpu";

Cpu cpu;
Cpu cores[CPU_COUNT];

int down_delay = DOWN_DELAY;
int up_delay = 1;

void print_cpu_state(){
	system("clear");
	printf("======================================\n");
	printf("=         cpu load : %-16.2lf=\n", cpu.load);
	printf("=------------------------------------=\n");
	printf("=          [ %d  %d  %d  %d ]            =\n", cores[0].online, cores[1].online, cores[2].online, cores[3].online);
	printf("======================================\n");
}

void cpu_off(Cpu * cpu){
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

void *get_load(void *a_cpu){

	FILE *fp;
	unsigned long long int fields[10], total_tick, total_tick_old, idle, idle_old, del_total_tick, del_idle;
	int update_cycle = 0, i, flag = 1;
	Cpu *cpu = (Cpu *) a_cpu;

	fp = fopen ("/proc/stat", "r");
	if (fp == NULL)
	{
		perror ("Error");
		return;
	}


	if (!read_fields (fp, fields))
	{ return; }

	for (i=0, total_tick = 0; i<10; i++)
	{ total_tick += fields[i]; }
	idle = fields[3]; /* idle ticks index */

	while (flag)
	{
		usleep (TIME_INTERVAL);
		total_tick_old = total_tick;
		idle_old = idle;

		fseek (fp, 0, SEEK_SET);
		fflush (fp);
		if (!read_fields (fp, fields))
		{ return; }

		for (i=0, total_tick = 0; i<10; i++)
		{ total_tick += fields[i]; }
		idle = fields[3];

		del_total_tick = total_tick - total_tick_old;
		del_idle = idle - idle_old;

		cpu->load = ((del_total_tick - del_idle) / (double) del_total_tick) * 100; /* 3 is index of idle time */

		if(cpu->load < DOWN_THRESHOLD){
			if(down_delay){
				down_delay--;
			} else {
				unplug();
				down_delay = DOWN_DELAY;
			}
		}

		if(cpu->load > UP_THRESHOLD){
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

		//  		printf ("Total CPU Usage: %3.2lf%%\n", cpu->load);
		if(DEBUGING_SCREEN){
			print_cpu_state();
		}
		update_cycle++;
	}


	fclose (fp); /* Ctrl + C quit, therefore this will not be reached. We rely on the kernel to close this file */

	return;
}

int main(int argc, char **argv){
	int err;
	int i;
	cpu = (Cpu) {
		0,
			0,
			CPU_ONLINE
	};

	for(i = 0; i < CPU_COUNT; i++){
		cores[i] = (Cpu) {
			.cpu = i,
				.load = 0,
				.online = CPU_ONLINE
		};
	}

	get_load(&cpu);
}


