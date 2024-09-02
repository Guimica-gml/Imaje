@echo off

cl.exe /W2 main.c /c /Fo:imaje.obj && link.exe /OUT:imaje.exe imaje.obj
