echo [Start Deploy IO_Engine LIB]

REM echo Copy To ServerCore From IO_Engine

REM if not exist ..\ServerCore\lib\Debug (
REM 	mkdir ..\ServerCore\lib\Debug
REM )
:: copy .\Export_LIB\Debug\IO_Engine.lib ..\ServerCore\lib\Debug\IO_EngineD.lib

::lib, pdb 배포
echo [Deploy lib, pdb To SimpleEcho]
::SImpleEcho 배포

:: debug
copy .\Export_LIB\Debug\IO_Engine.lib ..\..\SimpleEcho\lib\Debug\IO_EngineD.lib
copy .\Export_LIB\Debug\IO_Engine.pdb ..\..\SimpleEcho\bin\x64\Debug\IO_Engine.pdb
copy .\Export_LIB\Debug\IO_Engine.lib ..\..\DreamWorld_Server\lib\x64\Debug\IO_EngineD.lib
copy .\Export_LIB\Debug\IO_Engine.pdb ..\..\DreamWorld_Server\bin\x64\Debug\IO_Engine.pdb

:: Release
if exist .\Export_LIB\Release\IO_Engine.lib (
	copy .\Export_LIB\Release\IO_Engine.lib ..\..\SimpleEcho\lib\Release\IO_Engine.lib
	copy .\Export_LIB\Release\IO_Engine.pdb ..\..\SimpleEcho\bin\x64\Release\IO_Engine.pdb
	copy .\Export_LIB\Release\IO_Engine.lib ..\..\DreamWorld_Server\lib\x64\Release\IO_Engine.lib
	copy .\Export_LIB\Release\IO_Engine.pdb ..\..\DreamWorld_Server\bin\x64\Release\IO_Engine.pdb
)
echo [End Deploy IO_Engine LIB]

:: SimpleEcho header 배포
echo [Start Deploy IO_Engine Header File]
:: if not exist ..\ServerCore\include (
::  	mkdir ..\ServerCore\include\IO_Engine
:: )

:: xcopy ".\public" "..\ServerCore\include\IO_Engine\" /E /Y

xcopy ".\public" "..\..\SimpleEcho\include\IO_Engine\" /E /Y
xcopy ".\public" "..\..\DreamWorld_Server\include\IO_Engine\" /E /Y
echo [End Deploy IO_Engine Header File]


echo [Deployed Data To Export]
::  배포했던 정보들 Export 폴더로

if not exist .\Export\include\IO_Engine (
	mkdir .\Export\IO_Engine
)
if not exist .\Export\lib\Debug (
	mkdir .\Export\lib\Debug
)
if not exist .\Export\lib\Release (
	mkdir .\Export\lib\Release
)

if not exist .\Export\Debug (
	mkdir .\Export\Debug
)
if not exist .\Export\Release (
	mkdir .\Export\Release
)

xcopy ".\public" ".\Export\include\IO_Engine\" /E /Y
copy .\Export_LIB\Debug\IO_Engine.lib .\Export\lib\Debug\IO_EngineD.lib
copy .\Export_LIB\Debug\IO_Engine.pdb .\Export\Debug\IO_EngineD.pdb
if exist .\Export_LIB\Release (
	copy .\Export_LIB\Release\IO_Engine.lib .\Export\lib\Release\IO_Engine.lib
	copy .\Export_LIB\Release\IO_Engine.pdb .\Export\Release\IO_Engine.pdb
)

echo [Finish IO_Engine]