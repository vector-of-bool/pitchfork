#ifndef FILE_TEMPLATE_HPP_INCLUDED
#define FILE_TEMPLATE_HPP_INCLUDED

#include <pf/fs.hpp>

#include <cmrc/cmrc.hpp>
#include <kainjow/mustache.hpp>

#include <string>

namespace pf {

class template_renderer {
    fs::path                  _base_dir;
    kainjow::mustache::data   _template_data;
    cmrc::embedded_filesystem _fs;

public:
    template_renderer(fs::path dir, cmrc::embedded_filesystem fs)
        : _base_dir(dir)
        , _fs(fs) {}

    template <typename Key, typename What>
    void set(Key&& k, What&& w) {
        _template_data.set(k, std::forward<What>(w));
    }

    std::string render(const std::string& inpath) const;
    void        render_to_file(const std::string& respath, const fs::path& outpath) const {
        auto content = render(respath);
        pf::write_file(_base_dir / outpath, content);
    }
};

}  // namespace pf

#endif  // FILE_TEMPLATE_HPP_INCLUDED