@echo off
python setup.py build_ext --inplace --compiler=mingw32 --force
cd ..
move %CD%\Lab1\my_string.cp313-win_amd64.pyd %CD%\Lab3\my_string.cp313-win_amd64.pyd

