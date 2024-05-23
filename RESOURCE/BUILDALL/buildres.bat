@echo off
copy /y ..\text7.bin text7bin
copy /y ..\text8.bin text8bin
copy /y ..\font1.dat font1dat
copy /y ..\font2.dat font2dat
copy /y ..\HSCOBJ.BIN hscobj14
del ?????scr
del ?????tga
del ?????hsc
copy /y ..\ansi1.scr ansi1scr
copy /y ..\ansi2.scr ansi2scr
copy /y ..\font1.tga font1tga
copy /y ..\font2.tga font2tga
copy /y ..\logo7.tga logo7tga
copy /y ..\logo8.tga logo8tga
copy /y ..\live2.hsc live2hsc
rem for the following lines Microsoft compress.exe tool required
if exist compress.exe compress.exe ..\ansi1.scr ansi1scr
if exist compress.exe compress.exe ..\ansi2.scr ansi2scr
if exist compress.exe compress.exe ..\font1.tga font1tga
if exist compress.exe compress.exe ..\font2.tga font2tga
if exist compress.exe compress.exe ..\logo7.tga logo7tga
if exist compress.exe compress.exe ..\logo8.tga logo8tga
if exist compress.exe compress.exe ..\live2.hsc live2hsc
copy /y ..\magic.bin coremake.dat
rem following loop may not work properly under pure DOS due to extended syntax
for %%a in (hscobj14 ?????bin ?????dat ?????scr ?????tga ?????hsc) do makedata.exe %%a
for %%a in (hscobj14 ?????bin ?????dat ?????scr ?????tga ?????hsc) do copy /b coremake.dat + %%a
