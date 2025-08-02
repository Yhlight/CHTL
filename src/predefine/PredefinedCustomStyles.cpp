#include "PredefinedCustomStyles.h"
#include "Registry.h"
#include "PredefinedParser.h"
#include "../node/Custom.h"

namespace chtl {

// 文本样式定义
const std::string PredefinedCustomStyles::DefaultText = R"(
[Custom] @Style DefaultText {
    font-family: "Arial, sans-serif";
    font-size: "16px";
    line-height: "1.5";
    color: "#333";
}
)";

const std::string PredefinedCustomStyles::Heading1 = R"(
[Custom] @Style Heading1 {
    @Style DefaultText;
    font-size: "2.5rem";
    font-weight: "bold";
    margin-top: "1rem";
    margin-bottom: "1rem";
}
)";

const std::string PredefinedCustomStyles::Heading2 = R"(
[Custom] @Style Heading2 {
    @Style DefaultText;
    font-size: "2rem";
    font-weight: "bold";
    margin-top: "0.8rem";
    margin-bottom: "0.8rem";
}
)";

const std::string PredefinedCustomStyles::Heading3 = R"(
[Custom] @Style Heading3 {
    @Style DefaultText;
    font-size: "1.5rem";
    font-weight: "bold";
    margin-top: "0.6rem";
    margin-bottom: "0.6rem";
}
)";

const std::string PredefinedCustomStyles::SmallText = R"(
[Custom] @Style SmallText {
    @Style DefaultText;
    font-size: "0.875rem";
}
)";

const std::string PredefinedCustomStyles::BoldText = R"(
[Custom] @Style BoldText {
    font-weight: "bold";
}
)";

const std::string PredefinedCustomStyles::ItalicText = R"(
[Custom] @Style ItalicText {
    font-style: "italic";
}
)";

const std::string PredefinedCustomStyles::UnderlineText = R"(
[Custom] @Style UnderlineText {
    text-decoration: "underline";
}
)";

// 颜色主题定义
const std::string PredefinedCustomStyles::PrimaryColor = R"(
[Custom] @Style PrimaryColor {
    color: "#007bff";
    background-color: "#007bff";
}
)";

const std::string PredefinedCustomStyles::SecondaryColor = R"(
[Custom] @Style SecondaryColor {
    color: "#6c757d";
    background-color: "#6c757d";
}
)";

const std::string PredefinedCustomStyles::SuccessColor = R"(
[Custom] @Style SuccessColor {
    color: "#28a745";
    background-color: "#28a745";
}
)";

const std::string PredefinedCustomStyles::WarningColor = R"(
[Custom] @Style WarningColor {
    color: "#ffc107";
    background-color: "#ffc107";
}
)";

const std::string PredefinedCustomStyles::ErrorColor = R"(
[Custom] @Style ErrorColor {
    color: "#dc3545";
    background-color: "#dc3545";
}
)";

const std::string PredefinedCustomStyles::InfoColor = R"(
[Custom] @Style InfoColor {
    color: "#17a2b8";
    background-color: "#17a2b8";
}
)";

// 布局样式定义
const std::string PredefinedCustomStyles::FlexCenter = R"(
[Custom] @Style FlexCenter {
    display: "flex";
    justify-content: "center";
    align-items: "center";
}
)";

const std::string PredefinedCustomStyles::FlexRow = R"(
[Custom] @Style FlexRow {
    display: "flex";
    flex-direction: "row";
}
)";

const std::string PredefinedCustomStyles::FlexColumn = R"(
[Custom] @Style FlexColumn {
    display: "flex";
    flex-direction: "column";
}
)";

const std::string PredefinedCustomStyles::GridContainer = R"(
[Custom] @Style GridContainer {
    display: "grid";
    grid-template-columns: "repeat(auto-fit, minmax(250px, 1fr))";
    gap: "1rem";
}
)";

const std::string PredefinedCustomStyles::Hidden = R"(
[Custom] @Style Hidden {
    display: "none";
}
)";

// 边框和阴影定义
const std::string PredefinedCustomStyles::BorderDefault = R"(
[Custom] @Style BorderDefault {
    border: "1px solid #ddd";
}
)";

const std::string PredefinedCustomStyles::BorderRounded = R"(
[Custom] @Style BorderRounded {
    border-radius: "0.25rem";
}
)";

