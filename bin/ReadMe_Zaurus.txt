This is the Zaurus version of KO/Pi and KA/Pi (www.rogowski.info),
the platform independent PIM programs based on the PIM programs 
(KOrganizer and KAddressbook) of KDE (www.kdeorg). 

KO/Pi or KA/Pi are licensed under the GPL.
Please read the file kdepim/licence.txt

This ReadMe is included in two different packages.
Depending if the filename of the package, please read
section 2) or 3)

*****************************************************
2) Zaurus Sharp ROM, gcc 2.95 compiled
*****************************************************
Filename: kdepim_X.X.X_for_SharpROM.ipk.zip
Package contains:
kaddressbook_X.X.X_arm.ipk
korganizer-alarm_X.X.X_arm.ipk
kmicrokdelibs_X.X.X_arm.ipk
kmobilephoneaccess_X.X.X_arm.ipk
ksharpPIM-DTMaccess_X.X.X_arm.ipk
kopiemail_X.X.X_arm.ipk
pimTABicon_X.X.X_arm.ipk
korganizer_X.X.X_arm.ipk
pwmanager_X.X.X_arm.ipk

This package is for users of the Sharp ROM and clones.
Users of old OpenZaurus releases, which are gcc2.95
compiled can use it as well.

CONTENT:

File kmicrokdelibs_X.X.X_arm.ipk:
  The libs in this file are needed to run
  any of the included programs.
  Install it.

File kaddressbook_X.X.X_arm.ipk:
  Contains KA/Pi addressbook program and needed additional libs.

File korganizer-alarm_X.X.X_arm.ipk:
  Contains KO/Pi calendar program and needed additional libs.

File kopiemail_X.X.X_arm.ipk:
  Contains OM/Pi email program and needed additional libs.

File pwmanager_X.X.X_arm.ipk:
  Contains PwM/Pi password manager program.

File ksharpPIM-DTMaccess_X.X.X_arm.ipk:
  Contains needed program/lib to sync KA/Pi and KO/Pi
  with the Sharp PIM applications on the Zaurus,
  which use the new Sharp DMT Pim format.

File korganizer-alarm_X.X.X_arm.ipk:
   Provides an alarm applet to get informed about alarms.
   The Zaurus will wake up from suspend
   if an event triggers an alarm.

File kmobilephoneaccess_X.X.X_arm.ipk:
  Contains a command line tool for accessing mobile phones.
  It is used from Kx/Pi to sync with / export to
  mobile phones. It is not working very well, please do not
  expect too much.

File pimTABicon_X.X.X_arm.ipk:
  If you insatll a program of the package, you may get
  a new TAB wchich is called Pim.
  If you did not have had this TAB on your Z, that
  TAB will not display an icon.
  Install this small package to assihn an icon to 
  the new Pim TAB. 


Unzip package and install the apps in the usual way.

To get the character conversion in OM/Pi working, please download
at the sourceforge project site the package
sr-character-conversion_SharpROM_arm.ipk.zip
from the section "general files for KDE/Pim"
Instructions how to install this package are in a ReadMe in this file.

NOTE:
OM/Pi needs openssl, you can download an openssl package for the Z from here:
http://www.net.lib.utah.edu/zaurus/openssl-0.9.7a_0.9.7a_arm.ipk


*****************************************************
3) OpenZaurus ROM, gcc 3.xx compiled
*****************************************************
Filename: kdepim_X.X.X_for_OZ-gcc3xx.ipk.zip
Package contains:
kaddressbook_OZ-gcc3xx_X.X.X_arm.ipk
kmicrokdelibs_OZ-gcc3xx_X.X.X_arm.ipk
kopiemail_OZ-gcc3xx_X.X.X_arm.ipk
korganizer-alarm_OZ-gcc3xx_X.X.X_arm.ipk
korganizer_OZ-gcc3xx_X.X.X_arm.ipk
kmobilephoneaccess_OZ-gcc3xx_X.X.X_arm.ipk
pimTABicon_OZ-gcc3xx_X.X.X_arm.ipk
pwmanager_OZ-gcc3xx_X.X.X_arm.ipk


This package is for users of the Open Zaurus ROM 3.3.6,
compiled by coredump (aka known as Hentges-Rom).
The programs may run on the new
Open Zaurus Release 3.5.1 as well, but
they cannot be installed there properly,
because the package manager of OZ needs more
information in the ipk files to install them properly.

If you have Open Zaurus 3.5.1, you should use
the packages of the OZ 3.5.1 feed,
compiled and maintained by Hrw.
Do not ask the OZ people about problems with these
packages, they cannot help you.

CONTENT:

By the way, I cannot help you as well, because I
am not using OZ.

File kmicrokdelibs_OZ-gcc3xx_X.X.X_arm.ipk:
  The libs in this file are needed to run
  any of the included programs.
  Install it.

File kaddressbook_OZ-gcc3xx_X.X.X_arm.ipk:
  Contains KA/Pi addressbook program and needed additional libs.

File korganizer-alarm_OZ-gcc3xx_X.X.X_arm.ipk:
  Contains KO/Pi calendar program and needed additional libs.

File kopiemail_OZ-gcc3xx_X.X.X_arm.ipk:
  Contains OM/Pi email program and needed additional libs.

File pwmanager_OZ-gcc3xx_X.X.X_arm.ipk:
  Contains PwM/Pi password manager program.

File korganizer-alarm_OZ-gcc3xx_X.X.X_arm.ipk:
   Provides an alarm applet to get informed about alarms.
   The Zaurus will wake up from suspend
   if an event triggers an alarm.

File kmobilephoneaccess_OZ-gcc3xx_X.X.X_arm.ipk:
  Contains a command line tool for accessing mobile phones.
  It is used from Kx/Pi to sync with / export to
  mobile phones. It is not working very well, please do not
  expect too much.

File pimTABicon_OZ-gcc3xx_X.X.X_arm.ipk:
  If you insatll a program of the package, you may get
  a new TAB wchich is called Pim.
  If you did not have had this TAB on your Z, that
  TAB will not display an icon.
  Install this small package to assihn an icon to 
  the new Pim TAB. 

Unzip package and install the apps in the usual way. 

To get the character conversion in OM/Pi working, please download
at the sourceforge project site the package
oz-character-conversion_OZ-gcc3xx_arm.ipk.zip
from the section "general files for KDE/Pim"
Instructions how to install this package are in a ReadMe in this file.