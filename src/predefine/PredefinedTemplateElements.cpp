#include "PredefinedTemplateElements.h"
#include "Registry.h"
#include "PredefinedParser.h"
#include "../node/Template.h"

namespace chtl {

// 基础模板元素定义
const std::string PredefinedTemplateElements::BaseLayout = R"(
[Template] @Element BaseLayout {
    div {
        style {
            display: "flex";
            flex-direction: "column";
            min-height: "100vh";
        }
        
        header {
            style {
                flex-shrink: "0";
            }
        }
        
        main {
            style {
                flex: "1 0 auto";
            }
        }
        
        footer {
            style {
                flex-shrink: "0";
            }
        }
    }
}
)";

const std::string PredefinedTemplateElements::BaseForm = R"(
[Template] @Element BaseForm {
    form {
        style {
            max-width: "600px";
            margin: "0 auto";
        }
        
        div {
            style {
                margin-bottom: "1rem";
            }
            
            label {
                style {
                    display: "block";
                    margin-bottom: "0.5rem";
                }
            }
            
            input {
                style {
                    width: "100%";
                }
            }
        }
    }
}
)";

const std::string PredefinedTemplateElements::BaseList = R"(
[Template] @Element BaseList {
    div {
        ul {
            style {
                list-style: "none";
                padding: "0";
                margin: "0";
            }
            
            li {
                style {
                    padding: "0.5rem 0";
                    border-bottom: "1px solid #e0e0e0";
                }
            }
        }
    }
}
)";

// 注册所有预定义模板元素
void PredefinedTemplateElements::registerAll() {
    auto& registry = Registry::getInstance();
    
    registry.registerTemplateElement("BaseLayout", []() {
        return PredefinedParser::parseTemplateElement("BaseLayout", BaseLayout);
    });
    
    registry.registerTemplateElement("BaseForm", []() {
        return PredefinedParser::parseTemplateElement("BaseForm", BaseForm);
    });
    
    registry.registerTemplateElement("BaseList", []() {
        return PredefinedParser::parseTemplateElement("BaseList", BaseList);
    });
}

// 静态初始化
static struct TemplateElementsInitializer {
    TemplateElementsInitializer() {
        PredefinedTemplateElements::registerAll();
    }
} templateElementsInitializer;

} // namespace chtl