const std::string PredefinedCustomStyles::ShadowSmall = R"(
[Custom] @Style ShadowSmall {
    box-shadow: "0 1px 3px rgba(0,0,0,0.12), 0 1px 2px rgba(0,0,0,0.24)";
}
)";

const std::string PredefinedCustomStyles::ShadowMedium = R"(
[Custom] @Style ShadowMedium {
    box-shadow: "0 3px 6px rgba(0,0,0,0.16), 0 3px 6px rgba(0,0,0,0.23)";
}
)";

const std::string PredefinedCustomStyles::ShadowLarge = R"(
[Custom] @Style ShadowLarge {
    box-shadow: "0 10px 20px rgba(0,0,0,0.19), 0 6px 6px rgba(0,0,0,0.23)";
}
)";

// 间距定义
const std::string PredefinedCustomStyles::PaddingSmall = R"(
[Custom] @Style PaddingSmall {
    padding: "0.5rem";
}
)";

const std::string PredefinedCustomStyles::PaddingMedium = R"(
[Custom] @Style PaddingMedium {
    padding: "1rem";
}
)";

const std::string PredefinedCustomStyles::PaddingLarge = R"(
[Custom] @Style PaddingLarge {
    padding: "2rem";
}
)";

const std::string PredefinedCustomStyles::MarginSmall = R"(
[Custom] @Style MarginSmall {
    margin: "0.5rem";
}
)";

const std::string PredefinedCustomStyles::MarginMedium = R"(
[Custom] @Style MarginMedium {
    margin: "1rem";
}
)";

const std::string PredefinedCustomStyles::MarginLarge = R"(
[Custom] @Style MarginLarge {
    margin: "2rem";
}
)";

// 动画定义
const std::string PredefinedCustomStyles::FadeIn = R"(
[Custom] @Style FadeIn {
    animation: "fadeIn 0.3s ease-in";
}
)";

const std::string PredefinedCustomStyles::SlideIn = R"(
[Custom] @Style SlideIn {
    animation: "slideIn 0.3s ease-out";
}
)";

const std::string PredefinedCustomStyles::ScaleIn = R"(
[Custom] @Style ScaleIn {
    animation: "scaleIn 0.3s ease-out";
}
)";

// 响应式定义
const std::string PredefinedCustomStyles::MobileOnly = R"(
[Custom] @Style MobileOnly {
    @media "(min-width: 768px)" {
        display: "none";
    }
}
)";

const std::string PredefinedCustomStyles::DesktopOnly = R"(
[Custom] @Style DesktopOnly {
    @media "(max-width: 767px)" {
        display: "none";
    }
}
)";

