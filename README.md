# YADI
Yet another dependency injection library!  Or, maybe yadi ain't dependency inject, just some config tool?

### Build requirements
 - c++14
 - conan.io, see http://conan.io
 - edsidea conan remote, `conan remote add <REMOTE> https://api.bintray.com/conan/edsidea/edsidea` where <REMOTE> is your name for this remote
 - cmake 3.2

### Building from repository
 - mkdir build
 - cd build
 - conan install .. (`conan install .. --build` perhaps)
 - cmake ..
 - make
 - ctest
