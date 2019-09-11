rem Step 1: Downdload mingw-w64 from here: https://sourceforge.net/projects/mingw-w64/
rem Step 2: Settings for installation: Version: 8.1.0 (or later); Architecture: x86_64; Threads: win32; Left others as default
rem Step 3: Update the path to gcc.exe in the line below (if needed)
rem Step 4: Launch this .bat file to compile
rem Step 5: Use miner_ryzen.exe to mine 

rem Average hashrate for Ryzen 7 2700x (no OC, mem 2666Mhz) using such a build: ~32800 H/s

"C:\Program Files\mingw-w64\x86_64-8.1.0-win32-seh-rt_v6-rev0\mingw64\bin\gcc.exe" -fopenmp -m64 -Ofast -flto -march=znver1 -funroll-loops -msha miner.c ecc.c base58.c -lm -o miner_ryzen.exe

pause