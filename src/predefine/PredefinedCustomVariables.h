#ifndef CHTL_PREDEFINED_CUSTOM_VARIABLES_H
#define CHTL_PREDEFINED_CUSTOM_VARIABLES_H

#include <string>

namespace chtl {

// 预定义的自定义变量组
class PredefinedCustomVariables {
public:
    // 颜色方案
    static const std::string DefaultTheme;
    static const std::string DarkTheme;
    static const std::string LightTheme;
    
    // 间距系统
    static const std::string SpacingSystem;
    
    // 字体系统
    static const std::string FontSystem;
    
    // 断点系统
    static const std::string BreakpointSystem;
    
    // 动画时间
    static const std::string AnimationDurations;
    
    // 层级系统
    static const std::string ZIndexSystem;
    
    // 圆角系统
    static const std::string BorderRadiusSystem;
    
    // 阴影系统
    static const std::string ShadowSystem;
    
    // 注册所有预定义变量组
    static void registerAll();
};

} // namespace chtl

#endif // CHTL_PREDEFINED_CUSTOM_VARIABLES_H