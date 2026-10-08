@echo off
cd /d "%~dp0"
gcc -std=c11 -Wall -Wextra -Wpedantic -Iinclude src/*.c -o Faz_Ai.exe -lbcrypt
if errorlevel 1 (
    echo Falha na compilacao. Confira o GCC/MinGW-w64 e as mensagens acima.
    pause
    exit /b 1
)
echo Compilado. Execute .\Faz_Ai.exe nesta pasta.
pause
