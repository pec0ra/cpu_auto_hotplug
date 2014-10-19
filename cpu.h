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

#ifndef __FILE_CPU_H
#define __FILE_CPU_H

#define CPU_ONLINE		1
#define CPU_OFFLINE		0
#define CPU_COUNT		4

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

#endif // __FILE_CPU_H
