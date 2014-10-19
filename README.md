Cpu auto hotplug
================

Small program that disables some cores of the cpu depending on its load.


Installation
------------

Before installing, you might need to change the CPU\_COUNT in cpu.h acording to your hardware. The default is set for 4 cores. Be aware that cpu_info might not work properly with more than 4 cores.

Compile cpu auto hotplug with the command :
```
make
```

Then run as root the command :
```
make install
```

To uninstall enter the command as root :
```
make uninstall
```

Usage
-----

After installation, the program is set run automatically at boot. You can start it manually by entering the command as root :
```
service cpu_auto_hotplug_daemon start
```

To follow cpu auto hotplug activity, run the command in terminal :
```
cpu_info
```
And you will see a screen like this :
```
======================================
=         cpu load : 20.00           =
=------------------------------------=
=          [ 1  1  0  0 ]            =
======================================
```

The cpu load is the percentage of time the cpu spend busy during the last time interval (see TIME\_INTERVAL\_INFO in cpu_info.c)
The second part contains either a 1 for online or a 0 for offline for each of the cores.
