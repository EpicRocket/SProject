
setlocal

SET GCONFIG=%~dp0EpicRocketConfig.yaml

IF NOT EXIST "%GCONFIG%" (
	ECHO EpicRocketConfig.yaml not found. Please run EpicRocket.bat to generate it.
	GOTO :EOF
)

"ncdownloader.exe" "%GCONFIG%"
