# Copyright (c) 2016-2020 Knuth Project developers.
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

import os
from conans import CMake
from kthbuild import option_on_off, march_conan_manip, pass_march_to_compiler
from kthbuild import KnuthConanFile

class KnuthRPCConan(KnuthConanFile):
    def recipe_dir(self):
        return os.path.dirname(os.path.abspath(__file__))

    name = "rpc"
    license = "http://www.boost.org/users/license.html"
    url = "https://github.com/k-nuth/rpc"
    description = "JSON-RPC library"
    settings = "os", "compiler", "build_type", "arch"

    options = {"shared": [True, False],
               "fPIC": [True, False],
               "tests": [True, False],
            #    "console": [True, False],
               "currency": ['BCH', 'BTC', 'LTC'],

               "microarchitecture": "ANY",
               "fix_march": [True, False],
               "march_id": "ANY",

               "verbose": [True, False],
               "keoken": [True, False],
               "mempool": [True, False],
               "db": ['legacy', 'legacy_full', 'pruned', 'default', 'full'],
               "db_readonly": [True, False],

               "cxxflags": "ANY",
               "cflags": "ANY",
               "glibcxx_supports_cxx11_abi": "ANY",
               "cmake_export_compile_commands": [True, False],
               "binlog": [True, False],
    }

    default_options = {
        "shared": False,
        "fPIC": True,
        "tests": False, 
        # "console": False,
        "currency": "BCH",
        
        "microarchitecture": "_DUMMY_", 
        "fix_march": False,
        "march_id": "_DUMMY_",

        "verbose": False,
        "keoken": False,
        "mempool": False,
        "db": "default",
        "db_readonly": False,
        
        "cxxflags": "_DUMMY_",
        "cflags": "_DUMMY_",
        "glibcxx_supports_cxx11_abi": "_DUMMY_",
        "cmake_export_compile_commands": False,
        "binlog": False,
    }

    generators = "cmake"
    exports = "conan_*", "ci_utils/*"
    exports_sources = "src/*", "CMakeLists.txt", "cmake/*", "kth-rpcConfig.cmake.in", "knuthbuildinfo.cmake", "include/*", "test/*"
    package_files = "build/lkth-rpc.a"
    build_policy = "missing"

    @property
    def is_keoken(self):
        return self.options.currency == "BCH" and self.options.get_safe("keoken")

    def requirements(self):
        self.requires("boost/1.72.0@kth/stable")
        self.requires("libzmq/4.3.2@kth/stable")
        self.requires("node/0.X@%s/%s" % (self.user, self.channel))

    def config_options(self):
        KnuthConanFile.config_options(self)

    def configure(self):
        KnuthConanFile.configure(self)

        if self.options.keoken and self.options.currency != "BCH":
            self.output.warn("For the moment Keoken is only enabled for BCH. Building without Keoken support...")
            del self.options.keoken
        else:
            self.options["*"].keoken = self.options.keoken

        if self.is_keoken:
            if self.options.db == "pruned" or self.options.db == "default":
                self.output.warn("Keoken mode requires db=full and your configuration is db=%s, it has been changed automatically..." % (self.options.db,))
                self.options.db = "full"

        self.options["*"].db_readonly = self.options.db_readonly
        self.output.info("Compiling with read-only DB: %s" % (self.options.db_readonly,))

        self.options["*"].mempool = self.options.mempool
        self.output.info("Compiling with mempool: %s" % (self.options.mempool,))

    def package_id(self):
        KnuthConanFile.package_id(self)
        
        # self.info.options.console = "ANY"

    def build(self):
        cmake = self.cmake_basis()

        # cmake.definitions["WITH_CONSOLE"] = option_on_off(self.options.console)
        cmake.definitions["WITH_CONSOLE"] = option_on_off(False)

        cmake.definitions["WITH_KEOKEN"] = option_on_off(self.is_keoken)
        cmake.definitions["WITH_MEMPOOL"] = option_on_off(self.options.mempool)
        cmake.definitions["DB_READONLY_MODE"] = option_on_off(self.options.db_readonly)
        cmake.definitions["BINLOG"] = option_on_off(self.options.binlog)

        cmake.configure(source_dir=self.source_folder)
        if not self.options.cmake_export_compile_commands:
            cmake.build()
            if self.options.tests:
                cmake.test()

    def imports(self):
        self.copy("*.h", "", "include")

    def package(self):
        self.copy("*.h", dst="include", src="include")
        self.copy("*.hpp", dst="include", src="include")
        self.copy("*.ipp", dst="include", src="include")

        # # self.copy("kth.exe", dst="bin", keep_path=False) # Windows
        # # self.copy("kth", dst="bin", keep_path=False) # Linux / Macos
        # self.copy("kth.exe", dst="bin", src="bin") # Windows
        # self.copy("kth", dst="bin", src="bin") # Linux / Macos

        self.copy("*.lib", dst="lib", keep_path=False)
        self.copy("*.dll", dst="bin", keep_path=False)
        self.copy("*.dylib*", dst="lib", keep_path=False)
        self.copy("*.so", dst="lib", keep_path=False)
        self.copy("*.a", dst="lib", keep_path=False)

    def package_info(self):
        self.cpp_info.includedirs = ['include']
        self.cpp_info.libs = ["kth-rpc"]

