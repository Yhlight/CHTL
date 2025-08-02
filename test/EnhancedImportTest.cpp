#include "../src/loader/ImportManager.h"
#include <iostream>
#include <fstream>

using namespace chtl;

// 创建一个简单的ImportNode用于测试
std::shared_ptr<ImportNode> createTestImportNode(const std::string& fromPath) {
    auto importNode = std::make_shared<ImportNode>();
    auto fromOperator = std::make_shared<FromOperatorNode>();
    fromOperator->setSource(fromPath);
    importNode->setFromOperator(fromOperator);
    return importNode;
}

void testPathNormalization() {
    std::cout << "=== 路径规范化测试 ===" << std::endl;
    
    ImportPathNormalizer normalizer;
    normalizer.setModulePath("module");
    normalizer.setWorkingDirectory("test");
    
    // 测试各种路径格式
    std::vector<std::pair<std::string, std::string>> testCases = {
        {"layout", "layout"},
        {"./layout", "test/layout"},
        {"../module/layout.chtl", "module/layout.chtl"},
        {"module\\components.chtl", "module/components.chtl"},
        {"test/../test/./file.chtl", "test/file.chtl"}
    };
    
    for (const auto& testCase : testCases) {
        std::string normalized = normalizer.normalizePath(testCase.first);
        std::cout << "原路径: " << testCase.first << std::endl;
        std::cout << "规范化: " << normalized << std::endl;
        std::cout << "期望: " << testCase.second << std::endl;
        std::cout << "结果: " << (normalized == testCase.second ? "✅" : "❌") << std::endl;
        std::cout << std::endl;
    }
    
    // 测试路径等价性
    std::cout << "=== 路径等价性测试 ===" << std::endl;
    bool equivalent1 = normalizer.arePathsEquivalent("module/layout.chtl", "module/../module/layout.chtl");
    bool equivalent2 = normalizer.arePathsEquivalent("layout", "components");
    
    std::cout << "等价路径测试: " << (equivalent1 ? "✅" : "❌") << std::endl;
    std::cout << "不等价路径测试: " << (!equivalent2 ? "✅" : "❌") << std::endl;
}

void testCircularDependencyDetection() {
    std::cout << "\n=== 循环依赖检测测试 ===" << std::endl;
    
    CircularDependencyDetector detector;
    
    // 构建依赖关系：A->B, B->C, C->A (形成循环)
    detector.addDependency("fileA", "fileB");
    detector.addDependency("fileB", "fileC");
    detector.addDependency("fileC", "fileA");
    
    // 添加一个独立的链：D->E
    detector.addDependency("fileD", "fileE");
    
    std::cout << "依赖图信息:" << std::endl;
    std::cout << "节点数: " << detector.getNodeCount() << std::endl;
    std::cout << "边数: " << detector.getEdgeCount() << std::endl;
    
    // 检测循环依赖
    bool hasCircular = detector.hasCircularDependency();
    std::cout << "检测到循环依赖: " << (hasCircular ? "是" : "否") << std::endl;
    
    if (hasCircular) {
        auto cycles = detector.findAllCircularDependencies();
        std::cout << "发现 " << cycles.size() << " 个循环依赖:" << std::endl;
        
        for (size_t i = 0; i < cycles.size(); ++i) {
            std::cout << "循环 " << (i + 1) << ": ";
            for (const auto& node : cycles[i]) {
                std::cout << node << " -> ";
            }
            std::cout << std::endl;
        }
    }
    
    // 测试拓扑排序
    auto topOrder = detector.getTopologicalOrder();
    std::cout << "拓扑排序结果: ";
    for (const auto& node : topOrder) {
        std::cout << node << " ";
    }
    std::cout << std::endl;
    
    // 测试依赖深度
    int depthA = detector.getDependencyDepth("fileA");
    std::cout << "fileA的依赖深度: " << depthA << std::endl;
}

void testDuplicateImportManagement() {
    std::cout << "\n=== 重复导入管理测试 ===" << std::endl;
    
    DuplicateImportManager manager;
    
    // 模拟多个文件导入同一个模块
    auto import1 = createTestImportNode("layout");
    auto import2 = createTestImportNode("layout");
    auto import3 = createTestImportNode("components");
    
    manager.recordImport("module/layout.chtl", "test/fileA.chtl", import1);
    manager.recordImport("module/layout.chtl", "test/fileB.chtl", import2);
    manager.recordImport("module/components.chtl", "test/fileA.chtl", import3);
    
    std::cout << "总导入记录数: " << manager.getImportCount() << std::endl;
    
    // 检测重复导入
    bool isDuplicate = manager.isAlreadyImported("module/layout.chtl");
    std::cout << "layout是否已被导入: " << (isDuplicate ? "是" : "否") << std::endl;
    
    auto duplicates = manager.findDuplicateImports("module/layout.chtl");
    std::cout << "layout的导入次数: " << duplicates.size() << std::endl;
    
    // 统计导入频率
    auto frequency = manager.getImportFrequency();
    std::cout << "导入频率统计:" << std::endl;
    for (const auto& pair : frequency) {
        std::cout << "  " << pair.first << ": " << pair.second << " 次" << std::endl;
    }
    
    // 获取最常导入的文件
    auto mostImported = manager.getMostImportedFiles();
    if (!mostImported.empty()) {
        std::cout << "最常导入的文件: " << mostImported[0] << std::endl;
    }
}

