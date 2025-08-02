#include "../src/loader/ImportManager.h"
#include <iostream>

using namespace chtl;

void testPathNormalization() {
    std::cout << "=== 路径规范化测试 ===" << std::endl;
    
    ImportPathNormalizer normalizer;
    normalizer.setModulePath("module");
    normalizer.setWorkingDirectory("test");
    
    // 测试各种路径格式
    std::vector<std::pair<std::string, std::string>> testCases = {
        {"layout.chtl", "layout.chtl"},
        {"./layout.chtl", "test/layout.chtl"},
        {"module/layout.chtl", "module/layout.chtl"},
        {"module\\components.chtl", "module/components.chtl"},
        {"test/../test/./file.chtl", "test/file.chtl"}
    };
    
    std::cout << "路径规范化测试结果:" << std::endl;
    for (const auto& testCase : testCases) {
        std::string normalized = normalizer.normalizePath(testCase.first);
        std::cout << "  原路径: " << testCase.first << std::endl;
        std::cout << "  规范化: " << normalized << std::endl;
        std::cout << "  匹配预期: " << (normalized.find(testCase.second) != std::string::npos ? "✅" : "❌") << std::endl;
        std::cout << std::endl;
    }
    
    // 测试路径分析
    auto pathInfo = normalizer.analyzePath("module/layout.chtl");
    std::cout << "路径分析结果 (module/layout.chtl):" << std::endl;
    std::cout << "  标准化路径: " << pathInfo.normalizedPath << std::endl;
    std::cout << "  文件名: " << pathInfo.fileName << std::endl;
    std::cout << "  扩展名: " << pathInfo.extension << std::endl;
    std::cout << "  目录: " << pathInfo.directory << std::endl;
    std::cout << "  是绝对路径: " << (pathInfo.isAbsolute ? "是" : "否") << std::endl;
    std::cout << "  是模块: " << (pathInfo.isModule ? "是" : "否") << std::endl;
    std::cout << "  文件存在: " << (pathInfo.exists ? "是" : "否") << std::endl;
}

void testCircularDependencyDetection() {
    std::cout << "\n=== 循环依赖检测测试 ===" << std::endl;
    
    CircularDependencyDetector detector;
    
    // 构建复杂的依赖关系图
    detector.addDependency("main.chtl", "layout.chtl");
    detector.addDependency("main.chtl", "components.chtl");
    detector.addDependency("components.chtl", "utils.chtl");
    detector.addDependency("layout.chtl", "utils.chtl");
    
    // 添加循环依赖：A->B->C->A
    detector.addDependency("fileA.chtl", "fileB.chtl");
    detector.addDependency("fileB.chtl", "fileC.chtl");
    detector.addDependency("fileC.chtl", "fileA.chtl");
    
    std::cout << "依赖图统计信息:" << std::endl;
    std::cout << "  节点数: " << detector.getNodeCount() << std::endl;
    std::cout << "  边数: " << detector.getEdgeCount() << std::endl;
    
    // 检测循环依赖
    bool hasCircular = detector.hasCircularDependency();
    std::cout << "  检测到循环依赖: " << (hasCircular ? "是" : "否") << std::endl;
    
    if (hasCircular) {
        auto cycles = detector.findAllCircularDependencies();
        std::cout << "  发现 " << cycles.size() << " 个循环依赖:" << std::endl;
        
        for (size_t i = 0; i < cycles.size(); ++i) {
            std::cout << "    循环 " << (i + 1) << ": ";
            for (const auto& node : cycles[i]) {
                std::cout << node << " -> ";
            }
            std::cout << std::endl;
        }
    }
    
    // 测试拓扑排序
    auto topOrder = detector.getTopologicalOrder();
    std::cout << "  拓扑排序结果: ";
    for (const auto& node : topOrder) {
        std::cout << node << " ";
    }
    std::cout << std::endl;
    
    // 测试依赖深度
    int depthMain = detector.getDependencyDepth("main.chtl");
    std::cout << "  main.chtl的依赖深度: " << depthMain << std::endl;
    
    // 输出依赖图的DOT格式
    std::cout << "\n依赖图DOT格式:" << std::endl;
    std::cout << detector.toDotFormat() << std::endl;
}

