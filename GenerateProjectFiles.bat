setlocal

@REM 언리얼 엔진 경로
SET UnrealEngine=C:\UnrealEngine

@REM 프로젝트 이름
SET Project=SProject

SET UE="%UnrealEngine%/Engine/Build/BatchFiles/Build.bat"
SET PROJECT="%~dp0%Project%.uproject"

%UE% -projectfiles -project=%PROJECT% -game -engine -progress
