winget install LLVM.LLVM
tar -czf lab1.tar.gz TarBuild/
docker run -v "./lab1.tar.gz:/input/repo.tgz:ro" mystringtest