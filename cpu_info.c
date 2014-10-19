
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
#include <stdio.h>
#include <unistd.h>
#include "cpu.h"
#include "cpu_io.h"

#define TIME_INTERVAL_INFO 	500000

int main(int argc, char **argv){
	int i;
	int flag = 1;
	Cpu cores[CPU_COUNT];

	for(i = 0; i < CPU_COUNT; i++){
		cores[i] = (Cpu) {
			.cpu = i,
				.load = 0,
				.online = CPU_ONLINE
		};
	}

	while(flag){
		update_cpu_status(cores);
		print_cpu_state(cores, 0);
		usleep(TIME_INTERVAL_INFO);
	}
	return 1;
}
