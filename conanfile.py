from conans import ConanFile, CMake, tools


class YadiConan(ConanFile):
    name = "yadi"
    version = "0.1"
    license = "<Put the package license here>"
    url = "<Package recipe repository url here, for issues about the package>"
    description = "<Description of Yadi here>"
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake"
    requires = "yaml-cpp/master@ebclark2/stable"
    build_requires = "cmake_installer/1.0@conan/stable"
    options = {"shared": [True, False]}
    default_options = "shared=True", "yaml-cpp:fPIC=True", "cmake_installer:version=3.9.0"
    no_copy_source=True
    exports_sources = "*"

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        self.copy("*.hpp", dst="include", src="%s/src" % self.source_folder)
        self.copy("*.hpp", dst="include", src="%s/inspector" % self.source_folder)
        self.copy("*.so", dst="lib", keep_path=False)
        self.copy("*.dylib", dst="lib", keep_path=False)
        self.copy("*.a", dst="lib", keep_path=False)
        self.copy("yadi_inspector", dst="bin", keep_path=False)

    def package_info(self):
        self.cpp_info.libs = ["yadi"]
