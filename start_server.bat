@echo off
cd /d "%~dp0Backend"
gcc main.c module1.c module2.c module3.c module4.c -o app.exe
if errorlevel 1 (
    echo C compilation failed.
    pause
    exit /b 1
)
python server.py
pause
