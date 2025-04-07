echo [Start Deploy IO_Engine LIB]

echo [Deploy lib, pdb To SimpleEcho]

:: SimpleEcho 배포 (Debug)

if exist ".\Export_LIB\Debug\IO_Engine.lib" (
    if not exist "..\..\SimpleEcho\lib\Debug" (
        mkdir "..\..\SimpleEcho\lib\Debug"
    )
    copy ".\Export_LIB\Debug\IO_Engine.lib" "..\..\SimpleEcho\lib\Debug\IO_EngineD.lib"
    
    if not exist "..\..\SimpleEcho\bin\x64\Debug" (
        mkdir "..\..\SimpleEcho\bin\x64\Debug"
    )
    copy ".\Export_LIB\Debug\IO_Engine.pdb" "..\..\SimpleEcho\bin\x64\Debug\IO_Engine.pdb"
)

:: DreamWorld_Server 배포 (Debug)

if exist ".\Export_LIB\Debug\IO_Engine.lib" (
    if not exist "..\..\DreamWorld_Server\lib\x64\Debug" (
        mkdir "..\..\DreamWorld_Server\lib\x64\Debug"
    )
    copy ".\Export_LIB\Debug\IO_Engine.lib" "..\..\DreamWorld_Server\lib\x64\Debug\IO_EngineD.lib"
    
    if not exist "..\..\DreamWorld_Server\bin\x64\Debug" (
        mkdir "..\..\DreamWorld_Server\bin\x64\Debug"
    )
    copy ".\Export_LIB\Debug\IO_Engine.pdb" "..\..\DreamWorld_Server\bin\x64\Debug\IO_Engine.pdb"
)

:: DreamWorld_StressBot 배포 (Debug)

if exist ".\Export_LIB\Debug\IO_Engine.lib" (
    if not exist "..\..\DreamWorld_StressBot\lib\x64\Debug" (
        mkdir "..\..\DreamWorld_StressBot\lib\x64\Debug"
    )
    copy ".\Export_LIB\Debug\IO_Engine.lib" "..\..\DreamWorld_StressBot\lib\x64\Debug\IO_EngineD.lib"
    
    if not exist "..\..\DreamWorld_StressBot\bin\x64\Debug" (
        mkdir "..\..\DreamWorld_StressBot\bin\x64\Debug"
    )
    copy ".\Export_LIB\Debug\IO_Engine.pdb" "..\..\DreamWorld_StressBot\bin\x64\Debug\IO_Engine.pdb"
)

:: Release 배포

if exist ".\Export_LIB\Release\IO_Engine.lib" (
    if not exist "..\..\SimpleEcho\lib\Release" (
        mkdir "..\..\SimpleEcho\lib\Release"
    )
    copy ".\Export_LIB\Release\IO_Engine.lib" "..\..\SimpleEcho\lib\Release\IO_Engine.lib"
    
    if not exist "..\..\SimpleEcho\bin\x64\Release" (
        mkdir "..\..\SimpleEcho\bin\x64\Release"
    )
    copy ".\Export_LIB\Release\IO_Engine.pdb" "..\..\SimpleEcho\bin\x64\Release\IO_Engine.pdb"
)

if exist ".\Export_LIB\Release\IO_Engine.lib" (
    if not exist "..\..\DreamWorld_Server\lib\x64\Release" (
        mkdir "..\..\DreamWorld_Server\lib\x64\Release"
    )
    copy ".\Export_LIB\Release\IO_Engine.lib" "..\..\DreamWorld_Server\lib\x64\Release\IO_Engine.lib"
    
    if not exist "..\..\DreamWorld_Server\bin\x64\Release" (
        mkdir "..\..\DreamWorld_Server\bin\x64\Release"
    )
    copy ".\Export_LIB\Release\IO_Engine.pdb" "..\..\DreamWorld_Server\bin\x64\Release\IO_Engine.pdb"
)

if exist ".\Export_LIB\Release\IO_Engine.lib" (
    if not exist "..\..\DreamWorld_StressBot\lib\x64\Release" (
        mkdir "..\..\DreamWorld_StressBot\lib\x64\Release"
    )
    copy ".\Export_LIB\Release\IO_Engine.lib" "..\..\DreamWorld_StressBot\lib\x64\Release\IO_Engine.lib"
    
    if not exist "..\..\DreamWorld_StressBot\bin\x64\Release" (
        mkdir "..\..\DreamWorld_StressBot\bin\x64\Release"
    )
    copy ".\Export_LIB\Release\IO_Engine.pdb" "..\..\DreamWorld_StressBot\bin\x64\Release\IO_Engine.pdb"
)

echo [End Deploy IO_Engine LIB]

:: Header 파일 배포

echo [Start Deploy IO_Engine Header File]
xcopy ".\public" "..\..\SimpleEcho\include\IO_Engine\" /E /Y
xcopy ".\public" "..\..\DreamWorld_Server\include\IO_Engine\" /E /Y
xcopy ".\public" "..\..\DreamWorld_StressBot\include\IO_Engine\" /E /Y
echo [End Deploy IO_Engine Header File]

:: Export 폴더로 배포된 데이터 복사

echo [Deployed Data To Export]

if not exist .\Export\include\IO_Engine (
    mkdir .\Export\include\IO_Engine
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
if exist .\Export_LIB\Release\IO_Engine.lib (
    copy .\Export_LIB\Release\IO_Engine.lib .\Export\lib\Release\IO_Engine.lib
    copy .\Export_LIB\Release\IO_Engine.pdb .\Export\Release\IO_Engine.pdb
)

echo [Finish IO_Engine]
