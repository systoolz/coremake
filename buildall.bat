@echo off
if exist coremake.exe del coremake.exe
for %%a in (*.c) do if not %%a == playback.c TC201\BIN\tcc.exe -w -g1 -c -Z -O -mc -ITC201\INCLUDE %%a
TC201\BIN\tlink.exe @coremake.sln
if not exist coremake.exe goto quit
if exist APACKEXE\apack.exe APACKEXE\apack.exe -x -1 coremake.exe coremake.exe
copy /b coremake.exe + resource\buildall\coremake.dat
del *.obj
:quit
if not exist coremake.exe pause
