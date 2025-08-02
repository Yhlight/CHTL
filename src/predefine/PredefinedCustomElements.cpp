#include "PredefinedCustomElements.h"
#include "Registry.h"
#include "PredefinedParser.h"
#include "../node/Custom.h"

namespace chtl {

// 布局组件定义
const std::string PredefinedCustomElements::Container = R"(
[Custom] @Element Container {
    div {
        style {
            @Style PaddingMedium;
            max-width: "1200px";
            margin: "0 auto";
        }
    }
}
)";

const std::string PredefinedCustomElements::Card = R"(
[Custom] @Element Card {
    div {
        style {
            @Style BorderDefault;
            @Style BorderRounded;
            @Style ShadowSmall;
            @Style PaddingMedium;
            background-color: "white";
        }
        
        div {
            style {
                @Style MarginSmall;
            }
        }
    }
}
)";

const std::string PredefinedCustomElements::Header = R"(
[Custom] @Element Header {
    header {
        style {
            @Style PaddingMedium;
            background-color: "#f8f9fa";
            border-bottom: "1px solid #dee2e6";
        }
    }
}
)";

const std::string PredefinedCustomElements::Footer = R"(
[Custom] @Element Footer {
    footer {
        style {
            @Style PaddingLarge;
            background-color: "#343a40";
            color: "white";
            margin-top: "auto";
        }
    }
}
)";

const std::string PredefinedCustomElements::Sidebar = R"(
[Custom] @Element Sidebar {
    aside {
        style {
            @Style PaddingMedium;
            min-width: "250px";
            background-color: "#f8f9fa";
            border-right: "1px solid #dee2e6";
        }
    }
}
)";

const std::string PredefinedCustomElements::MainContent = R"(
[Custom] @Element MainContent {
    main {
        style {
            @Style PaddingLarge;
            flex: "1";
        }
    }
}
)";

// 导航组件定义
const std::string PredefinedCustomElements::Navbar = R"(
[Custom] @Element Navbar {
    nav {
        style {
            @Style FlexRow;
            @Style PaddingMedium;
            background-color: "#343a40";
            color: "white";
        }
        
        ul {
            style {
                @Style FlexRow;
                list-style: "none";
                padding: "0";
                margin: "0";
            }
        }
    }
}
)";

const std::string PredefinedCustomElements::NavItem = R"(
[Custom] @Element NavItem {
    li {
        style {
            @Style PaddingSmall;
            margin: "0 0.5rem";
        }
        
        a {
            style {
                color: "inherit";
                text-decoration: "none";
            }
        }
    }
}
)";

const std::string PredefinedCustomElements::Breadcrumb = R"(
[Custom] @Element Breadcrumb {
    nav {
        style {
            @Style PaddingSmall;
        }
        
        ol {
            style {
                @Style FlexRow;
                list-style: "none";
                padding: "0";
                margin: "0";
            }
            
            li {
                style {
                    margin-right: "0.5rem";
                }
            }
        }
    }
}
)";

const std::string PredefinedCustomElements::Pagination = R"(
[Custom] @Element Pagination {
    nav {
        ul {
            style {
                @Style FlexRow;
                @Style FlexCenter;
                list-style: "none";
                padding: "0";
            }
            
            li {
                style {
                    @Style BorderDefault;
                    @Style PaddingSmall;
                    margin: "0 2px";
                }
            }
        }
    }
}
)";

// 表单组件定义
const std::string PredefinedCustomElements::FormGroup = R"(
[Custom] @Element FormGroup {
    div {
        style {
            @Style MarginMedium;
        }
        
        label {
            style {
                display: "block";
                margin-bottom: "0.5rem";
                font-weight: "bold";
            }
        }
        
        input {
            style {
                width: "100%";
                padding: "0.375rem 0.75rem";
                border: "1px solid #ced4da";
                border-radius: "0.25rem";
            }
        }
    }
}
)";

const std::string PredefinedCustomElements::InputGroup = R"(
[Custom] @Element InputGroup {
    div {
        style {
            @Style FlexRow;
            align-items: "stretch";
        }
        
        span {
            style {
                @Style PaddingSmall;
                @Style BorderDefault;
                background-color: "#e9ecef";
            }
        }
        
        input {
            style {
                flex: "1";
                padding: "0.375rem 0.75rem";
                border: "1px solid #ced4da";
            }
        }
    }
}
)";

