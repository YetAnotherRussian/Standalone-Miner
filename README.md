## Builds in Directory
* **Windows:**    miner-win-64.exe
* **Linux:**      clang-miner-linux-64
* **Linux GUI:**  clang-gui-miner-linux-64

## Project Entry-Points
* **gminer.c** - Portable GUI Miner & Auto Claim (SDL)
* **miner.c**  - Just Portable Miner, console.

## Auto-Claim for Windows users
* **Install WGET** from http://downloads.sourceforge.net/gnuwin32/wget-1.11.4-1-setup.exe
* **rewards.txt** - A new rewards address is generated and appened to rewards.txt each time you start the miner, all claimed addresses during this session will be sent to the newly generated rewards address.
* **trans.txt**   - All API url calls that where executed are appended to this file, easy method of re-execution.
* **minted.txt**  - All minted private keys are appened to this file.

## Standalone Miner Compile
```
git clone https://github.com/vfcash/Standalone-Miner.git && cd Standalone-Miner
gcc -Ofast -fopenmp miner.c ecc.c base58.c -lm -o miner.exe
or with offloading
gcc -Ofast -fopenmp -lomptarget miner.c ecc.c base58.c -lm -o miner.exe
```

## MinGW on Windows (gcc)

http://www.codebind.com/cprogramming/install-mingw-windows-10-gcc/

```
1. Download and install MSYS-2 https://www.msys2.org/
2. Open MINGW64, install mingw-w64-x86_64-gcc, git via pacman
3. Clone the repo (git clone https://github.com/vfcash/Standalone-Miner.git && cd Standalone-Miner)
4. Build the binary (gcc -O3 -fopenmp -lomptarget miner.c ecc.c base58.c -lm -static -o miner.exe)
```
