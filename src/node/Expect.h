#ifndef CHTL_EXPECT_H
#define CHTL_EXPECT_H

#include "Node.h"
#include <string>
#include <vector>

namespace chtl {

// 期盼节点 - expect / not expect
class ExpectNode : public Node {
public:
    enum ExpectType {
        EXPECT_PRECISE,     // 精准期盼 - expect span, div, [Custom] @Element XXX
        EXPECT_TYPE,        // 类型期盼 - expect [Custom]
        EXPECT_NEGATIVE     // 否定期盼 - not expect [Template]
    };
    
private:
    ExpectType expectType;
    std::vector<std::string> expectedItems;  // 期盼的具体项目
    bool isNot;  // 是否是 not expect
    
public:
    ExpectNode(ExpectType type, bool notExpect = false, int line = 0, int col = 0)
        : Node(NodeType::EXPECT_NODE, line, col), 
          expectType(type), isNot(notExpect) {}
    
    ExpectType getExpectType() const { return expectType; }
    bool getIsNot() const { return isNot; }
    
    void setExpectType(ExpectType type) { expectType = type; }
    void setIsNot(bool value) { isNot = value; }
    
    void addExpectedItem(const std::string& item) {
        expectedItems.push_back(item);
    }
    
    const std::vector<std::string>& getExpectedItems() const {
        return expectedItems;
    }
    
    void clearExpectedItems() {
        expectedItems.clear();
    }
    
    std::string toString() const override {
        std::string result = isNot ? "NotExpectNode" : "ExpectNode";
        
        switch (expectType) {
            case EXPECT_PRECISE:
                result += " (Precise)";
                break;
            case EXPECT_TYPE:
                result += " (Type)";
                break;
            case EXPECT_NEGATIVE:
                result += " (Negative)";
                break;
        }
        
        if (!expectedItems.empty()) {
            result += ": ";
            for (size_t i = 0; i < expectedItems.size(); ++i) {
                if (i > 0) result += ", ";
                result += expectedItems[i];
            }
        }
        
        return result;
    }
};

} // namespace chtl

#endif // CHTL_EXPECT_H