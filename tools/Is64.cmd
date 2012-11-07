@echo off
set CC=cl

%CC% /nologo tools/Is64.cc >NUL

Is64

IF %ERRORLEVEL% == 1 (
	echo IS64=1
)
IF %ERRORLEVEL% == 0 (
	echo IS64=0
)

@del *.obj 
