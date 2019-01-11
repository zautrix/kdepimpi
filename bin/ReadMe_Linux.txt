This is the Linux desktop (SuSE 9.2 or later) 
version of KO/Pi and KA/Pi (www.rogowski.info),
the platform independent PIM programs based on the PIM programs 
(KOrganizer and KAddressbook) of KDE (www.kdeorg). 

KO/Pi or KA/Pi are licensed under the GPL.
Please read the file kdepim/licence.txt


Contents of the packages and how to install:

*****************************************************
4) Linux Desktop (SuSE 9.2)
*****************************************************
Filename: KDE-Pim-Pi-X.X.XX-SuSE_9.2.i586.rpm
Package contains:
KAddessbook/Pi
KOrganizer/Pi
PasswordManager/Pi
KPhone/Pi
KTimeTracker/Pi
KOPieMail/Pi
and the needed microkde libs.

Download file and install it on console with
su
rpm -ihv KDE-Pim-Pi-X.X.XX-SuSE_9.2.i586.rpm
Note:
If you want to update, please do it with
rpm -Uhv KDE-Pim-Pi-X.X.XX-SuSE_9.2.i586.rpm
(Uninstall with rpm -e KDE-Pim-Pi-X.X.XX-SuSE_9.2)

Here is the information,
provided in the rpm file about the content:

This package contains the platform-independent PIM programs from
www.rogowski.info, compiled for SuSE 9.2:
KTimeTacker/Pi
KPhone/Pi
KAddressbook/Pi
KOrganizer/Pi
PasswordManager/Pi
KOPieMail/Pi

These applications do not need anything from the KDE-desktop
at all to run on Linux. However, there is a dependency from
two KDE libs, because a small command line program is included
to make it possible to sync with the KDE-desktop applications.

These applications are independent from the KDE-desktop
environment. That means, nothing of your existing
KDE-desktop setup will be changed, or any data
(calendar-addressbook) used by the KDE-desktop
applications will be changed or accessed.
These applications stores their data and config in
$HOME/kdepim/
However, because the same file format is used,
an easy exchange of data with the KDE-desktop
is possible.
A small command line program is included
to make it possible to sync with the KDE-desktop applications.
You do not need to call this program from the commandline,
it is called from the KDE-Pim/Pi apps when you choose there:
Sync with KDE_Desktop.
If something is going wrong, please start the
KDE-Pim/Pi program itself from the console to get detailed output.

After installation, you should have a
PIM-pi
folder in your KDE start menu, where you can
start the applications from.

These programs makes it possible to sync your Zaurus easily
(with the KDE-Pim/Pi programs running on the Zaurus)
with the KDE-desktop calendar/addressbook data.
If you want to use that, you have to update your
KDE-desktop to version 3.3.0 or higher.
SuSE 9.2 contains KDE 3.3.0 such that no update is needed.
Actually - after the (non difficult) configuration is set up -
with two mouseklicks on the Zaurus,
the Zaurus syncs with the corresponding KDE-Pim/Pi
program on the Linux Desktop which syncs automatically
with the KDE-desktop data.

If you want to use the KDE-desktop calendar/addressbook applications,
just install these apps in this package and use them as a syncing tool for the
Zaurus <-> KDE-desktop sync.
The sync requires a network connection from your Zaurus to
the PC. A detailed Sync HowTo is available in the
Help menu of the applications.

These applications makes it also possible, that you can sync
(or just export the data to) your mobile phone with your
data of the KDE-desktop calendar/addressbook applications.
This is tested and working for Nokia mobile phones,
it may work with others as well.
(More info about that: -> Sync HowTo)

NOTE:
When using SuSE 9.1 you have to update your KDE to 3.3.x
and you have to make an online update in SuSE 9.1 to make it
possible to get the infrared connection working, such that
you can sync your (Nokia) mobile phone via infrared.