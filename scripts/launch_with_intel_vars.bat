@echo off
setlocal enabledelayedexpansion

call "%I_MPI_ONEAPI_ROOT%/env/vars.bat"
mpiexec.exe %*