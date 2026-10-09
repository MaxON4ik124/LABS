@REM  winget install LLVM.LLVM
@REM  tar -czf lab1.tar.gz TarBuild/
@REM  docker run -v "./lab1.tar.gz:/input/repo.tgz:ro" mystringtest
g++ -std=c++17 -Wall -Wextra -pedantic MyString.cpp tests.cpp -o tests.exe
.\tests.exe