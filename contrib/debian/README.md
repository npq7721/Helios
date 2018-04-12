
Debian
====================
This directory contains files used to package heliosd/helios-qt
for Debian-based Linux systems. If you compile heliosd/helios-qt yourself, there are some useful files here.

## helios: URI support ##


helios-qt.desktop  (Gnome / Open Desktop)
To install:

	sudo desktop-file-install helios-qt.desktop
	sudo update-desktop-database

If you build yourself, you will either need to modify the paths in
the .desktop file or copy or symlink your helios-qt binary to `/usr/bin`
and the `../../share/pixmaps/helios128.png` to `/usr/share/pixmaps`

helios-qt.protocol (KDE)

