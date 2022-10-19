REM This little script will build the solution four times, in each Configuation+Platform. The build logs are in log files.

REM Btw for those who don't know, lines starting with "REM" are comments. Yeah, I was confused too. You're welcome.

REM CHANGE THIS :: Set this path to be the path to your MSBuild.exe file. It should look something like the path below.
SET msbuild="C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe"

REM Setting the solution to be built from command line args. You could alternatively switch it out to just be your .sln file and not need to keep passing in arguments.
SET slnfile="gadkari_ameya.sln"

(%msbuild% %slnfile% /p:Configuration=Debug /p:Platform=x64 /flp:logfile=BuildLog_Debug_64.log && ECHO "Successfully built Debug x64" > BuildResults.log) || ECHO "Failed to build Debug x64" > BuildResults.log


(%msbuild% %slnfile% /p:Configuration=Release /p:Platform=x64 /flp:logfile=BuildLog_Release_64.log && ECHO "Successfully built Release x64" >> BuildResults.log) || ECHO "Failed to build Release x64" >> BuildResults.log


(%msbuild% %slnfile% /p:Configuration=Debug /p:Platform=x86 /flp:logfile=BuildLog_Debug_86.log && ECHO "Successfully built Debug x86" >> BuildResults.log) || ECHO "Failed to build Debug x86" >> BuildResults.log


(%msbuild% %slnfile% /p:Configuration=Release /p:Platform=x86 /flp:logfile=BuildLog_Release_86.log && ECHO "Successfully built Release x86" >> BuildResults.log) || ECHO "Failed to build Release x86" >> BuildResults.log
