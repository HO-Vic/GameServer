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

:: SimpleEcho lib, pdb 배포

if not exist ..\..\SimpleEcho\lib\Debug (
	mkdir ..\..\SimpleEcho\lib\Debug
)

if not exist ..\..\SimpleEcho\lib\Release (
	mkdir ..\..\SimpleEcho\lib\Release
)

if not exist ..\..\SimpleEcho\bin\x64\Debug (
	mkdir ..\..\SimpleEcho\bin\x64\Debug
)

if not exist ..\..\SimpleEcho\bin\x64\Release (
	mkdir ..\..\SimpleEcho\bin\x64\Release
)

if not exist ..\..\DreamWorld_Server\lib\Debug (
	mkdir ..\..\DreamWorld_Server\lib\Debug
)

if not exist ..\..\DreamWorld_Server\lib\Release (
	mkdir ..\..\DreamWorld_Server\lib\Release
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

if not exist ..\..\SimpleUDPEcho\lib\x64\Debug (
	mkdir ..\..\SimpleUDPEcho\lib\x64\Debug
)

if not exist ..\..\SimpleUDPEcho\lib\x64\Release (
	mkdir ..\..\SimpleUDPEcho\lib\x64\Release
)

if not exist ..\..\SimpleUDPEcho\bin\x64\Debug (
	mkdir ..\..\SimpleUDPEcho\bin\x64\Debug
)

if not exist ..\..\SimpleUDPEcho\bin\x64\Release (
	mkdir ..\..\SimpleUDPEcho\bin\x64\Release
)

copy .\Export_LIB\Debug\Utility.lib ..\..\SimpleEcho\lib\Debug\UtilityD.lib
copy .\Export_LIB\Debug\Utility.pdb ..\..\SimpleEcho\bin\x64\Debug\Utility.pdb
copy .\Export_LIB\Release\Utility.lib ..\..\SimpleEcho\lib\Release\Utility.lib
copy .\Export_LIB\Release\Utility.pdb ..\..\SimpleEcho\bin\x64\Release\Utility.pdb

copy .\Export_LIB\Debug\Utility.lib ..\..\DreamWorld_Server\lib\x64\Debug\UtilityD.lib
copy .\Export_LIB\Debug\Utility.pdb ..\..\DreamWorld_Server\bin\x64\Debug\Utility.pdb
copy .\Export_LIB\Release\Utility.lib ..\..\DreamWorld_Server\lib\x64\Release\Utility.lib
copy .\Export_LIB\Release\Utility.pdb ..\..\DreamWorld_Server\bin\x64\Release\Utility.pdb

copy .\Export_LIB\Debug\Utility.lib ..\..\DreamWorld_StressBot\lib\x64\Debug\UtilityD.lib
copy .\Export_LIB\Debug\Utility.pdb ..\..\DreamWorld_StressBot\bin\x64\Debug\Utility.pdb
copy .\Export_LIB\Release\Utility.lib ..\..\DreamWorld_StressBot\lib\x64\Release\Utility.lib
copy .\Export_LIB\Release\Utility.pdb ..\..\DreamWorld_StressBot\bin\x64\Release\Utility.pdb

copy .\Export_LIB\Debug\Utility.lib ..\..\SimpleUDPEcho\lib\x64\Debug\UtilityD.lib
copy .\Export_LIB\Debug\Utility.pdb ..\..\SimpleUDPEcho\bin\x64\Debug\Utility.pdb
copy .\Export_LIB\Release\Utility.lib ..\..\SimpleUDPEcho\lib\x64\Release\Utility.lib
copy .\Export_LIB\Release\Utility.pdb ..\..\SimpleUDPEcho\bin\x64\Release\Utility.pdb

echo [End Deploy Utility LIB]

echo [Start Deploy Utility Header File]
:: if not exist ..\ServerCore\include (
:: 	mkdir ..\ServerCore\include\Utility
:: )
:: xcopy ".\public" "..\ServerCore\include\Utility\" /E /Y
:: xcopy ".\include" "..\ServerCore\include\" /E /Y

::Io_Engine에 헤더 배포
rmdir /s /q "..\IO_Engine\include\Utility"
mkdir ..\IO_Engine\include\Utility
xcopy ".\public" "..\IO_Engine\include\Utility\" /E /Y

::  xcopy ".\include" "..\IO_Engine\include\" /E /Y

::SimpleEcho에 헤더 배포
rmdir /s /q "..\..\SimpleEcho\include\Utility"
mkdir ..\..\SimpleEcho\include\Utility
xcopy ".\public" "..\..\SimpleEcho\include\Utility\" /E /Y

::DreamWorld Server 헤더 배포
rmdir /s /q "..\..\DreamWorld_Server\include\Utility"
mkdir ..\..\DreamWorld_Server\include\Utility
xcopy ".\public" "..\..\DreamWorld_Server\include\Utility\" /E /Y

::DreamWorld Stress 헤더 배포
rmdir /s /q "..\..\DreamWorld_StressBot\include\Utility"
mkdir ..\..\DreamWorld_StressBot\include\Utility
xcopy ".\public" "..\..\DreamWorld_StressBot\include\Utility\" /E /Y
echo [End Deploy Utility Header File]

::SimpleUDPEcho 헤더 배포
rmdir /s /q "..\..\SimpleUDPEcho\include\Utility"
mkdir ..\..\SimpleUDPEcho\include\Utility
xcopy ".\public" "..\..\SimpleUDPEcho\include\Utility\" /E /Y
echo [End Deploy Utility Header File]

echo [Deployed Data To Export]
:: 배포했던 Export 정보 저장
if not exist .\Export\include\Utility (
	mkdir .\Export\include\Utility
)
if not exist .\Export\lib\Debug (
	mkdir .\Export\Debug
)

if not exist .\Export\lib\Release (
	mkdir .\Export\Release
)

xcopy ".\public" ".\Export\include\Utility" /E /Y
copy .\Export_LIB\Debug\Utility.lib .\Export\lib\Debug\UtilityD.lib
copy .\Export_LIB\Release\Utility.lib .\Export\lib\Release\Utility.lib
copy .\Export_LIB\Debug\Utility.pdb .\Export\lib\Debug\Utility.pdb
copy .\Export_LIB\Release\Utility.pdb .\Export\lib\Release\Utility.pdb
echo [Finish Utility]