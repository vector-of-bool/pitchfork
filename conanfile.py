from extras.pf_conan.pf_conan import CMakeConanFile


class PitchforkConanFile(CMakeConanFile):
    name = 'pf'
    version = '0.1.0'
    requires = (
        'Catch2/2.8.0@catchorg/stable',
        'spdlog/1.3.1@bincrafters/stable',
        'boost/1.70.0@conan/stable',
    )
    build_args = ['-DBUILD_SPEC=NO']

    @property
    def exports_sources(self):
        return super().exports_sources + ['!extras/vscode-pitchfork/node_modules/*']
