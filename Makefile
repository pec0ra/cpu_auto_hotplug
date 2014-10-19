#
#  Copyright (c) 2014, Basile Maret <basile.maret@gmail.com>
# 
#  This file is part of cpu auto hotplug
# 
#  Cpu auto hotplug is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
# 
#  This program is distributed in the hope that it will be useful, but WITHOUT
#  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
#  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
#  more details.
# 
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.
# 

CC=gcc
CFLAGS = -Wall
FILES=cpu.c cpu_io.c

all: cpu_auto_hotplug cpu_info

cpu_auto_hotplug: cpu_auto_hotplug.c $(FILES)
	$(CC)  $(CFLAGS) -o $@ $@.c $(FILES)

cpu_info: cpu_info.c $(FILES)
	$(CC)  $(CFLAGS) -o $@ $@.c $(FILES)


rebuild: clean all

debug: clean $(FILES)
	$(CC) $(CFLAGS) -g -o $@ $(FILES)

install:
	cp cpu_info /usr/local/bin/
	cp cpu_auto_hotplug /usr/sbin/
	cp cpu_auto_hotplug_daemon /etc/init.d/
	chmod +x /etc/init.d/cpu_auto_hotplug_daemon
	update-rc.d -f cpu_auto_hotplug_daemon remove
	update-rc.d cpu_auto_hotplug_daemon defaults
uninstall: 
	rm /usr/local/bin/cpu_info
	rm /usr/sbin/cpu_auto_hotplug
	update-rc.d -f cpu_auto_hotplug_daemon remove
	rm /etc/init.d/cpu_auto_hotplug_daemon 
clean:
	rm -f *.o *~ cpu_info cpu_auto_hotplug debug
