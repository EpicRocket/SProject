setlocal

SET SP_CONFIG=%~dp0SProjectConfig.yaml

IF NOT EXIST "%UE_CONFIG%" (
    ECHO UnrealEngineConfig.yaml not found. Please run EpicRocket.bat to generate it.
    GOTO :EOF
)

"ncdownloader.exe" "%SP_CONFIG%"
