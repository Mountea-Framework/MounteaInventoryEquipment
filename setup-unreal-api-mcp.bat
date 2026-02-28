@echo off
setlocal EnableExtensions EnableDelayedExpansion

set "SCRIPT_DIR=%~dp0"
if "%SCRIPT_DIR:~-1%"=="\" set "SCRIPT_DIR=%SCRIPT_DIR:~0,-1%"

echo [1/6] Detecting Unreal project path...
for /f "delims=" %%I in ('powershell -NoProfile -ExecutionPolicy Bypass -Command "$p=Get-Item -LiteralPath '%SCRIPT_DIR%'; while($true){$u=Get-ChildItem -LiteralPath $p.FullName -Filter *.uproject -File -ErrorAction SilentlyContinue | Select-Object -First 1; if($u){$p.FullName; break}; if(-not $p.Parent){break}; $p=$p.Parent}"') do (
    set "PROJECT_PATH=%%I"
)

if not defined PROJECT_PATH (
    echo [WARN] No .uproject found above this folder. Falling back to script folder.
    set "PROJECT_PATH=%SCRIPT_DIR%"
)
echo       Project path: !PROJECT_PATH!

echo [2/6] Checking Python...
where python >nul 2>&1
if errorlevel 1 (
    set "ERR_MSG=Python is not available on PATH."
    goto :fail
)

for /f "delims=" %%I in ('python -c "import sys; print(f'{sys.version_info[0]}.{sys.version_info[1]}.{sys.version_info[2]}')"') do set "PY_VERSION=%%I"
echo       Python: !PY_VERSION!

echo [3/6] Installing/updating unreal-api-mcp (user scope)...
python -m pip install --user --upgrade unreal-api-mcp
if errorlevel 1 (
    set "ERR_MSG=Failed to install unreal-api-mcp."
    goto :fail
)

echo [4/6] Resolving server executable path...
for /f "delims=" %%I in ('python -c "import os,site,shutil,sys,sysconfig,pathlib; cands=[]; cands.append(pathlib.Path(site.USER_BASE) / f'Python{sys.version_info[0]}{sys.version_info[1]}' / 'Scripts' / 'unreal-api-mcp.exe'); cands.append(pathlib.Path(sysconfig.get_path('scripts')) / 'unreal-api-mcp.exe'); w=shutil.which('unreal-api-mcp'); cands.extend([pathlib.Path(w)] if w else []); hit=''; [None for p in cands if not hit and p and p.exists() and not (hit:=str(p))]; print(hit)"') do set "SERVER_EXE=%%I"
if not defined SERVER_EXE (
    set "ERR_MSG=Could not resolve unreal-api-mcp executable."
    goto :fail
)
if not exist "!SERVER_EXE!" (
    set "ERR_MSG=Resolved executable does not exist: !SERVER_EXE!"
    goto :fail
)
echo       Server exe: !SERVER_EXE!

echo [5/6] Pre-downloading Unreal API database...
set "UNREAL_PROJECT_PATH=!PROJECT_PATH!"
python -c "from unreal_api_mcp import version; print('UE', version.detect_version()); print('DB', version.ensure_db())"
if errorlevel 1 (
    set "ERR_MSG=Failed to initialize/download Unreal API database."
    goto :fail
)

echo [6/6] Writing MCP config files...
set "SERVER_EXE_JSON=!SERVER_EXE:\=/!"
set "PROJECT_PATH_JSON=!PROJECT_PATH:\=/!"
set "MCP_DOT_FILE=%SCRIPT_DIR%\.mcp.json"
set "MCP_FILE=%SCRIPT_DIR%\mcp.json"

if exist "!MCP_DOT_FILE!" copy /Y "!MCP_DOT_FILE!" "!MCP_DOT_FILE!.bak" >nul
if exist "!MCP_FILE!" copy /Y "!MCP_FILE!" "!MCP_FILE!.bak" >nul

(
echo {
echo   "mcpServers": {
echo     "unreal-api": {
echo       "command": "!SERVER_EXE_JSON!",
echo       "args": [],
echo       "env": {
echo         "UNREAL_PROJECT_PATH": "!PROJECT_PATH_JSON!"
echo       }
echo     }
echo   }
echo }
) > "!MCP_DOT_FILE!"
if errorlevel 1 (
    set "ERR_MSG=Failed to write !MCP_DOT_FILE!."
    goto :fail
)

copy /Y "!MCP_DOT_FILE!" "!MCP_FILE!" >nul
if errorlevel 1 (
    set "ERR_MSG=Failed to write !MCP_FILE!."
    goto :fail
)

echo.
echo Setup complete.
echo - Created: !MCP_DOT_FILE!
echo - Created: !MCP_FILE!
echo - Project : !PROJECT_PATH!
echo - Server  : !SERVER_EXE!
echo.
echo Next step: restart your AI client/editor so MCP config is reloaded.
exit /b 0

:fail
echo [ERROR] !ERR_MSG!
exit /b 1
