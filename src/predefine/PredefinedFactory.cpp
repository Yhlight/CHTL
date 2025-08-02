#include "PredefinedRegistry.h"

namespace chtl {

// PredefinedFactory 实现
std::unique_ptr<PredefinedStyleGroup> PredefinedFactory::createStyleGroup(const std::string& name, 
                                                                          PredefinedType type) {
    return std::make_unique<PredefinedStyleGroup>(name, type);
}

std::unique_ptr<PredefinedElement> PredefinedFactory::createElement(const std::string& name, 
                                                                   const std::string& tagName,
                                                                   PredefinedType type) {
    auto element = std::make_unique<PredefinedElement>(name, type);
    element->setRootElement(tagName);
    return element;
}

std::unique_ptr<PredefinedVariableGroup> PredefinedFactory::createVariableGroup(const std::string& name,
                                                                                PredefinedType type) {
    return std::make_unique<PredefinedVariableGroup>(name, type);
}

// 常用样式组创建器
std::unique_ptr<PredefinedStyleGroup> PredefinedFactory::createCommonButtonStyle() {
    auto buttonStyle = createStyleGroup("ButtonStyle", PredefinedType::CUSTOM_STYLE);
    
    // 基础按钮样式
    buttonStyle->addProperty("display", "inline-block");
    buttonStyle->addProperty("padding", "8px 16px");
    buttonStyle->addProperty("margin", "4px");
    buttonStyle->addProperty("border", "1px solid #ccc");
    buttonStyle->addProperty("border-radius", "4px");
    buttonStyle->addProperty("background-color", "#f5f5f5");
    buttonStyle->addProperty("color", "#333");
    buttonStyle->addProperty("text-decoration", "none");
    buttonStyle->addProperty("cursor", "pointer");
    buttonStyle->addProperty("font-family", "Arial, sans-serif");
    buttonStyle->addProperty("font-size", "14px");
    buttonStyle->addProperty("transition", "all 0.3s ease");
    
    // 悬停状态
    std::unordered_map<std::string, std::string> hoverProps = {
        {"background-color", "#e0e0e0"},
        {"border-color", "#999"}
    };
    buttonStyle->addRule(":hover", hoverProps);
    
    // 激活状态
    std::unordered_map<std::string, std::string> activeProps = {
        {"background-color", "#d5d5d5"},
        {"transform", "scale(0.98)"}
    };
    buttonStyle->addRule(":active", activeProps);
    
    return buttonStyle;
}

std::unique_ptr<PredefinedStyleGroup> PredefinedFactory::createCommonContainerStyle() {
    auto containerStyle = createStyleGroup("ContainerStyle", PredefinedType::CUSTOM_STYLE);
    
    // 容器样式
    containerStyle->addProperty("max-width", "1200px");
    containerStyle->addProperty("margin", "0 auto");
    containerStyle->addProperty("padding", "20px");
    containerStyle->addProperty("box-sizing", "border-box");
    
    // 响应式设计
    std::unordered_map<std::string, std::string> mobileProps = {
        {"padding", "10px"},
        {"max-width", "100%"}
    };
    containerStyle->addRule("@media (max-width: 768px)", mobileProps);
    
    return containerStyle;
}

std::unique_ptr<PredefinedStyleGroup> PredefinedFactory::createCommonTextStyle() {
    auto textStyle = createStyleGroup("TextStyle", PredefinedType::CUSTOM_STYLE);
    
    // 基础文本样式
    textStyle->addProperty("font-family", "Arial, 'Helvetica Neue', Helvetica, sans-serif");
    textStyle->addProperty("line-height", "1.6");
    textStyle->addProperty("color", "#333");
    
    // 标题样式
    std::unordered_map<std::string, std::string> headingProps = {
        {"font-weight", "bold"},
        {"margin-top", "1.5em"},
        {"margin-bottom", "0.5em"}
    };
    textStyle->addRule("h1, h2, h3, h4, h5, h6", headingProps);
    
    // 段落样式
    std::unordered_map<std::string, std::string> paragraphProps = {
        {"margin-bottom", "1em"}
    };
    textStyle->addRule("p", paragraphProps);
    
    return textStyle;
}

// 常用元素创建器
std::unique_ptr<PredefinedElement> PredefinedFactory::createCommonCard() {
    auto card = createElement("Card", "div", PredefinedType::CUSTOM_ELEMENT);
    
    // 设置根元素属性
    std::unordered_map<std::string, std::string> cardAttrs = {
        {"class", "card"}
    };
    card->setRootElement("div", cardAttrs);
    
    // 添加卡片样式
    auto cardStyle = std::make_shared<StyleBlockNode>(NodePosition());
    cardStyle->addInlineProperty("background", "white");
    cardStyle->addInlineProperty("border-radius", "8px");
    cardStyle->addInlineProperty("box-shadow", "0 2px 4px rgba(0,0,0,0.1)");
    cardStyle->addInlineProperty("padding", "20px");
    cardStyle->addInlineProperty("margin", "10px");
    card->addStyleChild(cardStyle);
    
    // 添加标题区域
    card->addChildElement("div", {{"class", "card-header"}});
    
    // 添加内容区域
    card->addChildElement("div", {{"class", "card-body"}});
    
    // 添加底部区域
    card->addChildElement("div", {{"class", "card-footer"}});
    
    return card;
}

std::unique_ptr<PredefinedElement> PredefinedFactory::createCommonButton() {
    auto button = createElement("Button", "button", PredefinedType::CUSTOM_ELEMENT);
    
    // 设置根元素属性
    std::unordered_map<std::string, std::string> buttonAttrs = {
        {"type", "button"},
        {"class", "btn"}
    };
    button->setRootElement("button", buttonAttrs);
    
    // 添加按钮样式
    auto buttonStyle = std::make_shared<StyleBlockNode>(NodePosition());
    buttonStyle->addInlineProperty("padding", "10px 20px");
    buttonStyle->addInlineProperty("border", "none");
    buttonStyle->addInlineProperty("border-radius", "4px");
    buttonStyle->addInlineProperty("background-color", "#007bff");
    buttonStyle->addInlineProperty("color", "white");
    buttonStyle->addInlineProperty("cursor", "pointer");
    buttonStyle->addInlineProperty("font-size", "16px");
    buttonStyle->addInlineProperty("transition", "background-color 0.3s ease");
    button->addStyleChild(buttonStyle);
    
    // 添加默认文本
    button->addTextChild("Click Me");
    
    return button;
}

std::unique_ptr<PredefinedElement> PredefinedFactory::createCommonNavbar() {
    auto navbar = createElement("Navbar", "nav", PredefinedType::CUSTOM_ELEMENT);
    
    // 设置根元素属性
    std::unordered_map<std::string, std::string> navAttrs = {
        {"class", "navbar"}
    };
    navbar->setRootElement("nav", navAttrs);
    
    // 添加导航栏样式
    auto navStyle = std::make_shared<StyleBlockNode>(NodePosition());
    navStyle->addInlineProperty("display", "flex");
    navStyle->addInlineProperty("justify-content", "space-between");
    navStyle->addInlineProperty("align-items", "center");
    navStyle->addInlineProperty("padding", "1rem 2rem");
    navStyle->addInlineProperty("background-color", "#f8f9fa");
    navStyle->addInlineProperty("border-bottom", "1px solid #dee2e6");
    navbar->addStyleChild(navStyle);
    
    // 添加品牌区域
    navbar->addChildElement("div", {{"class", "navbar-brand"}});
    
    // 添加导航链接容器
    auto navLinks = std::make_shared<ElementNode>("ul", NodePosition());
    navLinks->addAttribute("class", "navbar-nav");
    
    // 添加样式
    auto linksStyle = std::make_shared<StyleBlockNode>(NodePosition());
    linksStyle->addInlineProperty("display", "flex");
    linksStyle->addInlineProperty("list-style", "none");
    linksStyle->addInlineProperty("margin", "0");
    linksStyle->addInlineProperty("padding", "0");
    navLinks->addChild(linksStyle);
    
    navbar->addChild(navLinks);
    
    return navbar;
}

// 常用变量组创建器
std::unique_ptr<PredefinedVariableGroup> PredefinedFactory::createCommonColors() {
    auto colors = createVariableGroup("Colors", PredefinedType::CUSTOM_VAR);
    
    // 主色调
    colors->addVariable("primary", "#007bff");
    colors->addVariable("secondary", "#6c757d");
    colors->addVariable("success", "#28a745");
    colors->addVariable("danger", "#dc3545");
    colors->addVariable("warning", "#ffc107");
    colors->addVariable("info", "#17a2b8");
    colors->addVariable("light", "#f8f9fa");
    colors->addVariable("dark", "#343a40");
    
    // 灰度色阶
    colors->addVariable("gray100", "#f8f9fa");
    colors->addVariable("gray200", "#e9ecef");
    colors->addVariable("gray300", "#dee2e6");
    colors->addVariable("gray400", "#ced4da");
    colors->addVariable("gray500", "#adb5bd");
    colors->addVariable("gray600", "#6c757d");
    colors->addVariable("gray700", "#495057");
    colors->addVariable("gray800", "#343a40");
    colors->addVariable("gray900", "#212529");
    
    // 文本颜色
    colors->addVariable("textPrimary", "#212529");
    colors->addVariable("textSecondary", "#6c757d");
    colors->addVariable("textMuted", "#868e96");
    
    return colors;
}

std::unique_ptr<PredefinedVariableGroup> PredefinedFactory::createCommonSizes() {
    auto sizes = createVariableGroup("Sizes", PredefinedType::CUSTOM_VAR);
    
    // 字体大小
    sizes->addVariable("fontSizeXs", "0.75rem");
    sizes->addVariable("fontSizeSm", "0.875rem");
    sizes->addVariable("fontSizeBase", "1rem");
    sizes->addVariable("fontSizeLg", "1.25rem");
    sizes->addVariable("fontSizeXl", "1.5rem");
    sizes->addVariable("fontSizeXxl", "2rem");
    
    // 容器宽度
    sizes->addVariable("containerSm", "540px");
    sizes->addVariable("containerMd", "720px");
    sizes->addVariable("containerLg", "960px");
    sizes->addVariable("containerXl", "1140px");
    sizes->addVariable("containerXxl", "1320px");
    
    // 边框半径
    sizes->addVariable("borderRadiusSm", "0.25rem");
    sizes->addVariable("borderRadius", "0.375rem");
    sizes->addVariable("borderRadiusLg", "0.5rem");
    sizes->addVariable("borderRadiusXl", "1rem");
    sizes->addVariable("borderRadiusXxl", "2rem");
    sizes->addVariable("borderRadiusPill", "50rem");
    
    return sizes;
}

std::unique_ptr<PredefinedVariableGroup> PredefinedFactory::createCommonSpacing() {
    auto spacing = createVariableGroup("Spacing", PredefinedType::CUSTOM_VAR);
    
    // 间距系统（基于 0.25rem 单位）
    spacing->addVariable("space0", "0");
    spacing->addVariable("space1", "0.25rem");
    spacing->addVariable("space2", "0.5rem");
    spacing->addVariable("space3", "0.75rem");
    spacing->addVariable("space4", "1rem");
    spacing->addVariable("space5", "1.25rem");
    spacing->addVariable("space6", "1.5rem");
    spacing->addVariable("space8", "2rem");
    spacing->addVariable("space10", "2.5rem");
    spacing->addVariable("space12", "3rem");
    spacing->addVariable("space16", "4rem");
    spacing->addVariable("space20", "5rem");
    spacing->addVariable("space24", "6rem");
    spacing->addVariable("space32", "8rem");
    
    // 常用组合间距
    spacing->addVariable("spacingXs", "4px");
    spacing->addVariable("spacingSm", "8px");
    spacing->addVariable("spacingMd", "16px");
    spacing->addVariable("spacingLg", "24px");
    spacing->addVariable("spacingXl", "32px");
    spacing->addVariable("spacingXxl", "48px");
    
    return spacing;
}

// 从配置创建的占位符实现
std::unique_ptr<PredefinedStyleGroup> PredefinedFactory::createStyleGroupFromConfig(
    const std::unordered_map<std::string, std::string>& config) {
    auto it = config.find("name");
    if (it == config.end()) {
        return nullptr;
    }
    
    auto styleGroup = createStyleGroup(it->second);
    
    // 从配置中添加属性
    for (const auto& pair : config) {
        if (pair.first != "name" && pair.first != "type") {
            styleGroup->addProperty(pair.first, pair.second);
        }
    }
    
    return styleGroup;
}

std::unique_ptr<PredefinedElement> PredefinedFactory::createElementFromConfig(
    const std::unordered_map<std::string, std::string>& config) {
    auto nameIt = config.find("name");
    auto tagIt = config.find("tag");
    
    if (nameIt == config.end()) {
        return nullptr;
    }
    
    std::string tagName = (tagIt != config.end()) ? tagIt->second : "div";
    return createElement(nameIt->second, tagName);
}

std::unique_ptr<PredefinedVariableGroup> PredefinedFactory::createVariableGroupFromConfig(
    const std::unordered_map<std::string, std::string>& config) {
    auto it = config.find("name");
    if (it == config.end()) {
        return nullptr;
    }
    
    auto varGroup = createVariableGroup(it->second);
    
    // 从配置中添加变量
    for (const auto& pair : config) {
        if (pair.first != "name" && pair.first != "type") {
            varGroup->addVariable(pair.first, pair.second);
        }
    }
    
    return varGroup;
}

} // namespace chtl