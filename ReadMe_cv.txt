This package includes the character conversion tables of the gnu C++ compiler environment.
The sources can be found in every source distribution of the gnu C++ compiler.
This package is needed by the mail program OM/Pi.

How to install:

The size of the installed data is about 4 MB.

Install the ipk on SD card or in internal memory.
We assume, that all is installed on SD.
You will have now a dir
/mnt/card/QtPalmtop/lib/gconv


to finish installation, we have to create a symlink from
/usr/lib/gconv to the new gconv dir.
To create a symlink do the following:

Open console and type in console:

cd /usr/lib
su
ln -s /mnt/card/QtPalmtop/lib/gconv

Threre is no need to uninstall or reinstall this package, if you update OM/Pi.

