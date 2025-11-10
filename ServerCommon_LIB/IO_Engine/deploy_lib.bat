echo [Start Deploy IO_Engine LIB]
echo Copy To IO_Engine From IO_Engine
::  if not exist ..\ServerCore\lib\Debug (
::  	mkdir ..\ServerCore\lib\Debug
::  )
:: copy .\Export_LIB\Debug\IO_Engine.lib ..\ServerCore\lib\Debug\IO_EngineD.lib

:: IO_Engine lib 배포
if not exist ..\IO_Engine\lib\Debug (
	mkdir ..\IO_Engine\lib\Debug
)
copy .\Export_LIB\Debug\IO_Engine.lib ..\IO_Engine\lib\Debug\IO_EngineD.lib

if not exist ..\IO_Engine\lib\Release (
	mkdir ..\IO_Engine\lib\Release
)

copy .\Export_LIB\Release\IO_Engine.lib ..\IO_Engine\lib\Release\IO_Engine.lib

:: SimpleTCPEcho
if not exist ..\..\SimpleTCPEcho\Server\lib\Debug (
	mkdir ..\..\SimpleTCPEcho\Server\lib\Debug
)

if not exist ..\..\SimpleTCPEcho\Server\lib\Release (
	mkdir ..\..\SimpleTCPEcho\Server\lib\Release
)

if not exist ..\..\SimpleTCPEcho\Server\bin\x64\Debug (
	mkdir ..\..\SimpleTCPEcho\Server\bin\x64\Debug
)

if not exist ..\..\SimpleTCPEcho\Server\bin\x64\Release (
	mkdir ..\..\SimpleTCPEcho\Server\bin\x64\Release
)
:: SimpleTCPEcho Client
if not exist ..\..\SimpleTCPEcho\Client\lib\Debug (
	mkdir ..\..\SimpleTCPEcho\Client\lib\Debug
)

if not exist ..\..\SimpleTCPEcho\Client\lib\Release (
	mkdir ..\..\SimpleTCPEcho\Client\lib\Release
)

if not exist ..\..\SimpleTCPEcho\Client\bin\x64\Debug (
	mkdir ..\..\SimpleTCPEcho\Client\bin\x64\Debug
)

if not exist ..\..\SimpleTCPEcho\Client\bin\x64\Release (
	mkdir ..\..\SimpleTCPEcho\Client\bin\x64\Release
)
:: SimpleUDPEcho
if not exist ..\..\SimpleUDPEcho\Server\lib\x64\Debug (
	mkdir ..\..\SimpleUDPEcho\Server\lib\x64\Debug
)

if not exist ..\..\SimpleUDPEcho\Server\lib\x64\Release (
	mkdir ..\..\SimpleUDPEcho\Server\lib\x64\Release
)

if not exist ..\..\SimpleUDPEcho\Server\bin\x64\Debug (
	mkdir ..\..\SimpleUDPEcho\Server\bin\x64\Debug
)

if not exist ..\..\SimpleUDPEcho\Server\bin\x64\Release (
	mkdir ..\..\SimpleUDPEcho\Server\bin\x64\Release
)
::SimpleUDPEcho Client
if not exist ..\..\SimpleUDPEcho\Client\lib\x64\Debug (
	mkdir ..\..\SimpleUDPEcho\Client\lib\x64\Debug
)

if not exist ..\..\SimpleUDPEcho\Client\lib\x64\Release (
	mkdir ..\..\SimpleUDPEcho\Client\lib\x64\Release
)

if not exist ..\..\SimpleUDPEcho\Client\bin\x64\Debug (
	mkdir ..\..\SimpleUDPEcho\Client\bin\x64\Debug
)

if not exist ..\..\SimpleUDPEcho\Client\bin\x64\Release (
	mkdir ..\..\SimpleUDPEcho\Client\bin\x64\Release
)

if not exist ..\..\DreamWorld_Server\lib\x64\Debug (
	mkdir ..\..\DreamWorld_Server\lib\x64\Debug
)

