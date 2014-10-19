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

#ifndef __FILE_CPU_IO_H
#define __FILE_CPU_IO_H
#include "cpu.h"

int update_cpu_status(Cpu *cores);

int read_fields (FILE *fp, unsigned long long int *fields);

void print_cpu_state(Cpu *cores, double load);

int cpu_off(Cpu *cores, Cpu * cpu);
int cpu_on(Cpu *cores, Cpu * cpu);

#endif  // __FILE_CPU_IO_H
