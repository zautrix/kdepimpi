
cd bin
rmdir /S /Q CVS
rmdir /S /Q winicons
rmdir /S /Q kdepim\CVS
rmdir /S /Q kdepim\pwmanager
rmdir /S /Q kdepim\kopiemail
rmdir /S /Q kdepim\kaddressbook\CVS
rmdir /S /Q kdepim\korganizer\CVS
del /Q kdepim\Make*.*
del /Q *.exp
del /Q *.lib
del /Q *.manifest
del /Q ReadMe_Linux.txt
del /Q ReadMe_Zaurus.txt
del /Q ReadMe_Memorystick.txt

cd ..