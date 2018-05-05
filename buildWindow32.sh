PATH=$(echo "$PATH" | sed -e 's/:\/mnt.*//g')
cd depends
make HOST=i686-w64-mingw32
cd ..
./autogen.sh
CONFIG_SITE=$PWD/depends/i686-w64-mingw32/share/config.site ./configure --prefix=/
make

