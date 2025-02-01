echo [Start Deploy Utility LIB]
echo Copy To IO_Engine From Utility

if not exist ..\IO_Engine\lib\Debug (
	mkdir ..\IO_Engine\lib\Debug
)
copy .\Export_LIB\Debug\Utility.lib ..\IO_Engine\lib\Debug\UtilityD.lib
echo [End Deploy Utility LIB]

echo [Start Deploy Utility Header File]
if not exist ..\IO_Engine\include (
	mkdir ..\IO_Engine\include\Utility
)
xcopy ".\public" "..\IO_Engine\include\Utility\" /E /Y
xcopy ".\include" "..\IO_Engine\include\" /E /Y
echo [End Deploy Utility Header File]

echo [Start Deploy Utility LIB]
echo Copy To ServerCore From Utility

if not exist ..\ServerCore\lib\Debug (
	mkdir ..\ServerCore\lib\Debug
)
copy .\Export_LIB\Debug\Utility.lib ..\ServerCore\lib\Debug\UtilityD.lib
echo [End Deploy Utility LIB]

echo [Start Deploy Utility Header File]
if not exist ..\ServerCore\include (
	mkdir ..\ServerCore\include\Utility
)
xcopy ".\public" "..\ServerCore\include\Utility\" /E /Y
xcopy ".\include" "..\ServerCore\include\" /E /Y
echo [End Deploy Utility Header File]

if not exist .\Export\Utility (
	mkdir .\Export\Utility
)
if not exist .\Export\Debug (
	mkdir .\Export\Debug
)
xcopy ".\public" ".\Export\Utility" /E /Y
copy .\Export_LIB\Debug\Utility.lib .\Export\Debug\UtilityD.lib