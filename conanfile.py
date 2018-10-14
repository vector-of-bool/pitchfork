from extras.pf_conan.pf_conan import CMakeConanFile


class PitchforkConanFile(CMakeConanFile):
    name = 'pf'
    version = '0.1.0'
    requires = (
        'catch2/2.3.0@bincrafters/stable',
        'spdlog/1.1.0@bincrafters/stable',
        'boost/1.68.0@conan/stable',
    )
    build_args = ['-DBUILD_SPEC=NO']

    @property
    def exports_sources(self):
        return super().exports_sources + ['!extras/vscode-pitchfork/node_modules/*']
