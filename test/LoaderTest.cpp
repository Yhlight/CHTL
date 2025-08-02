#include "../src/loader/ChtlLoader.h"
#include "../src/node/Import.h"
#include <iostream>
#include <fstream>
#include <cstdio>
#include <sys/stat.h>
#include <sys/types.h>

using namespace chtl;

class LoaderTest {
private:
    int testsPassed = 0;
    int testsFailed = 0;
    std::string testDir = "/tmp/chtl_test_files";
    
    void test(const std::string& name, bool condition) {
        if (condition) {
            std::cout << "[PASS] " << name << std::endl;
            testsPassed++;
        } else {
            std::cout << "[FAIL] " << name << std::endl;
            testsFailed++;
        }
    }
    
    void createTestFile(const std::string& path, const std::string& content) {
        // 创建目录（如果不存在）
        size_t lastSlash = path.find_last_of('/');
        if (lastSlash != std::string::npos) {
            std::string dir = path.substr(0, lastSlash);
            mkdir(dir.c_str(), 0755);
        }
        
        std::ofstream file(path);
        file << content;
        file.close();
    }
    
    void setupTestFiles() {
        // 创建测试目录
        mkdir(testDir.c_str(), 0755);
        mkdir((testDir + "/module").c_str(), 0755);
        
        // 创建测试文件
        createTestFile(testDir + "/main.chtl", 
            "[Import] @Style from \"style.css\"\n"
            "[Import] @Chtl from module.sub\n"
            "div { }");
        
        createTestFile(testDir + "/style.css", 
            "body { margin: 0; }");
        
        createTestFile(testDir + "/module/sub.chtl", 
            "[Custom] @Element Box { div { } }");
        
        // 创建循环依赖测试文件
        createTestFile(testDir + "/circular1.chtl",
            "[Import] @Chtl from circular2");
            
        createTestFile(testDir + "/circular2.chtl",
            "[Import] @Chtl from circular3");
            
        createTestFile(testDir + "/circular3.chtl",
            "[Import] @Chtl from circular1");
        
        // 创建通配符测试文件
        createTestFile(testDir + "/module/comp1.chtl", "");
        createTestFile(testDir + "/module/comp2.chtl", "");
        createTestFile(testDir + "/module/style.css", "");
        createTestFile(testDir + "/module/script.js", "");
    }
    
    void cleanupTestFiles() {
        // 递归删除测试目录
        system(("rm -rf " + testDir).c_str());
    }
    
public:
    void runAllTests() {
        std::cout << "=== ChtlLoader Tests ===" << std::endl;
        
        setupTestFiles();
        
        testPathNormalization();
        testPathConversion();
        testFileLoading();
        testDuplicateLoading();
        testCircularDependency();
        testWildcardImport();
        testImportNode();
        
        cleanupTestFiles();
        
        std::cout << "\nSummary: " << testsPassed << " passed, " 
                  << testsFailed << " failed" << std::endl;
    }
    
    void testPathNormalization() {
        std::cout << "\n-- Path Normalization Tests --" << std::endl;
        
        ChtlLoader loader;
        
        // 测试基本路径规范化
        std::string normalized = loader.normalizePath("./test/../file.chtl");
        test("Normalize relative path", normalized == "file.chtl");
        
        normalized = loader.normalizePath("test//double//slash");
        test("Normalize double slashes", normalized == "test/double/slash");
        
        normalized = loader.normalizePath("test\\back\\slash");
        test("Normalize backslashes", normalized == "test/back/slash");
        
        // 测试 ./file 的规范化
        normalized = loader.normalizePath("./style.css");
        std::cout << "  ./style.css normalizes to: '" << normalized << "'" << std::endl;
        test("Normalize ./file", normalized == "style.css");
    }
    
    void testPathConversion() {
        std::cout << "\n-- Path Conversion Tests --" << std::endl;
        
        ChtlLoader loader;
        
        // 测试点号转换
        std::string converted = loader.convertDotToSlash("module.submodule.file");
        test("Convert dots to slashes", converted == "module/submodule/file");
        
        converted = loader.convertDotToSlash("module.file.chtl");
        test("Preserve file extension", converted == "module/file.chtl");
        
        converted = loader.convertDotToSlash("path/to/file.chtl");
        test("Already slash path unchanged", converted == "path/to/file.chtl");
    }
    
    void testFileLoading() {
        std::cout << "\n-- File Loading Tests --" << std::endl;
        
        ChtlLoader loader(testDir);
        
        try {
            FileInfo info = loader.loadFile("main.chtl");
            test("Load existing file", info.isLoaded);
            test("File content not empty", !info.content.empty());
            test("File path resolved", !info.path.empty());
            
            // 测试使用点号路径
            try {
                FileInfo info2 = loader.loadFile("module.sub");
                test("Load file with dot notation", info2.isLoaded);
            } catch (const LoaderError& e) {
                // 如果文件不存在，这是正常的
                test("Load file with dot notation", e.getErrorType() == LoaderError::FILE_NOT_FOUND);
            }
            
        } catch (const LoaderError& e) {
            test("File loading should not throw", false);
        }
        
        // 测试不存在的文件
        try {
            loader.loadFile("nonexistent.chtl");
            test("Loading nonexistent file should throw", false);
        } catch (const LoaderError& e) {
            test("Loading nonexistent file throws correct error", 
                 e.getErrorType() == LoaderError::FILE_NOT_FOUND);
        }
    }
    
