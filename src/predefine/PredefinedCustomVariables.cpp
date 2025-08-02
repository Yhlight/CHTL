#include "PredefinedCustomVariables.h"
#include "Registry.h"
#include "PredefinedParser.h"
#include "../node/Custom.h"

namespace chtl {

// 颜色方案定义
const std::string PredefinedCustomVariables::DefaultTheme = R"(
[Custom] @Var DefaultTheme {
    primary: "#007bff";
    secondary: "#6c757d";
    success: "#28a745";
    danger: "#dc3545";
    warning: "#ffc107";
    info: "#17a2b8";
    light: "#f8f9fa";
    dark: "#343a40";
    
    textPrimary: "#212529";
    textSecondary: "#6c757d";
    textMuted: "#adb5bd";
    
    bgPrimary: "#ffffff";
    bgSecondary: "#f8f9fa";
    bgTertiary: "#e9ecef";
    
    borderColor: "#dee2e6";
    borderColorLight: "#e9ecef";
    borderColorDark: "#ced4da";
}
)";

const std::string PredefinedCustomVariables::DarkTheme = R"(
[Custom] @Var DarkTheme {
    primary: "#0d6efd";
    secondary: "#6c757d";
    success: "#198754";
    danger: "#dc3545";
    warning: "#ffc107";
    info: "#0dcaf0";
    light: "#212529";
    dark: "#f8f9fa";
    
    textPrimary: "#ffffff";
    textSecondary: "#adb5bd";
    textMuted: "#6c757d";
    
    bgPrimary: "#212529";
    bgSecondary: "#343a40";
    bgTertiary: "#495057";
    
    borderColor: "#495057";
    borderColorLight: "#6c757d";
    borderColorDark: "#343a40";
}
)";

const std::string PredefinedCustomVariables::LightTheme = R"(
[Custom] @Var LightTheme {
    @Var DefaultTheme;
    
    primary: "#4285f4";
    secondary: "#718096";
    
    bgPrimary: "#ffffff";
    bgSecondary: "#fafafa";
    bgTertiary: "#f5f5f5";
}
)";

// 间距系统定义
const std::string PredefinedCustomVariables::SpacingSystem = R"(
[Custom] @Var SpacingSystem {
    space0: "0";
    space1: "0.25rem";
    space2: "0.5rem";
    space3: "0.75rem";
    space4: "1rem";
    space5: "1.25rem";
    space6: "1.5rem";
    space8: "2rem";
    space10: "2.5rem";
    space12: "3rem";
    space16: "4rem";
    space20: "5rem";
    space24: "6rem";
    space32: "8rem";
}
)";

// 字体系统定义
const std::string PredefinedCustomVariables::FontSystem = R"(
[Custom] @Var FontSystem {
    fontFamilyBase: "-apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif";
    fontFamilyMono: "'SFMono-Regular', Consolas, 'Liberation Mono', monospace";
    fontFamilySerif: "Georgia, 'Times New Roman', serif";
    
    fontSizeXs: "0.75rem";
    fontSizeSm: "0.875rem";
    fontSizeBase: "1rem";
    fontSizeLg: "1.125rem";
    fontSizeXl: "1.25rem";
    fontSize2xl: "1.5rem";
    fontSize3xl: "1.875rem";
    fontSize4xl: "2.25rem";
    fontSize5xl: "3rem";
    
    fontWeightLight: "300";
    fontWeightNormal: "400";
    fontWeightMedium: "500";
    fontWeightSemibold: "600";
    fontWeightBold: "700";
    
    lineHeightTight: "1.25";
    lineHeightNormal: "1.5";
    lineHeightRelaxed: "1.75";
    lineHeightLoose: "2";
}
)";

// 断点系统定义
const std::string PredefinedCustomVariables::BreakpointSystem = R"(
[Custom] @Var BreakpointSystem {
    xs: "0";
    sm: "576px";
    md: "768px";
    lg: "992px";
    xl: "1200px";
    xxl: "1400px";
    
    maxWidthSm: "540px";
    maxWidthMd: "720px";
    maxWidthLg: "960px";
    maxWidthXl: "1140px";
    maxWidthXxl: "1320px";
}
)";

// 动画时间定义
const std::string PredefinedCustomVariables::AnimationDurations = R"(
[Custom] @Var AnimationDurations {
    durationFast: "150ms";
    durationBase: "300ms";
    durationSlow: "500ms";
    durationSlowest: "1000ms";
    
    easingDefault: "ease";
    easingIn: "ease-in";
    easingOut: "ease-out";
    easingInOut: "ease-in-out";
    easingBounce: "cubic-bezier(0.68, -0.55, 0.265, 1.55)";
}
)";

