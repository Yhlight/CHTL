#ifndef CHTL_PREDEFINED_TEMPLATE_STYLES_H
#define CHTL_PREDEFINED_TEMPLATE_STYLES_H

#include <string>

namespace chtl {

// 预定义的模板样式
class PredefinedTemplateStyles {
public:
    // 基础模板样式
    static const std::string BaseButton;
    static const std::string BaseInput;
    static const std::string BaseCard;
    
    // 注册所有预定义模板样式
    static void registerAll();
};

} // namespace chtl

#endif // CHTL_PREDEFINED_TEMPLATE_STYLES_H