// 注册所有预定义样式
void PredefinedCustomStyles::registerAll() {
    auto& registry = Registry::getInstance();
    
    // 文本样式
    registry.registerCustomStyle("DefaultText", []() {
        return PredefinedParser::parseCustomStyle("DefaultText", DefaultText);
    });
    
    registry.registerCustomStyle("Heading1", []() {
        return PredefinedParser::parseCustomStyle("Heading1", Heading1);
    });
    
    registry.registerCustomStyle("Heading2", []() {
        return PredefinedParser::parseCustomStyle("Heading2", Heading2);
    });
    
    registry.registerCustomStyle("Heading3", []() {
        return PredefinedParser::parseCustomStyle("Heading3", Heading3);
    });
    
    registry.registerCustomStyle("SmallText", []() {
        return PredefinedParser::parseCustomStyle("SmallText", SmallText);
    });
    
    registry.registerCustomStyle("BoldText", []() {
        return PredefinedParser::parseCustomStyle("BoldText", BoldText);
    });
    
    registry.registerCustomStyle("ItalicText", []() {
        return PredefinedParser::parseCustomStyle("ItalicText", ItalicText);
    });
    
    registry.registerCustomStyle("UnderlineText", []() {
        return PredefinedParser::parseCustomStyle("UnderlineText", UnderlineText);
    });
    
    // 颜色主题
    registry.registerCustomStyle("PrimaryColor", []() {
        return PredefinedParser::parseCustomStyle("PrimaryColor", PrimaryColor);
    });
    
    registry.registerCustomStyle("SecondaryColor", []() {
        return PredefinedParser::parseCustomStyle("SecondaryColor", SecondaryColor);
    });
    
    registry.registerCustomStyle("SuccessColor", []() {
        return PredefinedParser::parseCustomStyle("SuccessColor", SuccessColor);
    });
    
    registry.registerCustomStyle("WarningColor", []() {
        return PredefinedParser::parseCustomStyle("WarningColor", WarningColor);
    });
    
    registry.registerCustomStyle("ErrorColor", []() {
        return PredefinedParser::parseCustomStyle("ErrorColor", ErrorColor);
    });
    
    registry.registerCustomStyle("InfoColor", []() {
        return PredefinedParser::parseCustomStyle("InfoColor", InfoColor);
    });
    
    // 布局样式
    registry.registerCustomStyle("FlexCenter", []() {
        return PredefinedParser::parseCustomStyle("FlexCenter", FlexCenter);
    });
    
    registry.registerCustomStyle("FlexRow", []() {
        return PredefinedParser::parseCustomStyle("FlexRow", FlexRow);
    });
    
    registry.registerCustomStyle("FlexColumn", []() {
        return PredefinedParser::parseCustomStyle("FlexColumn", FlexColumn);
    });
    
    registry.registerCustomStyle("GridContainer", []() {
        return PredefinedParser::parseCustomStyle("GridContainer", GridContainer);
    });
    
    registry.registerCustomStyle("Hidden", []() {
        return PredefinedParser::parseCustomStyle("Hidden", Hidden);
    });
    
    // 边框和阴影
    registry.registerCustomStyle("BorderDefault", []() {
        return PredefinedParser::parseCustomStyle("BorderDefault", BorderDefault);
    });
    
    registry.registerCustomStyle("BorderRounded", []() {
        return PredefinedParser::parseCustomStyle("BorderRounded", BorderRounded);
    });
    
    registry.registerCustomStyle("ShadowSmall", []() {
        return PredefinedParser::parseCustomStyle("ShadowSmall", ShadowSmall);
    });
    
    registry.registerCustomStyle("ShadowMedium", []() {
        return PredefinedParser::parseCustomStyle("ShadowMedium", ShadowMedium);
    });
    
    registry.registerCustomStyle("ShadowLarge", []() {
        return PredefinedParser::parseCustomStyle("ShadowLarge", ShadowLarge);
    });
    
    // 间距
    registry.registerCustomStyle("PaddingSmall", []() {
        return PredefinedParser::parseCustomStyle("PaddingSmall", PaddingSmall);
    });
    
    registry.registerCustomStyle("PaddingMedium", []() {
        return PredefinedParser::parseCustomStyle("PaddingMedium", PaddingMedium);
    });
    
    registry.registerCustomStyle("PaddingLarge", []() {
        return PredefinedParser::parseCustomStyle("PaddingLarge", PaddingLarge);
    });
    
    registry.registerCustomStyle("MarginSmall", []() {
        return PredefinedParser::parseCustomStyle("MarginSmall", MarginSmall);
    });
    
    registry.registerCustomStyle("MarginMedium", []() {
        return PredefinedParser::parseCustomStyle("MarginMedium", MarginMedium);
    });
    
    registry.registerCustomStyle("MarginLarge", []() {
        return PredefinedParser::parseCustomStyle("MarginLarge", MarginLarge);
    });
    
    // 动画
    registry.registerCustomStyle("FadeIn", []() {
        return PredefinedParser::parseCustomStyle("FadeIn", FadeIn);
    });
    
    registry.registerCustomStyle("SlideIn", []() {
        return PredefinedParser::parseCustomStyle("SlideIn", SlideIn);
    });
    
    registry.registerCustomStyle("ScaleIn", []() {
        return PredefinedParser::parseCustomStyle("ScaleIn", ScaleIn);
    });
    
    // 响应式
    registry.registerCustomStyle("MobileOnly", []() {
        return PredefinedParser::parseCustomStyle("MobileOnly", MobileOnly);
    });
    
    registry.registerCustomStyle("DesktopOnly", []() {
        return PredefinedParser::parseCustomStyle("DesktopOnly", DesktopOnly);
    });
}

// 静态初始化
static struct CustomStylesInitializer {
    CustomStylesInitializer() {
        PredefinedCustomStyles::registerAll();
    }
} customStylesInitializer;

} // namespace chtl