void testEnhancedImportManager() {
    std::cout << "\n=== 增强Import管理器测试 ===" << std::endl;
    
    EnhancedImportManager manager;
    manager.initialize("module", "test");
    
    // 测试基本导入
    auto import1 = createTestImportNode("layout");
    auto result1 = manager.processImport(import1, "test/main.chtl");
    
    std::cout << "导入layout模块:" << std::endl;
    std::cout << "  成功: " << (result1.success ? "是" : "否") << std::endl;
    std::cout << "  规范化路径: " << result1.normalizedPath << std::endl;
    
    if (!result1.success) {
        std::cout << "  错误:" << std::endl;
        for (const auto& error : result1.errors) {
            std::cout << "    - " << error << std::endl;
        }
    }
    
    // 测试重复导入
    auto import2 = createTestImportNode("layout");
    auto result2 = manager.processImport(import2, "test/main.chtl");
    
    std::cout << "\n重复导入layout模块:" << std::endl;
    std::cout << "  是重复导入: " << (result2.wasDuplicate ? "是" : "否") << std::endl;
    std::cout << "  使用缓存: " << (result2.wasCached ? "是" : "否") << std::endl;
    
    if (!result2.warnings.empty()) {
        std::cout << "  警告:" << std::endl;
        for (const auto& warning : result2.warnings) {
            std::cout << "    - " << warning << std::endl;
        }
    }
    
    // 获取统计信息
    auto stats = manager.getStatistics();
    std::cout << "\nImport统计信息:" << std::endl;
    std::cout << "  总导入数: " << stats.totalImports << std::endl;
    std::cout << "  唯一文件数: " << stats.uniqueFiles << std::endl;
    std::cout << "  重复导入数: " << stats.duplicateImports << std::endl;
    std::cout << "  循环依赖数: " << stats.circularDependencies << std::endl;
    std::cout << "  缓存文件数: " << stats.cachedLoads << std::endl;
    std::cout << "  平均依赖深度: " << stats.averageDependencyDepth << std::endl;
}

void testCircularDependencyScenario() {
    std::cout << "\n=== 循环依赖场景测试 ===" << std::endl;
    
    EnhancedImportManager manager;
    manager.initialize("module", "test");
    
    // 如果存在循环依赖测试文件，测试循环依赖检测
    std::ifstream testFileA("test/circular_dependency_test.chtl");
    std::ifstream testFileB("test/circular_dependency_test_b.chtl");
    
    if (testFileA.good() && testFileB.good()) {
        std::cout << "找到循环依赖测试文件" << std::endl;
        
        // 创建ImportNode模拟循环依赖
        auto importB = createTestImportNode("test/circular_dependency_test_b");
        auto result = manager.processImport(importB, "test/circular_dependency_test.chtl");
        
        std::cout << "处理可能的循环依赖:" << std::endl;
        std::cout << "  成功: " << (result.success ? "是" : "否") << std::endl;
        
        if (!result.circularDependencyChain.empty()) {
            std::cout << "  检测到循环依赖链: ";
            for (const auto& file : result.circularDependencyChain) {
                std::cout << file << " -> ";
            }
            std::cout << std::endl;
        }
        
        if (!result.errors.empty()) {
            std::cout << "  错误:" << std::endl;
            for (const auto& error : result.errors) {
                std::cout << "    - " << error << std::endl;
            }
        }
    } else {
        std::cout << "循环依赖测试文件不存在，跳过此测试" << std::endl;
    }
    
    testFileA.close();
    testFileB.close();
}

void testPathEquivalence() {
    std::cout << "\n=== 路径等价性深度测试 ===" << std::endl;
    
    ImportPathNormalizer normalizer;
    
    // 测试不同表达方式的同一路径
    std::vector<std::vector<std::string>> equivalentGroups = {
        {"module/layout.chtl", "module/../module/layout.chtl", "./module/layout.chtl"},
        {"test/file.chtl", "test/./file.chtl", "test/subdir/../file.chtl"},
        {".", "./", "./././."}
    };
    
    for (size_t i = 0; i < equivalentGroups.size(); ++i) {
        std::cout << "等价组 " << (i + 1) << ":" << std::endl;
        const auto& group = equivalentGroups[i];
        
        for (size_t j = 0; j < group.size(); ++j) {
            for (size_t k = j + 1; k < group.size(); ++k) {
                bool areEquivalent = normalizer.arePathsEquivalent(group[j], group[k]);
                std::cout << "  " << group[j] << " ≡ " << group[k] << ": " 
                         << (areEquivalent ? "✅" : "❌") << std::endl;
            }
        }
        std::cout << std::endl;
    }
}

int main() {
    std::cout << "CHTL 增强Import系统测试" << std::endl;
    std::cout << "=========================" << std::endl;
    
    try {
        testPathNormalization();
        testCircularDependencyDetection();
        testDuplicateImportManagement();
        testEnhancedImportManager();
        testCircularDependencyScenario();
        testPathEquivalence();
        
        std::cout << "\n✅ 所有增强Import系统测试完成" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "❌ 测试异常: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}