if not exist ..\..\DreamWorld_Server\lib\x64\Release (
	mkdir ..\..\DreamWorld_Server\lib\x64\Release
)

if not exist ..\..\DreamWorld_Server\bin\x64\Debug (
	mkdir ..\..\DreamWorld_Server\bin\x64\Debug
)

if not exist ..\..\DreamWorld_Server\bin\x64\Release (
	mkdir ..\..\DreamWorld_Server\bin\x64\Release
)

if not exist ..\..\DreamWorld_StressBot\lib\x64\Debug (
	mkdir ..\..\DreamWorld_StressBot\lib\x64\Debug
)

if not exist ..\..\DreamWorld_StressBot\lib\x64\Release (
	mkdir ..\..\DreamWorld_StressBot\lib\x64\Release
)

if not exist ..\..\DreamWorld_StressBot\bin\x64\Debug (
	mkdir ..\..\DreamWorld_StressBot\bin\x64\Debug
)

if not exist ..\..\DreamWorld_StressBot\bin\x64\Release (
	mkdir ..\..\DreamWorld_StressBot\bin\x64\Release
)

copy .\Export_LIB\Debug\IO_Engine.lib ..\..\SimpleTCPEcho\Server\lib\Debug\IO_EngineD.lib
copy .\Export_LIB\Debug\IO_Engine.pdb ..\..\SimpleTCPEcho\Server\bin\x64\Debug\IO_Engine.pdb
copy .\Export_LIB\Release\IO_Engine.lib ..\..\SimpleTCPEcho\Server\lib\Release\IO_Engine.lib
copy .\Export_LIB\Release\IO_Engine.pdb ..\..\SimpleTCPEcho\Server\bin\x64\Release\IO_Engine.pdb

copy .\Export_LIB\Debug\IO_Engine.lib ..\..\SimpleTCPEcho\Client\lib\Debug\IO_EngineD.lib
copy .\Export_LIB\Debug\IO_Engine.pdb ..\..\SimpleTCPEcho\Client\bin\x64\Debug\IO_Engine.pdb
copy .\Export_LIB\Release\IO_Engine.lib ..\..\SimpleTCPEcho\Client\lib\Release\IO_Engine.lib
copy .\Export_LIB\Release\IO_Engine.pdb ..\..\SimpleTCPEcho\Client\bin\x64\Release\IO_Engine.pdb

copy .\Export_LIB\Debug\IO_Engine.lib ..\..\DreamWorld_Server\lib\x64\Debug\IO_EngineD.lib
copy .\Export_LIB\Debug\IO_Engine.pdb ..\..\DreamWorld_Server\bin\x64\Debug\IO_Engine.pdb
copy .\Export_LIB\Release\IO_Engine.lib ..\..\DreamWorld_Server\lib\x64\Release\IO_Engine.lib
copy .\Export_LIB\Release\IO_Engine.pdb ..\..\DreamWorld_Server\bin\x64\Release\IO_Engine.pdb

copy .\Export_LIB\Debug\IO_Engine.lib ..\..\DreamWorld_StressBot\lib\x64\Debug\IO_EngineD.lib
copy .\Export_LIB\Debug\IO_Engine.pdb ..\..\DreamWorld_StressBot\bin\x64\Debug\IO_Engine.pdb
copy .\Export_LIB\Release\IO_Engine.lib ..\..\DreamWorld_StressBot\lib\x64\Release\IO_Engine.lib
copy .\Export_LIB\Release\IO_Engine.pdb ..\..\DreamWorld_StressBot\bin\x64\Release\IO_Engine.pdb

