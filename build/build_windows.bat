set RPS_DIR=.

rem msdev RPS.dsw /MAKE ALL /REBUILD

copy %RPS_DIR%\Sensors\include\*.h %RPS_DIR%\include
copy %RPS_DIR%\Sensors\GetLoadAvg\include\*.h %RPS_DIR%\include
copy %RPS_DIR%\Sensors\GetFlowBW\include\*.h %RPS_DIR%\include
copy %RPS_DIR%\Mirror\include\*.h %RPS_DIR%\include
copy %RPS_DIR%\TimeSeries\include\*.h %RPS_DIR%\include
copy %RPS_DIR%\RPSInterface\include\*.h %RPS_DIR%\include
copy %RPS_DIR%\RemosInterface\include\*.h %RPS_DIR%\include

copy %RPS_DIR%\Sensors\GetLoadAvg\Debug\*.exe %RPS_DIR%\Debug
copy %RPS_DIR%\Sensors\GetLoadAvg\Debug\*.lib %RPS_DIR%\Debug

copy %RPS_DIR%\Sensors\GetFlowBW\Debug\*.exe %RPS_DIR%\Debug
copy %RPS_DIR%\Sensors\GetFlowBW\Debug\*.lib %RPS_DIR%\Debug

copy %RPS_DIR%\Mirror\Debug\*.lib %RPS_DIR%\Debug

copy %RPS_DIR%\TimeSeries\Debug\*.exe %RPS_DIR%\Debug
copy %RPS_DIR%\TimeSeries\Debug\*.lib %RPS_DIR%\Debug

copy %RPS_DIR%\RPSInterface\Debug\*.lib %RPS_DIR%\Debug

copy %RPS_DIR%\RemosInterface\Debug\*.lib %RPS_DIR%\Debug

copy %RPS_DIR%\PredComp\Debug\*.exe %RPS_DIR%\Debug

copy %RPS_DIR%\Sensors\GetLoadAvg\Release\*.exe %RPS_DIR%\Release
copy %RPS_DIR%\Sensors\GetLoadAvg\Release\*.lib %RPS_DIR%\Release

copy %RPS_DIR%\Sensors\GetFlowBW\Release\*.exe %RPS_DIR%\Release
copy %RPS_DIR%\Sensors\GetFlowBW\Release\*.lib %RPS_DIR%\Release

copy %RPS_DIR%\Mirror\Release\*.lib %RPS_DIR%\Release

copy %RPS_DIR%\TimeSeries\Release\*.exe %RPS_DIR%\Release
copy %RPS_DIR%\TimeSeries\Release\*.lib %RPS_DIR%\Release

copy %RPS_DIR%\RPSInterface\Release\*.lib %RPS_DIR%\Release

copy %RPS_DIR%\RemosInterface\Release\*.lib %RPS_DIR%\Release

copy %RPS_DIR%\PredComp\Release\*.exe %RPS_DIR%\Release

