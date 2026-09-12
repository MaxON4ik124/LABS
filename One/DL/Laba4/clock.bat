@echo off
for /f "tokens=1-5 delims=., " %%d in ("%date% %time%") do (echo %%d.%%e.%%f %%g)
pause
