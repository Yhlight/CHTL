#ifndef CHTL_PREDEFINED_CUSTOM_ELEMENTS_H
#define CHTL_PREDEFINED_CUSTOM_ELEMENTS_H

#include <string>

namespace chtl {

// 预定义的自定义元素
class PredefinedCustomElements {
public:
    // 布局组件
    static const std::string Container;
    static const std::string Card;
    static const std::string Header;
    static const std::string Footer;
    static const std::string Sidebar;
    static const std::string MainContent;
    
    // 导航组件
    static const std::string Navbar;
    static const std::string NavItem;
    static const std::string Breadcrumb;
    static const std::string Pagination;
    
    // 表单组件
    static const std::string FormGroup;
    static const std::string InputGroup;
    static const std::string ButtonGroup;
    
    // 内容组件
    static const std::string Article;
    static const std::string Section;
    static const std::string Alert;
    static const std::string Badge;
    static const std::string Modal;
    static const std::string Tooltip;
    
    // 媒体组件
    static const std::string MediaObject;
    static const std::string Gallery;
    static const std::string Carousel;
    
    // 列表组件
    static const std::string ListGroup;
    static const std::string ListItem;
    
    // 注册所有预定义元素
    static void registerAll();
};

} // namespace chtl

#endif // CHTL_PREDEFINED_CUSTOM_ELEMENTS_H