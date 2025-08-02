#include "PredefinedTemplateVariables.h"
#include "Registry.h"
#include "PredefinedParser.h"
#include "../node/Template.h"

namespace chtl {

// 基础模板变量组定义
const std::string PredefinedTemplateVariables::BaseColors = R"(
[Template] @Var BaseColors {
    black: "#000000";
    white: "#ffffff";
    gray: "#808080";
    red: "#ff0000";
    green: "#00ff00";
    blue: "#0000ff";
    yellow: "#ffff00";
    cyan: "#00ffff";
    magenta: "#ff00ff";
}
)";

const std::string PredefinedTemplateVariables::BaseSizes = R"(
[Template] @Var BaseSizes {
    xs: "0.5rem";
    sm: "0.75rem";
    md: "1rem";
    lg: "1.5rem";
    xl: "2rem";
    xxl: "3rem";
}
)";

const std::string PredefinedTemplateVariables::BaseBreakpoints = R"(
[Template] @Var BaseBreakpoints {
    mobile: "480px";
    tablet: "768px";
    desktop: "1024px";
    wide: "1280px";
    ultrawide: "1920px";
}
)";

// 注册所有预定义模板变量组
void PredefinedTemplateVariables::registerAll() {
    auto& registry = Registry::getInstance();
    
    registry.registerTemplateVar("BaseColors", []() {
        return PredefinedParser::parseTemplateVar("BaseColors", BaseColors);
    });
    
    registry.registerTemplateVar("BaseSizes", []() {
        return PredefinedParser::parseTemplateVar("BaseSizes", BaseSizes);
    });
    
    registry.registerTemplateVar("BaseBreakpoints", []() {
        return PredefinedParser::parseTemplateVar("BaseBreakpoints", BaseBreakpoints);
    });
}

// 静态初始化
static struct TemplateVariablesInitializer {
    TemplateVariablesInitializer() {
        PredefinedTemplateVariables::registerAll();
    }
} templateVariablesInitializer;

} // namespace chtl