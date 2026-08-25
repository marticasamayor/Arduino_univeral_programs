@echo off
cd /d "%~dp0"  REM Canvia al directori del script

REM Activar l'entorn virtual
call venv\Scripts\activate.bat

REM crea l'executable del python
pyinstaller --onefile --noconsole -i "icon48.ico" --name "Arduino Serial Plotter" .\ComToGraphic_app.py