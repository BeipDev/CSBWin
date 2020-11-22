REM Make a staging folder called ZipTemp that holds the install files & directory structure
mkdir ZipTemp
mkdir ZipTemp\DM
copy DM ZipTemp\DM
mkdir ZipTemp\CSB
copy CSB ZipTemp\CSB

copy config.txt ZipTemp
copy CSBWin.exe ZipTemp
copy hcsb.* ZipTemp
copy mini.dat ZipTemp
copy readme.txt ZipTemp
copy "Play DM.bat" ZipTemp
copy "Play CSB.bat" ZipTemp
copy "Start new Kid Dungeon Game.bat" ZipTemp
copy "Watch DM Replay.bat" ZipTemp
copy "Watch CSB Replay.bat" ZipTemp

del CSBWin_x86.zip

REM Generate the x86 Zip file of the install folder
powershell -command^
 Compress-Archive -Path ZipTemp\* CSBWin_x86.zip

REM Cleanup
rmdir /s /q ZipTemp

echo Zip Files Created
