
copy xp_variables.pri variables.pri 
qmake
nmake clean
nmake
copy me_variables.pri variables.pri 
qmake
nmake clean
nmake 
winclean
copy xp_variables.pri variables.pri 