const std::string PredefinedCustomElements::ButtonGroup = R"(
[Custom] @Element ButtonGroup {
    div {
        style {
            @Style FlexRow;
        }
        
        button {
            style {
                @Style PaddingSmall;
                @Style BorderDefault;
                background-color: "#f8f9fa";
                cursor: "pointer";
                margin: "0";
            }
        }
    }
}
)";

// 内容组件定义
const std::string PredefinedCustomElements::Article = R"(
[Custom] @Element Article {
    article {
        style {
            @Style MarginLarge;
        }
        
        h1 {
            style {
                @Style Heading1;
            }
        }
        
        p {
            style {
                @Style DefaultText;
                margin-bottom: "1rem";
            }
        }
    }
}
)";

const std::string PredefinedCustomElements::Section = R"(
[Custom] @Element Section {
    section {
        style {
            @Style PaddingLarge;
            @Style MarginMedium;
        }
    }
}
)";

const std::string PredefinedCustomElements::Alert = R"(
[Custom] @Element Alert {
    div {
        style {
            @Style PaddingMedium;
            @Style BorderDefault;
            @Style BorderRounded;
            background-color: "#d4edda";
            border-color: "#c3e6cb";
            color: "#155724";
        }
    }
}
)";

const std::string PredefinedCustomElements::Badge = R"(
[Custom] @Element Badge {
    span {
        style {
            display: "inline-block";
            padding: "0.25em 0.4em";
            font-size: "75%";
            font-weight: "700";
            line-height: "1";
            text-align: "center";
            white-space: "nowrap";
            vertical-align: "baseline";
            border-radius: "0.25rem";
            background-color: "#6c757d";
            color: "white";
        }
    }
}
)";

const std::string PredefinedCustomElements::Modal = R"(
[Custom] @Element Modal {
    div {
        style {
            position: "fixed";
            top: "0";
            left: "0";
            width: "100%";
            height: "100%";
            background-color: "rgba(0,0,0,0.5)";
            @Style FlexCenter;
            z-index: "1000";
        }
        
        div {
            style {
                @Style Card;
                max-width: "500px";
                width: "90%";
            }
        }
    }
}
)";

const std::string PredefinedCustomElements::Tooltip = R"(
[Custom] @Element Tooltip {
    div {
        style {
            position: "relative";
            display: "inline-block";
        }
        
        span {
            style {
                visibility: "hidden";
                position: "absolute";
                bottom: "125%";
                left: "50%";
                transform: "translateX(-50%)";
                @Style PaddingSmall;
                background-color: "#333";
                color: "white";
                text-align: "center";
                border-radius: "6px";
                white-space: "nowrap";
            }
        }
    }
}
)";

// 媒体组件定义
const std::string PredefinedCustomElements::MediaObject = R"(
[Custom] @Element MediaObject {
    div {
        style {
            @Style FlexRow;
            align-items: "flex-start";
        }
        
        img {
            style {
                margin-right: "1rem";
                max-width: "100px";
            }
        }
        
        div {
            style {
                flex: "1";
            }
        }
    }
}
)";

const std::string PredefinedCustomElements::Gallery = R"(
[Custom] @Element Gallery {
    div {
        style {
            @Style GridContainer;
        }
        
        div {
            style {
                overflow: "hidden";
                @Style BorderRounded;
            }
            
            img {
                style {
                    width: "100%";
                    height: "auto";
                    display: "block";
                }
            }
        }
    }
}
)";

const std::string PredefinedCustomElements::Carousel = R"(
[Custom] @Element Carousel {
    div {
        style {
            position: "relative";
            overflow: "hidden";
        }
        
        div {
            style {
                @Style FlexRow;
                transition: "transform 0.3s ease";
            }
            
            div {
                style {
                    min-width: "100%";
                }
            }
        }
    }
}
)";

