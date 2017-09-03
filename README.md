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

### Definitions
 - base type: The type a factory instantiates.  factory<foo> would have a base type of foo.
 - output type: The return type of a create function.  By default the factory<foo> output type would be std::unique_ptr<foo>, but other types may be requested.
 - initializer: A function capable of instantiaing the base type of a factory.
 - adapter: A layer on top of factory<foo>::create to allow types such as templated containers to be created without having to register initializers for each element type.
 - type: A string identifying an initializer.
 - config: YAML to be passed to an initializer.
 - factory config: YAML containing both type information and config to be passed to an initializer. 