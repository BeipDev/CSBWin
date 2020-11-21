REM Make a staging folder called ZipTemp that holds the install files & directory structure
mkdir ZipTemp
copy config.txt ZipTemp
copy CSBWin.exe ZipTemp
copy dungeon.dat ZipTemp
copy dungeon-csb.dat ZipTemp
copy graphics.dat ZipTemp
copy hcsb.* ZipTemp
copy mini.dat ZipTemp
copy PlayFile-DM.log ZipTemp
copy readme.txt ZipTemp
copy "Start new CSB Game.bat" ZipTemp
copy "Start new Kid Dungeon Game.bat" ZipTemp
copy "Watch DM Replay.bat" ZipTemp

del CSBWin_x86.zip

REM Generate the x86 Zip file of the install folder
powershell -command^
 Compress-Archive -Path ZipTemp\* CSBWin_x86.zip

REM Cleanup
rmdir /s /q ZipTemp

echo Zip Files Created
