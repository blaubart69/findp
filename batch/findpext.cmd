@echo off
if "%1"=="" (
	echo usage: findpext {extension} [findp options]
	exit /B 1
)
%~dp0\findp.exe %2 %3 %4 %5 %6 %7 %8 %9 2>nul | findstr /I /R "\.%1$"