@echo off
call ..\bin\buildsuper_x64-win.bat .\4coder_fleury.cpp
copy .\custom_4coder.dll ..\..\custom_4coder.dll
copy .\custom_4coder.pdb ..\..\custom_4coder.pdb