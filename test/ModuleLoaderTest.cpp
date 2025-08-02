#include "../src/loader/ChtlLoader.h"
#include <iostream>

using namespace chtl;

void testModuleScanning() {
    std::cout << "=== 模块扫描测试 ===" << std::endl;
    
    ChtlLoader loader;
    loader.setModulePath("module");
    
    auto modules = loader.getAvailableModules();
    std::cout << "发现模块数量: " << modules.size() << std::endl;
    
    for (const auto& module : modules) {
        std::cout << "  - " << module << std::endl;
    }
}

void testModuleLoading() {
    std::cout << "\n=== 模块加载测试 ===" << std::endl;
    
    ChtlLoader loader;
    loader.setModulePath("module");
    
    // 尝试加载layout模块
    auto result = loader.loadFile("layout");
    
    if (result.success) {
        std::cout << "✅ layout模块加载成功" << std::endl;
        std::cout << "文件路径: " << result.filePath << std::endl;
        std::cout << "内容大小: " << result.content.length() << " 字符" << std::endl;
    } else {
        std::cout << "❌ layout模块加载失败" << std::endl;
        for (const auto& error : result.errors) {
            std::cout << "  错误: " << error << std::endl;
        }
    }
}

void testFilePathResolution() {
    std::cout << "\n=== 文件路径解析测试 ===" << std::endl;
    
    ChtlLoader loader;
    loader.setModulePath("module");
    loader.setWorkingDirectory("test");
    
    // 测试模块路径解析
    std::string layoutPath = loader.resolveFilePath("layout");
    std::cout << "layout模块路径: " << layoutPath << std::endl;
    
    // 测试用户文件路径解析
    std::string userPath = loader.resolveFilePath("simple_parser_test.chtl");
    std::cout << "用户文件路径: " << userPath << std::endl;
    
    // 测试文件存在性
    std::cout << "layout模块存在: " << (loader.fileExists(layoutPath) ? "是" : "否") << std::endl;
    std::cout << "用户文件存在: " << (loader.fileExists(userPath) ? "是" : "否") << std::endl;
}

void testModuleRegistry() {
    std::cout << "\n=== 模块注册表测试 ===" << std::endl;
    
    auto& registry = ModuleRegistry::getInstance();
    
    // 初始化内置模块
    registry.initializeBuiltinModules("module");
    
    auto moduleNames = registry.getAllModuleNames();
    std::cout << "注册模块数量: " << moduleNames.size() << std::endl;
    
    for (const auto& name : moduleNames) {
        std::cout << "  模块: " << name << std::endl;
        std::cout << "    路径: " << registry.getModulePath(name) << std::endl;
        std::cout << "    描述: " << registry.getModuleDescription(name) << std::endl;
        std::cout << "    版本: " << registry.getModuleVersion(name) << std::endl;
    }
}

void testPathUtilities() {
    std::cout << "\n=== 路径工具测试 ===" << std::endl;
    
    // 测试路径规范化
    std::string path1 = "module\\layout.chtl";
    std::string normalized = ChtlLoader::normalizePath(path1);
    std::cout << "原路径: " << path1 << std::endl;
    std::cout << "规范化: " << normalized << std::endl;
    
    // 测试路径分解
    std::string testPath = "module/components.chtl";
    std::cout << "测试路径: " << testPath << std::endl;
    std::cout << "  文件名: " << ChtlLoader::getFileName(testPath) << std::endl;
    std::cout << "  目录: " << ChtlLoader::getDirectory(testPath) << std::endl;
    std::cout << "  扩展名: " << ChtlLoader::getFileExtension(testPath) << std::endl;
    
    // 测试路径连接
    std::string joined = ChtlLoader::joinPath("module", "utils.chtl");
    std::cout << "连接路径: " << joined << std::endl;
    
    // 测试绝对路径检测
    std::cout << "是绝对路径:" << std::endl;
    std::cout << "  /usr/bin: " << (ChtlLoader::isAbsolutePath("/usr/bin") ? "是" : "否") << std::endl;
    std::cout << "  C:\\Windows: " << (ChtlLoader::isAbsolutePath("C:\\Windows") ? "是" : "否") << std::endl;
    std::cout << "  module/layout: " << (ChtlLoader::isAbsolutePath("module/layout") ? "是" : "否") << std::endl;
}

int main() {
    std::cout << "CHTL 模块加载器测试" << std::endl;
    std::cout << "==================" << std::endl;
    
    try {
        testModuleScanning();
        testModuleLoading();
        testFilePathResolution();
        testModuleRegistry();
        testPathUtilities();
        
        std::cout << "\n✅ 所有模块加载器测试完成" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "❌ 测试异常: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}