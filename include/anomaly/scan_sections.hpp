#pragma once
#include "memory.hpp"
#include <algorithm>
#include <limits>
#include <span>
namespace ue5mem {
// Section aliases are scan requests only; never rewrite PE headers or publish
// synthetic memory ranges spanning gaps between executable sections.
inline std::vector<SectionInfo> SelectScanSections(const ModuleInfo& module,
    std::span<const SectionInfo> sections,std::string_view requested) {
    std::vector<SectionInfo> result;
    if(!module.size||module.base>std::numeric_limits<std::uintptr_t>::max()-module.size)return result;
    const auto valid=[&](const SectionInfo& s){return s.virtual_size&&s.base>=module.base&&
        s.base-module.base<module.size&&s.virtual_size<=module.size-(s.base-module.base);};
    const auto exact=std::find_if(sections.begin(),sections.end(),[&](const auto& s){return s.name==requested;});
    if(exact!=sections.end()) {if(valid(*exact))result.push_back(*exact);return result;}
    if(requested!=".text")return result;
    for(const auto& s:sections)if(valid(s)&&(s.characteristics&IMAGE_SCN_MEM_EXECUTE))result.push_back(s);
    return result;
}
}