// 列表组件定义
const std::string PredefinedCustomElements::ListGroup = R"(
[Custom] @Element ListGroup {
    ul {
        style {
            list-style: "none";
            padding: "0";
            margin: "0";
            @Style BorderDefault;
            @Style BorderRounded;
        }
    }
}
)";

const std::string PredefinedCustomElements::ListItem = R"(
[Custom] @Element ListItem {
    li {
        style {
            @Style PaddingMedium;
            border-bottom: "1px solid #dee2e6";
        }
    }
}
)";

// 注册所有预定义元素
void PredefinedCustomElements::registerAll() {
    auto& registry = Registry::getInstance();
    
    // 布局组件
    registry.registerCustomElement("Container", []() {
        return PredefinedParser::parseCustomElement("Container", Container);
    });
    
    registry.registerCustomElement("Card", []() {
        return PredefinedParser::parseCustomElement("Card", Card);
    });
    
    registry.registerCustomElement("Header", []() {
        return PredefinedParser::parseCustomElement("Header", Header);
    });
    
    registry.registerCustomElement("Footer", []() {
        return PredefinedParser::parseCustomElement("Footer", Footer);
    });
    
    registry.registerCustomElement("Sidebar", []() {
        return PredefinedParser::parseCustomElement("Sidebar", Sidebar);
    });
    
    registry.registerCustomElement("MainContent", []() {
        return PredefinedParser::parseCustomElement("MainContent", MainContent);
    });
    
    // 导航组件
    registry.registerCustomElement("Navbar", []() {
        return PredefinedParser::parseCustomElement("Navbar", Navbar);
    });
    
    registry.registerCustomElement("NavItem", []() {
        return PredefinedParser::parseCustomElement("NavItem", NavItem);
    });
    
    registry.registerCustomElement("Breadcrumb", []() {
        return PredefinedParser::parseCustomElement("Breadcrumb", Breadcrumb);
    });
    
    registry.registerCustomElement("Pagination", []() {
        return PredefinedParser::parseCustomElement("Pagination", Pagination);
    });
    
    // 表单组件
    registry.registerCustomElement("FormGroup", []() {
        return PredefinedParser::parseCustomElement("FormGroup", FormGroup);
    });
    
    registry.registerCustomElement("InputGroup", []() {
        return PredefinedParser::parseCustomElement("InputGroup", InputGroup);
    });
    
    registry.registerCustomElement("ButtonGroup", []() {
        return PredefinedParser::parseCustomElement("ButtonGroup", ButtonGroup);
    });
    
    // 内容组件
    registry.registerCustomElement("Article", []() {
        return PredefinedParser::parseCustomElement("Article", Article);
    });
    
    registry.registerCustomElement("Section", []() {
        return PredefinedParser::parseCustomElement("Section", Section);
    });
    
    registry.registerCustomElement("Alert", []() {
        return PredefinedParser::parseCustomElement("Alert", Alert);
    });
    
    registry.registerCustomElement("Badge", []() {
        return PredefinedParser::parseCustomElement("Badge", Badge);
    });
    
    registry.registerCustomElement("Modal", []() {
        return PredefinedParser::parseCustomElement("Modal", Modal);
    });
    
    registry.registerCustomElement("Tooltip", []() {
        return PredefinedParser::parseCustomElement("Tooltip", Tooltip);
    });
    
    // 媒体组件
    registry.registerCustomElement("MediaObject", []() {
        return PredefinedParser::parseCustomElement("MediaObject", MediaObject);
    });
    
    registry.registerCustomElement("Gallery", []() {
        return PredefinedParser::parseCustomElement("Gallery", Gallery);
    });
    
    registry.registerCustomElement("Carousel", []() {
        return PredefinedParser::parseCustomElement("Carousel", Carousel);
    });
    
    // 列表组件
    registry.registerCustomElement("ListGroup", []() {
        return PredefinedParser::parseCustomElement("ListGroup", ListGroup);
    });
    
    registry.registerCustomElement("ListItem", []() {
        return PredefinedParser::parseCustomElement("ListItem", ListItem);
    });
}

// 静态初始化
static struct CustomElementsInitializer {
    CustomElementsInitializer() {
        PredefinedCustomElements::registerAll();
    }
} customElementsInitializer;

} // namespace chtl