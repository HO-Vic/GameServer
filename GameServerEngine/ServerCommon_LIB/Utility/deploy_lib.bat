echo [Start Deploy Utility LIB]
echo Copy To IO_Engine From Utility

if not exist ..\IO_Engine\lib\Debug (
	mkdir ..\IO_Engine\lib\Debug
)
copy .\Export_LIB\Debug\Utility.lib ..\IO_Engine\lib\Debug\UtilityD.lib

echo [End Deploy Utility LIB]