#ifndef CHTL_PREDEFINED_CUSTOM_STYLES_H
#define CHTL_PREDEFINED_CUSTOM_STYLES_H

#include <string>

namespace chtl {

// 预定义的自定义样式
class PredefinedCustomStyles {
public:
    // 文本样式
    static const std::string DefaultText;
    static const std::string Heading1;
    static const std::string Heading2;
    static const std::string Heading3;
    static const std::string SmallText;
    static const std::string BoldText;
    static const std::string ItalicText;
    static const std::string UnderlineText;
    
    // 颜色主题
    static const std::string PrimaryColor;
    static const std::string SecondaryColor;
    static const std::string SuccessColor;
    static const std::string WarningColor;
    static const std::string ErrorColor;
    static const std::string InfoColor;
    
    // 布局样式
    static const std::string FlexCenter;
    static const std::string FlexRow;
    static const std::string FlexColumn;
    static const std::string GridContainer;
    static const std::string Hidden;
    
    // 边框和阴影
    static const std::string BorderDefault;
    static const std::string BorderRounded;
    static const std::string ShadowSmall;
    static const std::string ShadowMedium;
    static const std::string ShadowLarge;
    
    // 间距
    static const std::string PaddingSmall;
    static const std::string PaddingMedium;
    static const std::string PaddingLarge;
    static const std::string MarginSmall;
    static const std::string MarginMedium;
    static const std::string MarginLarge;
    
    // 动画
    static const std::string FadeIn;
    static const std::string SlideIn;
    static const std::string ScaleIn;
    
    // 响应式
    static const std::string MobileOnly;
    static const std::string DesktopOnly;
    
    // 注册所有预定义样式
    static void registerAll();
};

} // namespace chtl

#endif // CHTL_PREDEFINED_CUSTOM_STYLES_H