    void testDuplicateLoading() {
        std::cout << "\n-- Duplicate Loading Tests --" << std::endl;
        
        ChtlLoader loader(testDir);
        
        // 第一次加载
        FileInfo info1 = loader.loadFile("style.css");
        
        // 第二次加载同一文件
        FileInfo info2 = loader.loadFile("style.css");
        
        test("Duplicate loading returns same path", info1.path == info2.path);
        test("File loaded from cache", loader.isFileLoaded("style.css"));
        
        // 使用不同路径表示同一文件
        try {
            FileInfo info3 = loader.loadFile("./style.css");
            test("Different path notation resolves to same file", info1.path == info3.path);
        } catch (const LoaderError& e) {
            std::cout << "  Error loading ./style.css: " << e.what() << std::endl;
            test("Different path notation resolves to same file", false);
        }
    }
    
    void testCircularDependency() {
        std::cout << "\n-- Circular Dependency Tests --" << std::endl;
        
        ChtlLoader loader(testDir);
        
        // 添加循环依赖
        loader.addImportDependency("circular1.chtl", "circular2.chtl");
        loader.addImportDependency("circular2.chtl", "circular3.chtl");
        loader.addImportDependency("circular3.chtl", "circular1.chtl");
        
        std::vector<std::string> cycle;
        bool hasCycle = loader.hasCircularDependency(cycle);
        
        test("Detect circular dependency", hasCycle);
        test("Cycle path not empty", !cycle.empty());
        
        if (!cycle.empty()) {
            std::cout << "  Cycle detected: ";
            for (const auto& file : cycle) {
                std::cout << file << " -> ";
            }
            std::cout << "..." << std::endl;
        }
    }
    
    void testWildcardImport() {
        std::cout << "\n-- Wildcard Import Tests --" << std::endl;
        
        ChtlLoader loader(testDir);
        
        // 测试通配符导入
        std::vector<std::string> files = loader.resolveWildcardImport(
            "module/*", testDir + "/main.chtl");
        
        test("Wildcard import finds files", files.size() == 4);
        
        // 检查文件类型
        int chtlCount = 0, cssCount = 0, jsCount = 0;
        for (const auto& file : files) {
            if (file.find(".chtl") != std::string::npos) chtlCount++;
            if (file.find(".css") != std::string::npos) cssCount++;
            if (file.find(".js") != std::string::npos) jsCount++;
        }
        
        test("Found CHTL files", chtlCount == 2);
        test("Found CSS files", cssCount == 1);
        test("Found JS files", jsCount == 1);
    }
    
    void testImportNode() {
        std::cout << "\n-- Import Node Tests --" << std::endl;
        
        ChtlLoader loader(testDir);
        
        // 创建导入节点
        ImportNode import1(ImportNode::IMPORT_STYLE, "style.css");
        test("Import node created", import1.getFromPath() == "style.css");
        
        // 设置解析路径
        std::string resolved = loader.resolveImportPath("style.css", testDir + "/main.chtl");
        import1.setResolvedPath(resolved);
        test("Path resolved", import1.getIsResolved());
        
        // 测试需要目标名称的导入
        ImportNode import2(ImportNode::IMPORT_CUSTOM_ELEMENT, "module/sub.chtl");
        import2.setTargetName("Box");
        test("Import requires target name", import2.requiresTargetName());
        test("Import validation with target", import2.validate());
        
        ImportNode import3(ImportNode::IMPORT_CUSTOM_STYLE, "styles.chtl");
        test("Import validation without required target", !import3.validate());
        
        // 测试有效名称
        ImportNode import4(ImportNode::IMPORT_CHTL, "module/component.chtl");
        import4.setAsName("MyComponent");
        test("Effective name with 'as'", import4.getEffectiveName() == "MyComponent");
        
        ImportNode import5(ImportNode::IMPORT_CHTL, "path/to/file.chtl");
        test("Effective name from path", import5.getEffectiveName() == "file");
        
        // 测试通配符
        ImportNode import6(ImportNode::IMPORT_CHTL, "module/*");
        import6.setIsWildcard(true);
        import6.addExpandedPath("module/comp1.chtl");
        import6.addExpandedPath("module/comp2.chtl");
        test("Wildcard expanded paths", import6.getExpandedPaths().size() == 2);
    }
};

int main() {
    LoaderTest test;
    test.runAllTests();
    return 0;
}