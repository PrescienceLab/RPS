mkdir %RPS_INSTALL_DIR%
mkdir %RPS_INSTALL_DIR%\bin
mkdir %RPS_INSTALL_DIR%\lib
mkdir %RPS_INSTALL_DIR%\include
mkdir %RPS_INSTALL_DIR%\doc

copy %RPS_DIR%\bin\I386\WIN32\* %RPS_INSTALL_DIR%\bin
copy %RPS_DIR%\Scripts\* %RPS_INSTALL_DIR%\bin
copy %RPS_DIR%\lib\I386\WIN32\* %RPS_INSTALL_DIR%\lib
copy %RPS_DIR%\doc\* %RPS_INSTALL_DIR%\doc

copy %RPS_DIR%\README %RPS_INSTALL_DIR%
copy %RPS_DIR%\LICENSE %RPS_INSTALL_DIR%
copy %RPS_DIR%\ROADMAP %RPS_INSTALL_DIR%


echo set RPS_DIR=%RPS_INSTALL_DIR%> %RPS_INSTALL_DIR%\ENV.bat
echo set RPS_OS=WIN32>> %RPS_INSTALL_DIR%\ENV.bat
echo set RPS_ARCH=I386>> %RPS_INSTALL_DIR%\ENV.bat
type %RPS_DIR%\build\setup_rps_env_installed_windows.bat >>%RPS_INSTALL_DIR%\ENV.bat
type %RPS_DIR%\build\setup_default_ports_windows.bat >>%RPS_INSTALL_DIR%\ENV.bat

