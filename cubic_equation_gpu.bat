SET PATH=%PATH%;%cd%\MinGW\bin
gcc cubic_equation_gpu.c src/kernel_loader.c -o cubic_equation_gpu.exe -lOpenCL -Iinclude && cubic_equation_gpu.exe
cmd