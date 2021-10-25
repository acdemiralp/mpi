@echo off
setlocal enabledelayedexpansion

call "%I_MPI_ONEAPI_ROOT%/env/vars.bat"
cd ..
if not exist build (mkdir build)
cd build
cmake ..