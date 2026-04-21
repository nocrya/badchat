from conan import ConanFile
from conan.tools.cmake import CMakeDeps, CMakeToolchain


class BadchatConan(ConanFile):
    name = "badchat"
    version = "1.0.0"
    settings = "os", "compiler", "build_type", "arch"

    requires = (
        "boost/1.81.0",
        "grpc/1.50.0",
        "protobuf/3.21.12",
        "hiredis/1.1.0",
        "jsoncpp/1.9.5",
        "openssl/3.0.9",
    )

    default_options = {
        "boost/*:shared": False,
        "boost/*:without_python": True,
        "grpc/*:shared": False,
    }

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.generate()
