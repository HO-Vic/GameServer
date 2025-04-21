setlocal enabledelayedexpansion

:: 중단 시 처리를 위한 배치 작업
pushd ".\bin\x64\Release\"
DreamWorld_Server.exe
popd

