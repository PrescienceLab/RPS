set RPS_DIR=.
set TYPE=DEBUG

msdev RPS.dsw /MAKE ALL /REBUILD

copy %RPS_DIR%\Sensors\include\*.h %RPS_DIR%\include
copy %RPS_DIR%\Sensors\GetLoadAvg\include\*.h %RPS_DIR%\include
copy %RPS_DIR%\Sensors\GetFlowBW\include\*.h %RPS_DIR%\include
copy %RPS_DIR%\Mirror\include\*.h %RPS_DIR%\include
copy %RPS_DIR%\TimeSeries\include\*.h %RPS_DIR%\include
copy %RPS_DIR%\RPSInterface\include\*.h %RPS_DIR%\include
copy %RPS_DIR%\RemosInterface\include\*.h %RPS_DIR%\include

copy %RPS_DIR%\Sensors\GetLoadAvg\%TYPE%\*.exe %RPS_DIR%\bin\I386\WIN32
copy %RPS_DIR%\Sensors\GetLoadAvg\%TYPE%\*.lib %RPS_DIR%\lib\I386\WIN32

copy %RPS_DIR%\Sensors\GetFlowBW\%TYPE%\*.exe %RPS_DIR%\bin\I386\WIN32
copy %RPS_DIR%\Sensors\GetFlowBW\%TYPE%\*.lib %RPS_DIR%\lib\I386\WIN32

copy %RPS_DIR%\Mirror\%TYPE%\*.lib %RPS_DIR%\lib\I386\WIN32

copy %RPS_DIR%\TimeSeries\%TYPE%\*.exe %RPS_DIR%\bin\I386\WIN32
copy %RPS_DIR%\TimeSeries\%TYPE%\*.lib %RPS_DIR%\lib\I386\WIN32

copy %RPS_DIR%\RPSInterface\%TYPE%\*.lib %RPS_DIR%\lib\I386\WIN32

copy %RPS_DIR%\RemosInterface\%TYPE%\*.lib %RPS_DIR%\lib\I386\WIN32

copy %RPS_DIR%\PredComp\%TYPE%\*.exe %RPS_DIR%\bin\I386\WIN32
