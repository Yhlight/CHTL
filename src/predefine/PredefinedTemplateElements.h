#ifndef CHTL_PREDEFINED_TEMPLATE_ELEMENTS_H
#define CHTL_PREDEFINED_TEMPLATE_ELEMENTS_H

#include <string>

namespace chtl {

// 预定义的模板元素
class PredefinedTemplateElements {
public:
    // 基础模板元素
    static const std::string BaseLayout;
    static const std::string BaseForm;
    static const std::string BaseList;
    
    // 注册所有预定义模板元素
    static void registerAll();
};

} // namespace chtl

#endif // CHTL_PREDEFINED_TEMPLATE_ELEMENTS_H