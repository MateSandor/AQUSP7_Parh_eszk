SET PATH=%PATH%;%cd%\MinGW\bin
gcc harmadfoku_gpu.c src/kernel_loader.c -o harmadfoku_gpu.exe -lOpenCL -Iinclude && harmadfoku_gpu.exe
cmd