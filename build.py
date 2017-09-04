from conan.packager import ConanMultiPackager


if __name__ == "__main__":
    builder = ConanMultiPackager(args="--build")
    builder.add_common_builds()
    builder.run()
