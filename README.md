Cpu auto hotplug
================

Small program that disables some cores of the cpu depending on its load.


Installation
------------

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

After installation, the program is set run automatically at boot. You can start it manually with the command :
```
service cpu_auto_hotplug_daemon start
```