// 层级系统定义
const std::string PredefinedCustomVariables::ZIndexSystem = R"(
[Custom] @Var ZIndexSystem {
    zIndexDropdown: "1000";
    zIndexSticky: "1020";
    zIndexFixed: "1030";
    zIndexModalBackdrop: "1040";
    zIndexModal: "1050";
    zIndexPopover: "1060";
    zIndexTooltip: "1070";
}
)";

// 圆角系统定义
const std::string PredefinedCustomVariables::BorderRadiusSystem = R"(
[Custom] @Var BorderRadiusSystem {
    radiusNone: "0";
    radiusSm: "0.125rem";
    radiusBase: "0.25rem";
    radiusMd: "0.375rem";
    radiusLg: "0.5rem";
    radiusXl: "0.75rem";
    radius2xl: "1rem";
    radiusFull: "9999px";
}
)";

// 阴影系统定义
const std::string PredefinedCustomVariables::ShadowSystem = R"(
[Custom] @Var ShadowSystem {
    shadowXs: "0 1px 2px 0 rgba(0, 0, 0, 0.05)";
    shadowSm: "0 1px 3px 0 rgba(0, 0, 0, 0.1), 0 1px 2px 0 rgba(0, 0, 0, 0.06)";
    shadowBase: "0 4px 6px -1px rgba(0, 0, 0, 0.1), 0 2px 4px -1px rgba(0, 0, 0, 0.06)";
    shadowMd: "0 10px 15px -3px rgba(0, 0, 0, 0.1), 0 4px 6px -2px rgba(0, 0, 0, 0.05)";
    shadowLg: "0 20px 25px -5px rgba(0, 0, 0, 0.1), 0 10px 10px -5px rgba(0, 0, 0, 0.04)";
    shadowXl: "0 25px 50px -12px rgba(0, 0, 0, 0.25)";
    shadow2xl: "0 35px 60px -15px rgba(0, 0, 0, 0.3)";
    shadowInner: "inset 0 2px 4px 0 rgba(0, 0, 0, 0.06)";
    shadowNone: "none";
}
)";

// 注册所有预定义变量组
void PredefinedCustomVariables::registerAll() {
    auto& registry = Registry::getInstance();
    
    // 颜色方案
    registry.registerCustomVar("DefaultTheme", []() {
        return PredefinedParser::parseCustomVar("DefaultTheme", DefaultTheme);
    });
    
    registry.registerCustomVar("DarkTheme", []() {
        return PredefinedParser::parseCustomVar("DarkTheme", DarkTheme);
    });
    
    registry.registerCustomVar("LightTheme", []() {
        return PredefinedParser::parseCustomVar("LightTheme", LightTheme);
    });
    
    // 间距系统
    registry.registerCustomVar("SpacingSystem", []() {
        return PredefinedParser::parseCustomVar("SpacingSystem", SpacingSystem);
    });
    
    // 字体系统
    registry.registerCustomVar("FontSystem", []() {
        return PredefinedParser::parseCustomVar("FontSystem", FontSystem);
    });
    
    // 断点系统
    registry.registerCustomVar("BreakpointSystem", []() {
        return PredefinedParser::parseCustomVar("BreakpointSystem", BreakpointSystem);
    });
    
    // 动画时间
    registry.registerCustomVar("AnimationDurations", []() {
        return PredefinedParser::parseCustomVar("AnimationDurations", AnimationDurations);
    });
    
    // 层级系统
    registry.registerCustomVar("ZIndexSystem", []() {
        return PredefinedParser::parseCustomVar("ZIndexSystem", ZIndexSystem);
    });
    
    // 圆角系统
    registry.registerCustomVar("BorderRadiusSystem", []() {
        return PredefinedParser::parseCustomVar("BorderRadiusSystem", BorderRadiusSystem);
    });
    
    // 阴影系统
    registry.registerCustomVar("ShadowSystem", []() {
        return PredefinedParser::parseCustomVar("ShadowSystem", ShadowSystem);
    });
}

// 静态初始化
static struct CustomVariablesInitializer {
    CustomVariablesInitializer() {
        PredefinedCustomVariables::registerAll();
    }
} customVariablesInitializer;

} // namespace chtl