set RPSDIR=%RPS_DIR%
set PPSDIR=%RPSDIR%

set PATH=%RPSDIR%\Scripts;%RPSDIR%\bin\%RPS_ARCH%\%RPS_OS%;%RPSDIR%\lib\%RPS_ARCH%\%RPS_OS%;%RPSDIR%\ResearchScripts;%PATH%

set PERLLIB=%RPS_DIR%\Scripts:${PERLLIB}

set RPS_NEWSTYLEOUTPUT=yes

set RPS_MINLOGLEVEL=100000

#set RPS_LOG=somefile

%RPS_DIR%\build\setup_default_ports_windows.bat


