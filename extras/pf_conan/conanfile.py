from pf_conan import CMakeConanFile
import conans


class PitchforkConanFile(conans.ConanFile):
    name = 'pf-conan'
    version = '0.1.0'
    exports = 'pf_conan/*'
    # We have no settings
    settings = None
    # We have no options
    options = None

    def package_id(self):
        self.info.header_only()
