#ifndef CHTL_COMMENT_H
#define CHTL_COMMENT_H

#include "Node.h"
#include <string>

namespace chtl {

// 注释节点
class CommentNode : public Node {
public:
    enum CommentType {
        COMMENT_SINGLE_LINE,    // //
        COMMENT_MULTI_LINE,     // /* */
        COMMENT_GENERATOR       // --
    };
    
private:
    CommentType commentType;
    std::string content;
    
public:
    CommentNode(CommentType type, const std::string& text, 
                int line = 0, int col = 0)
        : Node(NodeType::COMMENT_NODE, line, col), 
          commentType(type), content(text) {}
    
    CommentType getCommentType() const { return commentType; }
    const std::string& getContent() const { return content; }
    
    void setCommentType(CommentType type) { commentType = type; }
    void setContent(const std::string& text) { content = text; }
    
    std::string toString() const override {
        std::string typeStr;
        switch (commentType) {
            case COMMENT_SINGLE_LINE: typeStr = "//"; break;
            case COMMENT_MULTI_LINE: typeStr = "/**/"; break;
            case COMMENT_GENERATOR: typeStr = "--"; break;
        }
        return "CommentNode(" + typeStr + "): " + content;
    }
};

} // namespace chtl

#endif // CHTL_COMMENT_H