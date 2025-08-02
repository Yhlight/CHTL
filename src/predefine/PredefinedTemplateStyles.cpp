#include "PredefinedTemplateStyles.h"
#include "Registry.h"
#include "PredefinedParser.h"
#include "../node/Template.h"

namespace chtl {

// 基础模板样式定义
const std::string PredefinedTemplateStyles::BaseButton = R"(
[Template] @Style BaseButton {
    display: "inline-block";
    padding: "0.375rem 0.75rem";
    font-size: "1rem";
    line-height: "1.5";
    text-align: "center";
    text-decoration: "none";
    vertical-align: "middle";
    cursor: "pointer";
    user-select: "none";
    border: "1px solid transparent";
    border-radius: "0.25rem";
    transition: "all 0.15s ease-in-out";
}
)";

const std::string PredefinedTemplateStyles::BaseInput = R"(
[Template] @Style BaseInput {
    display: "block";
    width: "100%";
    padding: "0.375rem 0.75rem";
    font-size: "1rem";
    line-height: "1.5";
    color: "#495057";
    background-color: "#fff";
    background-clip: "padding-box";
    border: "1px solid #ced4da";
    border-radius: "0.25rem";
    transition: "border-color 0.15s ease-in-out, box-shadow 0.15s ease-in-out";
}
)";

const std::string PredefinedTemplateStyles::BaseCard = R"(
[Template] @Style BaseCard {
    position: "relative";
    display: "flex";
    flex-direction: "column";
    min-width: "0";
    word-wrap: "break-word";
    background-color: "#fff";
    background-clip: "border-box";
    border: "1px solid rgba(0,0,0,.125)";
    border-radius: "0.25rem";
}
)";

// 注册所有预定义模板样式
void PredefinedTemplateStyles::registerAll() {
    auto& registry = Registry::getInstance();
    
    registry.registerTemplateStyle("BaseButton", []() {
        return PredefinedParser::parseTemplateStyle("BaseButton", BaseButton);
    });
    
    registry.registerTemplateStyle("BaseInput", []() {
        return PredefinedParser::parseTemplateStyle("BaseInput", BaseInput);
    });
    
    registry.registerTemplateStyle("BaseCard", []() {
        return PredefinedParser::parseTemplateStyle("BaseCard", BaseCard);
    });
}

// 静态初始化
static struct TemplateStylesInitializer {
    TemplateStylesInitializer() {
        PredefinedTemplateStyles::registerAll();
    }
} templateStylesInitializer;

} // namespace chtl