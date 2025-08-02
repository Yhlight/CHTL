#include "../src/parser/ConfigParser.h"
#include "../src/lexer/BasicLexer.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace chtl;

void testConfigurationParsing() {
    std::cout << "=== 配置解析测试 ===" << std::endl;
    
    // 简单的Configuration测试
    std::string testCode = R"(
[Configuration]
{
    INDEX_INITIAL_COUNT = 5;
    CUSTOM_STYLE = true;
    DEBUG_MODE = false;
}

html {
    head {
        title { "ConfigParser测试" }
    }
    body {
        div { "测试内容" }
    }
}
)";

    // 词法分析
    BasicLexer lexer(testCode);
    auto tokens = lexer.tokenize();
    
    std::cout << "Token数量: " << tokens.size() << std::endl;
    
    // 语法分析
    ConfigParser parser(tokens);
    auto ast = parser.parse();
    
    if (ast) {
        std::cout << "✅ Configuration解析成功" << std::endl;
        std::cout << "AST调试信息: " << ast->toDebugString() << std::endl;
    } else {
        std::cout << "❌ Configuration解析失败" << std::endl;
    }
    
    // 检查配置错误
    auto configErrors = parser.getConfigErrors();
    if (!configErrors.empty()) {
        std::cout << "配置错误: " << std::endl;
        for (const auto& error : configErrors) {
            std::cout << "  - " << error << std::endl;
        }
    }
}

void testCustomKeywordMapping() {
    std::cout << "\n=== 自定义关键字映射测试 ===" << std::endl;
    
    std::string testCode = R"(
[Configuration]
{
    DEBUG_MODE = true;
    
    [Name] ElementGroup
    {
        容器 = "container";
        按钮 = "button";
        卡片 = "card";
        
        prefix = "el-";
        default_class = "element";
    }
    
    [Name] StyleGroup
    {
        主要颜色 = "primary-color";
        背景颜色 = "background-color";
        
        prefix = "sg-";
        namespace = "StyleGroup";
    }
}

html {
    body {
        容器 {
            class: "main-container";
            按钮 {
                text { "点击" }
            }
        }
    }
}
)";

    BasicLexer lexer(testCode);
    auto tokens = lexer.tokenize();
    
    ConfigParser parser(tokens);
    auto ast = parser.parse();
    
    if (ast) {
        std::cout << "✅ 自定义关键字解析成功" << std::endl;
        
        // 检查自定义关键字映射
        auto mappings = parser.getCustomKeywordMappings();
        std::cout << "自定义关键字映射: " << std::endl;
        for (const auto& pair : mappings) {
            std::cout << "  " << pair.first << " -> " << pair.second << std::endl;
        }
    } else {
        std::cout << "❌ 自定义关键字解析失败" << std::endl;
    }
}

void testConfigValidation() {
    std::cout << "\n=== 配置验证测试 ===" << std::endl;
    
    ConfigValidator validator;
    
    // 测试有效的配置选项
    auto validConfig = std::make_shared<ConfigurationNode>();
    auto validOption = std::make_shared<ConfigOptionNode>(
        ConfigOptionNode::OptionType::INDEX_INITIAL_COUNT,
        ConfigOptionNode::ConfigValue(10)
    );
    validConfig->addConfigOption(validOption);
    
    bool isValid = validator.validateConfiguration(validConfig);
    std::cout << "有效配置验证: " << (isValid ? "✅ 通过" : "❌ 失败") << std::endl;
    
    // 测试无效的配置选项
    auto invalidOption = std::make_shared<ConfigOptionNode>(
        ConfigOptionNode::OptionType::INDEX_INITIAL_COUNT,
        ConfigOptionNode::ConfigValue(-5)  // 负数无效
    );
    validConfig->addConfigOption(invalidOption);
    
    isValid = validator.validateConfiguration(validConfig);
    std::cout << "无效配置验证: " << (isValid ? "❌ 未检测到错误" : "✅ 正确检测到错误") << std::endl;
    
    auto errors = validator.getValidationErrors();
    if (!errors.empty()) {
        std::cout << "验证错误: " << std::endl;
        for (const auto& error : errors) {
            std::cout << "  - " << error << std::endl;
        }
    }
}

void testParserFactory() {
    std::cout << "\n=== 解析器工厂测试 ===" << std::endl;
    
    std::string testCode = R"(
[Configuration]
{
    DEBUG_MODE = true;
}
)";

    BasicLexer lexer(testCode);
    auto tokens = lexer.tokenize();
    
    // 测试标准解析器
    auto standardParser = ConfigParserFactory::createStandardParser(tokens);
    auto ast1 = standardParser->parse();
    std::cout << "标准解析器: " << (ast1 ? "✅ 成功" : "❌ 失败") << std::endl;
    
    // 测试自定义关键字解析器
    std::unordered_map<std::string, std::string> customMappings = {
        {"容器", "container"},
        {"按钮", "button"}
    };
    auto customParser = ConfigParserFactory::createCustomKeywordParser(tokens, customMappings);
    auto ast2 = customParser->parse();
    std::cout << "自定义关键字解析器: " << (ast2 ? "✅ 成功" : "❌ 失败") << std::endl;
    
    // 测试调试模式解析器
    auto debugParser = ConfigParserFactory::createDebugParser(tokens);
    auto ast3 = debugParser->parse();
    std::cout << "调试模式解析器: " << (ast3 ? "✅ 成功" : "❌ 失败") << std::endl;
}

void testConfigParseState() {
    std::cout << "\n=== 配置解析状态测试 ===" << std::endl;
    
    ConfigParseState state;
    
    // 测试状态管理
    std::cout << "初始状态: " << (int)state.getCurrentState() << std::endl;
    
    state.setState(ConfigParseState::State::IN_CONFIGURATION);
    std::cout << "设置状态: " << (int)state.getCurrentState() << std::endl;
    
    state.pushState(ConfigParseState::State::IN_NAME_BLOCK);
    std::cout << "推入状态: " << (int)state.getCurrentState() << std::endl;
    
    auto previousState = state.popState();
    std::cout << "弹出状态: " << (int)previousState << " -> " << (int)state.getCurrentState() << std::endl;
    
    // 测试计数功能
    state.incrementOptionCount();
    state.incrementNameBlockCount();
    state.incrementCustomKeywordCount();
    
    std::cout << "计数: 选项=" << state.getOptionCount() 
              << ", 名称块=" << state.getNameBlockCount()
              << ", 自定义关键字=" << state.getCustomKeywordCount() << std::endl;
    
    // 测试错误处理
    state.addError("测试错误");
    std::cout << "错误状态: " << (state.hasErrors() ? "有错误" : "无错误") << std::endl;
}

int main() {
    std::cout << "CHTL ConfigParser 测试开始" << std::endl;
    std::cout << "===========================" << std::endl;
    
    try {
        testConfigurationParsing();
        testCustomKeywordMapping();
        testConfigValidation();
        testParserFactory();
        testConfigParseState();
        
        std::cout << "\n✅ 所有ConfigParser测试完成" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "❌ 测试异常: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}