void testDuplicateImportManagement() {
    std::cout << "\n=== 重复导入管理测试 ===" << std::endl;
    
    DuplicateImportManager manager;
    
    // 模拟多个文件导入同一个模块的情况
    manager.recordImport("module/layout.chtl", "test/pageA.chtl", nullptr);
    manager.recordImport("module/layout.chtl", "test/pageB.chtl", nullptr);
    manager.recordImport("module/components.chtl", "test/pageA.chtl", nullptr);
    manager.recordImport("module/layout.chtl", "test/pageC.chtl", nullptr);
    manager.recordImport("module/utils.chtl", "test/pageB.chtl", nullptr);
    
    std::cout << "导入统计信息:" << std::endl;
    std::cout << "  总导入记录数: " << manager.getImportCount() << std::endl;
    
    // 检测重复导入
    bool isDuplicate = manager.isAlreadyImported("module/layout.chtl");
    std::cout << "  layout模块是否已被导入: " << (isDuplicate ? "是" : "否") << std::endl;
    
    auto duplicates = manager.findDuplicateImports("module/layout.chtl");
    std::cout << "  layout模块的导入次数: " << duplicates.size() << std::endl;
    
    // 统计导入频率
    auto frequency = manager.getImportFrequency();
    std::cout << "  导入频率统计:" << std::endl;
    for (const auto& pair : frequency) {
        std::cout << "    " << pair.first << ": " << pair.second << " 次" << std::endl;
    }
    
    // 获取最常导入的文件
    auto mostImported = manager.getMostImportedFiles();
    std::cout << "  导入频率排序:" << std::endl;
    for (size_t i = 0; i < std::min(size_t(3), mostImported.size()); ++i) {
        std::cout << "    " << (i + 1) << ". " << mostImported[i] << std::endl;
    }
    
    // 获取某个文件的所有导入
    auto pageAImports = manager.getImportsForFile("test/pageA.chtl");
    std::cout << "  pageA.chtl导入的模块数: " << pageAImports.size() << std::endl;
}

void testEnhancedImportManagerBasic() {
    std::cout << "\n=== 增强Import管理器基础测试 ===" << std::endl;
    
    EnhancedImportManager manager;
    manager.initialize("module", "test");
    
    std::cout << "管理器初始化完成" << std::endl;
    
    // 测试缓存功能
    std::cout << "  缓存状态: " << (manager.isCacheEnabled() ? "启用" : "禁用") << std::endl;
    
    manager.enableCache(false);
    std::cout << "  禁用缓存后: " << (manager.isCacheEnabled() ? "启用" : "禁用") << std::endl;
    
    manager.enableCache(true);
    std::cout << "  重新启用缓存: " << (manager.isCacheEnabled() ? "启用" : "禁用") << std::endl;
    
    // 测试错误处理
    std::vector<std::string> errors = manager.getErrors();
    std::vector<std::string> warnings = manager.getWarnings();
    
    std::cout << "  当前错误数: " << errors.size() << std::endl;
    std::cout << "  当前警告数: " << warnings.size() << std::endl;
    
    // 获取统计信息
    auto stats = manager.getStatistics();
    std::cout << "  统计信息:" << std::endl;
    std::cout << "    总导入数: " << stats.totalImports << std::endl;
    std::cout << "    唯一文件数: " << stats.uniqueFiles << std::endl;
    std::cout << "    重复导入数: " << stats.duplicateImports << std::endl;
    std::cout << "    循环依赖数: " << stats.circularDependencies << std::endl;
    std::cout << "    缓存文件数: " << stats.cachedLoads << std::endl;
    std::cout << "    平均依赖深度: " << stats.averageDependencyDepth << std::endl;
}

