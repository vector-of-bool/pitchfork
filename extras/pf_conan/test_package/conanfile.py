import conans
import subprocess

pf = conans.python_requires('pf-conan/[*]@test/test')


class ConanFile(pf.CMakeConanFile):
    name = 'pf-conan-test'
    version = 'test'

    def test(self):
        subprocess.check_call([f'{self.build_folder}/my_program', 'Test string'])
