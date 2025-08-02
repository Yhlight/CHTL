#ifndef CHTL_PREDEFINED_TEMPLATE_VARIABLES_H
#define CHTL_PREDEFINED_TEMPLATE_VARIABLES_H

#include <string>

namespace chtl {

// 预定义的模板变量组
class PredefinedTemplateVariables {
public:
    // 基础模板变量组
    static const std::string BaseColors;
    static const std::string BaseSizes;
    static const std::string BaseBreakpoints;
    
    // 注册所有预定义模板变量组
    static void registerAll();
};

} // namespace chtl

#endif // CHTL_PREDEFINED_TEMPLATE_VARIABLES_H