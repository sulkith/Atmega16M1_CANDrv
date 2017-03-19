copy CANDrv.* doxygen
cd doxygen
doxygen Doxyfile
cd latex
call make.bat
copy refman.pdf ..
cd ..
del CANDrv.h
del CANDrv.cpp
rd /S /Q latex
del CANDrv.pdf
rename refman.pdf CANDrv.pdf
cd ..
pause