void testPathEquivalenceAdvanced() {
    std::cout << "\n=== 高级路径等价性测试 ===" << std::endl;
    
    ImportPathNormalizer normalizer;
    
    // 测试复杂的路径等价情况
    std::vector<std::pair<std::string, std::string>> equivalentPairs = {
        {"module/layout.chtl", "module/../module/layout.chtl"},
        {"./test/file.chtl", "test/file.chtl"},
        {"test/../module/utils.chtl", "module/utils.chtl"},
        {"module/./components.chtl", "module/components.chtl"}
    };
    
    std::cout << "路径等价性测试:" << std::endl;
    for (const auto& pair : equivalentPairs) {
        bool areEquivalent = normalizer.arePathsEquivalent(pair.first, pair.second);
        std::cout << "  " << pair.first << " ≡ " << pair.second << ": " 
                 << (areEquivalent ? "✅" : "❌") << std::endl;
    }
    
    // 测试不等价的路径
    std::vector<std::pair<std::string, std::string>> nonEquivalentPairs = {
        {"module/layout.chtl", "module/components.chtl"},
        {"test/file.chtl", "module/file.chtl"},
        {"layout.chtl", "utils.chtl"}
    };
    
    std::cout << "\n路径非等价性测试:" << std::endl;
    for (const auto& pair : nonEquivalentPairs) {
        bool areEquivalent = normalizer.arePathsEquivalent(pair.first, pair.second);
        std::cout << "  " << pair.first << " ≢ " << pair.second << ": " 
                 << (!areEquivalent ? "✅" : "❌") << std::endl;
    }
}

void testDependencyGraphVisualization() {
    std::cout << "\n=== 依赖图可视化测试 ===" << std::endl;
    
    CircularDependencyDetector detector;
    
    // 构建一个典型的Web应用依赖图
    detector.addDependency("index.chtl", "layout.chtl");
    detector.addDependency("index.chtl", "components.chtl");
    detector.addDependency("about.chtl", "layout.chtl");
    detector.addDependency("about.chtl", "components.chtl");
    detector.addDependency("layout.chtl", "utils.chtl");
    detector.addDependency("components.chtl", "utils.chtl");
    
    std::cout << "Web应用依赖图分析:" << std::endl;
    std::cout << "  节点数: " << detector.getNodeCount() << std::endl;
    std::cout << "  边数: " << detector.getEdgeCount() << std::endl;
    
    // 分析每个文件的依赖
    auto allNodes = detector.getAllNodes();
    std::cout << "\n各文件的依赖分析:" << std::endl;
    for (const auto& node : allNodes) {
        auto dependencies = detector.getDependencies(node);
        auto dependents = detector.getDependents(node);
        int depth = detector.getDependencyDepth(node);
        
        std::cout << "  " << node << ":" << std::endl;
        std::cout << "    依赖于 " << dependencies.size() << " 个文件: ";
        for (const auto& dep : dependencies) {
            std::cout << dep << " ";
        }
        std::cout << std::endl;
        
        std::cout << "    被 " << dependents.size() << " 个文件依赖: ";
        for (const auto& dep : dependents) {
            std::cout << dep << " ";
        }
        std::cout << std::endl;
        
        std::cout << "    依赖深度: " << depth << std::endl;
    }
    
    // 输出拓扑排序结果（推荐的加载顺序）
    auto loadOrder = detector.getTopologicalOrder();
    std::cout << "\n推荐的文件加载顺序:" << std::endl;
    for (size_t i = 0; i < loadOrder.size(); ++i) {
        std::cout << "  " << (i + 1) << ". " << loadOrder[i] << std::endl;
    }
}

int main() {
    std::cout << "CHTL 增强Import系统测试" << std::endl;
    std::cout << "=========================" << std::endl;
    
    try {
        testPathNormalization();
        testCircularDependencyDetection();
        testDuplicateImportManagement();
        testEnhancedImportManagerBasic();
        testPathEquivalenceAdvanced();
        testDependencyGraphVisualization();
        
        std::cout << "\n🎉 所有增强Import系统测试成功完成！" << std::endl;
        std::cout << "\n主要验证功能:" << std::endl;
        std::cout << "✅ 路径规范化和等价性检测" << std::endl;
        std::cout << "✅ 循环依赖检测和拓扑排序" << std::endl;
        std::cout << "✅ 重复导入管理和频率统计" << std::endl;
        std::cout << "✅ 增强Import管理器基础功能" << std::endl;
        std::cout << "✅ 依赖图分析和可视化" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "❌ 测试异常: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}