echo [Start Deploy Utility LIB]
echo Copy To IO_Engine From Utility
::  if not exist ..\ServerCore\lib\Debug (
::  	mkdir ..\ServerCore\lib\Debug
::  )
:: copy .\Export_LIB\Debug\Utility.lib ..\ServerCore\lib\Debug\UtilityD.lib

:: IO_Engine lib 배포
if not exist ..\IO_Engine\lib\Debug (
	mkdir ..\IO_Engine\lib\Debug
)
copy .\Export_LIB\Debug\Utility.lib ..\IO_Engine\lib\Debug\UtilityD.lib

if not exist ..\IO_Engine\lib\Release (
	mkdir ..\IO_Engine\lib\Release
)

copy .\Export_LIB\Release\Utility.lib ..\IO_Engine\lib\Release\Utility.lib

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

copy .\Export_LIB\Debug\Utility.lib ..\..\SimpleTCPEcho\Server\lib\Debug\UtilityD.lib
copy .\Export_LIB\Debug\Utility.pdb ..\..\SimpleTCPEcho\Server\bin\x64\Debug\Utility.pdb
copy .\Export_LIB\Release\Utility.lib ..\..\SimpleTCPEcho\Server\lib\Release\Utility.lib
copy .\Export_LIB\Release\Utility.pdb ..\..\SimpleTCPEcho\Server\bin\x64\Release\Utility.pdb

copy .\Export_LIB\Debug\Utility.lib ..\..\SimpleTCPEcho\Client\lib\Debug\UtilityD.lib
copy .\Export_LIB\Debug\Utility.pdb ..\..\SimpleTCPEcho\Client\bin\x64\Debug\Utility.pdb
copy .\Export_LIB\Release\Utility.lib ..\..\SimpleTCPEcho\Client\lib\Release\Utility.lib
copy .\Export_LIB\Release\Utility.pdb ..\..\SimpleTCPEcho\Client\bin\x64\Release\Utility.pdb

copy .\Export_LIB\Debug\Utility.lib ..\..\DreamWorld_Server\lib\x64\Debug\UtilityD.lib
copy .\Export_LIB\Debug\Utility.pdb ..\..\DreamWorld_Server\bin\x64\Debug\Utility.pdb
copy .\Export_LIB\Release\Utility.lib ..\..\DreamWorld_Server\lib\x64\Release\Utility.lib
copy .\Export_LIB\Release\Utility.pdb ..\..\DreamWorld_Server\bin\x64\Release\Utility.pdb

copy .\Export_LIB\Debug\Utility.lib ..\..\DreamWorld_StressBot\lib\x64\Debug\UtilityD.lib
copy .\Export_LIB\Debug\Utility.pdb ..\..\DreamWorld_StressBot\bin\x64\Debug\Utility.pdb
copy .\Export_LIB\Release\Utility.lib ..\..\DreamWorld_StressBot\lib\x64\Release\Utility.lib
copy .\Export_LIB\Release\Utility.pdb ..\..\DreamWorld_StressBot\bin\x64\Release\Utility.pdb

copy .\Export_LIB\Debug\Utility.lib ..\..\SimpleUDPEcho\Server\lib\x64\Debug\UtilityD.lib
copy .\Export_LIB\Debug\Utility.pdb ..\..\SimpleUDPEcho\Server\bin\x64\Debug\Utility.pdb
copy .\Export_LIB\Release\Utility.lib ..\..\SimpleUDPEcho\Server\lib\x64\Release\Utility.lib
copy .\Export_LIB\Release\Utility.pdb ..\..\SimpleUDPEcho\Server\bin\x64\Release\Utility.pdb

copy .\Export_LIB\Debug\Utility.lib ..\..\SimpleUDPEcho\Client\lib\x64\Debug\UtilityD.lib
copy .\Export_LIB\Debug\Utility.pdb ..\..\SimpleUDPEcho\Client\bin\x64\Debug\Utility.pdb
copy .\Export_LIB\Release\Utility.lib ..\..\SimpleUDPEcho\Client\lib\x64\Release\Utility.lib
copy .\Export_LIB\Release\Utility.pdb ..\..\SimpleUDPEcho\Client\bin\x64\Release\Utility.pdb

echo [End Deploy Utility LIB]

echo [Start Deploy Utility Header File]

::Io_Engine에 헤더 배포
rmdir /s /q "..\IO_Engine\include\Utility"
mkdir ..\IO_Engine\include\Utility
xcopy ".\public" "..\IO_Engine\include\Utility\" /E /Y

::  xcopy ".\include" "..\IO_Engine\include\" /E /Y


::DreamWorld Server 헤더 배포
rmdir /s /q "..\..\DreamWorld_Server\include\Utility"
mkdir ..\..\DreamWorld_Server\include\Utility
xcopy ".\public" "..\..\DreamWorld_Server\include\Utility\" /E /Y

::DreamWorld Stress 헤더 배포
rmdir /s /q "..\..\DreamWorld_StressBot\include\Utility"
mkdir ..\..\DreamWorld_StressBot\include\Utility
xcopy ".\public" "..\..\DreamWorld_StressBot\include\Utility\" /E /Y

::SimpleUDPEcho 헤더 배포
rmdir /s /q "..\..\SimpleUDPEcho\Server\include\Utility"
mkdir ..\..\SimpleUDPEcho\Server\include\Utility
xcopy ".\public" "..\..\SimpleUDPEcho\Server\include\Utility\" /E /Y

rmdir /s /q "..\..\SimpleUDPEcho\Client\include\Utility"
mkdir ..\..\SimpleUDPEcho\Client\include\Utility
xcopy ".\public" "..\..\SimpleUDPEcho\Client\include\Utility\" /E /Y

::SimpleTCPEcho에 헤더 배포
rmdir /s /q "..\..\SimpleTCPEcho\Server\include\Utility"
mkdir ..\..\SimpleTCPEcho\Server\include\Utility
xcopy ".\public" "..\..\SimpleTCPEcho\Server\include\Utility\" /E /Y

rmdir /s /q "..\..\SimpleTCPEcho\Client\include\Utility"
mkdir ..\..\SimpleTCPEcho\Client\include\Utility
xcopy ".\public" "..\..\SimpleTCPEcho\Client\include\Utility\" /E /Y


echo [End Deploy Utility Header File]

echo [Deployed Data To Export]
:: 배포했던 Export 정보 저장
if not exist .\Export\include\Utility (
	mkdir .\Export\include\Utility
)
if not exist .\Export\lib\Debug (
	mkdir .\Export\lib\Debug
)

if not exist .\Export\lib\Release (
	mkdir .\Export\lib\Release
)

xcopy ".\public" ".\Export\include\Utility" /E /Y
copy .\Export_LIB\Debug\Utility.lib .\Export\lib\Debug\UtilityD.lib
copy .\Export_LIB\Release\Utility.lib .\Export\lib\Release\Utility.lib
copy .\Export_LIB\Debug\Utility.pdb .\Export\lib\Debug\Utility.pdb
copy .\Export_LIB\Release\Utility.pdb .\Export\lib\Release\Utility.pdb
echo [Finish Utility]