copy .\Export_LIB\Debug\IO_Engine.lib ..\..\SimpleUDPEcho\Server\lib\x64\Debug\IO_EngineD.lib
copy .\Export_LIB\Debug\IO_Engine.pdb ..\..\SimpleUDPEcho\Server\bin\x64\Debug\IO_Engine.pdb
copy .\Export_LIB\Release\IO_Engine.lib ..\..\SimpleUDPEcho\Server\lib\x64\Release\IO_Engine.lib
copy .\Export_LIB\Release\IO_Engine.pdb ..\..\SimpleUDPEcho\Server\bin\x64\Release\IO_Engine.pdb

copy .\Export_LIB\Debug\IO_Engine.lib ..\..\SimpleUDPEcho\Client\lib\x64\Debug\IO_EngineD.lib
copy .\Export_LIB\Debug\IO_Engine.pdb ..\..\SimpleUDPEcho\Client\bin\x64\Debug\IO_Engine.pdb
copy .\Export_LIB\Release\IO_Engine.lib ..\..\SimpleUDPEcho\Client\lib\x64\Release\IO_Engine.lib
copy .\Export_LIB\Release\IO_Engine.pdb ..\..\SimpleUDPEcho\Client\bin\x64\Release\IO_Engine.pdb

echo [End Deploy IO_Engine LIB]

echo [Start Deploy IO_Engine Header File]

::DreamWorld Server 헤더 배포
rmdir /s /q "..\..\DreamWorld_Server\include\IO_Engine"
mkdir ..\..\DreamWorld_Server\include\IO_Engine
xcopy ".\public" "..\..\DreamWorld_Server\include\IO_Engine\" /E /Y

::DreamWorld Stress 헤더 배포
rmdir /s /q "..\..\DreamWorld_StressBot\include\IO_Engine"
mkdir ..\..\DreamWorld_StressBot\include\IO_Engine
xcopy ".\public" "..\..\DreamWorld_StressBot\include\IO_Engine\" /E /Y

::SimpleUDPEcho 헤더 배포
rmdir /s /q "..\..\SimpleUDPEcho\Server\include\IO_Engine"
mkdir ..\..\SimpleUDPEcho\Server\include\IO_Engine
xcopy ".\public" "..\..\SimpleUDPEcho\Server\include\IO_Engine\" /E /Y

rmdir /s /q "..\..\SimpleUDPEcho\Client\include\IO_Engine"
mkdir ..\..\SimpleUDPEcho\Client\include\IO_Engine
xcopy ".\public" "..\..\SimpleUDPEcho\Client\include\IO_Engine\" /E /Y

::SimpleTCPEcho에 헤더 배포
rmdir /s /q "..\..\SimpleTCPEcho\Server\include\IO_Engine"
mkdir ..\..\SimpleTCPEcho\Server\include\IO_Engine
xcopy ".\public" "..\..\SimpleTCPEcho\Server\include\IO_Engine\" /E /Y

rmdir /s /q "..\..\SimpleTCPEcho\Client\include\IO_Engine"
mkdir ..\..\SimpleTCPEcho\Client\include\IO_Engine
xcopy ".\public" "..\..\SimpleTCPEcho\Client\include\IO_Engine\" /E /Y


echo [End Deploy IO_Engine Header File]

echo [Deployed Data To Export]
:: 배포했던 Export 정보 저장
if not exist .\Export\include\IO_Engine (
	mkdir .\Export\include\IO_Engine
)
if not exist .\Export\lib\Debug (
	mkdir .\Export\Debug
)

if not exist .\Export\lib\Release (
	mkdir .\Export\Release
)

xcopy ".\public" ".\Export\include\IO_Engine" /E /Y
copy .\Export_LIB\Debug\IO_Engine.lib .\Export\lib\Debug\IO_EngineD.lib
copy .\Export_LIB\Release\IO_Engine.lib .\Export\lib\Release\IO_Engine.lib
copy .\Export_LIB\Debug\IO_Engine.pdb .\Export\lib\Debug\IO_Engine.pdb
copy .\Export_LIB\Release\IO_Engine.pdb .\Export\lib\Release\IO_Engine.pdb
echo [Finish IO_Engine]