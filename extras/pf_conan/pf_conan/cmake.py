import conans
from os import path
import tempfile
import subprocess

NINJA_TEST = '''
build test-target: phony
'''


def has_ninja():
    with tempfile.NamedTemporaryFile(suffix='.ninja') as tmpfd:
        tmpfd.write(NINJA_TEST.encode('utf-8'))
        for exe in ('ninja-build', 'ninja'):
            try:
                subprocess.check_call([exe, '-f', tmpfd.name])
                return True
            except FileNotFoundError:
                pass
            except subprocess.CalledProcessError:
                pass
        return False


class CMakeConanFile(conans.ConanFile):
    generators = 'cmake_paths'
    no_copy_source = True
    settings = 'os', 'arch', 'compiler'
    build_args = []
    exports_sources = '*', '!build/*', '!_build/*'

    def build(self):
        use_ninja = has_ninja()
        for bt in ('Debug', 'Release'):
            cmake = conans.CMake(
                self,
                generator='Ninja' if use_ninja else None,
            )
            cmake.configure(
                source_folder=self.source_folder,
                args=list(
                    [
                        '-DCMAKE_BUILD_TYPE={}'.format(bt),
                        '--no-warn-unused-cli',
                    ],
                    *self.build_args
                ),
            )
            cmake.build()

    @property
    def has_pub_include_dir(self):
        return path.exists(path.join(self.source_folder, 'include'))

    def package_info(self):
        if self.has_pub_include_dir:
            self.cpp_info.inclduedirs = ['include']
        else:
            self.cpp_info.inclduedirs = ['src']
