echo [Start Deploy IO_Engine LIB]
echo Copy To ServerCore From IO_Engine

if not exist ..\ServerCore\lib\Debug (
	mkdir ..\ServerCore\lib\Debug
)
copy .\Export_LIB\Debug\IO_Engine.lib ..\ServerCore\lib\Debug\IO_EngineD.lib
echo [End Deploy Utility LIB]

echo [Start Deploy IO_Engine Header File]
if not exist ..\ServerCore\include (
	mkdir ..\ServerCore\include\IO_Engine
)
xcopy ".\public" "..\ServerCore\include\IO_Engine\" /E /Y
echo [End Deploy IO_Engine Header File]
