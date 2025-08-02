#include "Origin.h"
#include <sstream>
#include <algorithm>
#include <regex>

namespace chtl {

// OriginNode实现
OriginNode::OriginNode(OriginType type, const NodePosition& position)
    : Node(NodeType::ORIGIN, position), originType_(type) {
}

OriginNode::OriginType OriginNode::getOriginType() const {
    return originType_;
}

void OriginNode::setOriginType(OriginType type) {
    originType_ = type;
}

const std::string& OriginNode::getContent() const {
    return content_;
}

void OriginNode::setContent(const std::string& content) {
    content_ = content;
}

bool OriginNode::hasContent() const {
    return !content_.empty();
}

void OriginNode::clearContent() {
    content_.clear();
}

void OriginNode::appendContent(const std::string& content) {
    content_ += content;
}

void OriginNode::prependContent(const std::string& content) {
    content_ = content + content_;
}

const std::string& OriginNode::getSourceFile() const {
    return sourceFile_;
}

void OriginNode::setSourceFile(const std::string& sourceFile) {
    sourceFile_ = sourceFile;
}

bool OriginNode::hasSourceFile() const {
    return !sourceFile_.empty();
}

size_t OriginNode::getContentLength() const {
    return content_.length();
}

size_t OriginNode::getLineCount() const {
    if (content_.empty()) {
        return 0;
    }
    
    size_t count = 1;
    for (char c : content_) {
        if (c == '\n') {
            count++;
        }
    }
    return count;
}

std::vector<std::string> OriginNode::getContentLines() const {
    std::vector<std::string> lines;
    std::stringstream ss(content_);
    std::string line;
    
    while (std::getline(ss, line)) {
        lines.push_back(line);
    }
    
    return lines;
}

void OriginNode::setContentLines(const std::vector<std::string>& lines) {
    std::stringstream ss;
    for (size_t i = 0; i < lines.size(); ++i) {
        if (i > 0) ss << "\n";
        ss << lines[i];
    }
    content_ = ss.str();
}

std::string OriginNode::getProcessedContent() const {
    // 基本实现，子类可以重写以提供特定的处理
    return content_;
}

bool OriginNode::validate() const {
    if (!Node::validate()) {
        return false;
    }
    
    // Origin节点必须有内容
    if (content_.empty()) {
        return false;
    }
    
    return true;
}

std::string OriginNode::toString() const {
    std::stringstream ss;
    ss << "[Origin] @" << originTypeToString();
    
    if (!sourceFile_.empty()) {
        ss << " from " << sourceFile_;
    }
    
    if (!content_.empty()) {
        ss << " {" << std::endl;
        ss << content_ << std::endl;
        ss << "}";
    }
    
    return ss.str();
}

std::string OriginNode::toDebugString(int indent) const {
    std::string indentStr(indent, ' ');
    std::stringstream ss;
    ss << indentStr << "OriginNode {" << std::endl;
    ss << indentStr << "  type: " << originTypeToString() << std::endl;
    ss << indentStr << "  contentLength: " << getContentLength() << std::endl;
    ss << indentStr << "  lineCount: " << getLineCount() << std::endl;
    
    if (!sourceFile_.empty()) {
        ss << indentStr << "  sourceFile: " << sourceFile_ << std::endl;
    }
    
    if (!content_.empty() && content_.length() <= 100) {
        ss << indentStr << "  content: " << content_ << std::endl;
    } else if (!content_.empty()) {
        ss << indentStr << "  content: [" << content_.length() << " characters]" << std::endl;
    }
    
    ss << indentStr << "  position: " << getPosition().toString() << std::endl;
    ss << indentStr << "}";
    return ss.str();
}

std::shared_ptr<Node> OriginNode::clone() const {
    auto cloned = std::make_shared<OriginNode>(originType_, getPosition());
    cloned->copyBaseProperties(*this);
    cloned->content_ = content_;
    cloned->sourceFile_ = sourceFile_;
    return cloned;
}

void OriginNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

std::string OriginNode::originTypeToString() const {
    switch (originType_) {
        case OriginType::HTML:       return "Html";
        case OriginType::CSS:        return "Css";
        case OriginType::JAVASCRIPT: return "JavaScript";
        default:                     return "Unknown";
    }
}

// OriginHtmlNode实现
OriginHtmlNode::OriginHtmlNode(const NodePosition& position)
    : OriginNode(OriginType::HTML, position) {
    setNodeType(NodeType::ORIGIN_HTML);
}

void OriginHtmlNode::addAttribute(const std::string& name, const std::string& value) {
    attributes_[name] = value;
}

void OriginHtmlNode::removeAttribute(const std::string& name) {
    attributes_.erase(name);
}

std::string OriginHtmlNode::getAttribute(const std::string& name) const {
    auto it = attributes_.find(name);
    return it != attributes_.end() ? it->second : "";
}

bool OriginHtmlNode::hasAttribute(const std::string& name) const {
    return attributes_.find(name) != attributes_.end();
}

const std::unordered_map<std::string, std::string>& OriginHtmlNode::getAllAttributes() const {
    return attributes_;
}

void OriginHtmlNode::clearAttributes() {
    attributes_.clear();
}

void OriginHtmlNode::setDoctype(const std::string& doctype) {
    doctype_ = doctype;
}

std::string OriginHtmlNode::getDoctype() const {
    return doctype_;
}

bool OriginHtmlNode::hasDoctype() const {
    return !doctype_.empty();
}

std::string OriginHtmlNode::getProcessedContent() const {
    std::stringstream ss;
    
    // 添加doctype
    if (!doctype_.empty()) {
        ss << "<!DOCTYPE " << doctype_ << ">" << std::endl;
    }
    
    // 添加原始内容
    ss << getContent();
    
    return ss.str();
}

bool OriginHtmlNode::isValidHtml() const {
    std::string content = getContent();
    if (content.empty()) {
        return false;
    }
    
    // 简单的HTML验证：检查是否有基本的HTML结构
    std::regex htmlRegex(R"(<html.*?>.*</html>)", std::regex::icase | std::regex::dotall);
    return std::regex_search(content, htmlRegex);
}

std::vector<std::string> OriginHtmlNode::extractTags() const {
    std::vector<std::string> tags;
    std::string content = getContent();
    
    std::regex tagRegex(R"(<(\w+)(?:\s[^>]*)?>)");
    std::sregex_iterator iter(content.begin(), content.end(), tagRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        const std::smatch& match = *iter;
        if (match.size() >= 2) {
            std::string tag = match[1].str();
            if (std::find(tags.begin(), tags.end(), tag) == tags.end()) {
                tags.push_back(tag);
            }
        }
    }
    
    return tags;
}

bool OriginHtmlNode::validate() const {
    if (!OriginNode::validate()) {
        return false;
    }
    
    // HTML内容的基本验证
    return isValidHtml();
}

std::string OriginHtmlNode::toString() const {
    std::stringstream ss;
    ss << "[Origin] @Html";
    
    if (!doctype_.empty()) {
        ss << " doctype=\"" << doctype_ << "\"";
    }
    
    if (!attributes_.empty()) {
        ss << " {";
        for (const auto& pair : attributes_) {
            ss << " " << pair.first << "=\"" << pair.second << "\"";
        }
        ss << " }";
    }
    
    if (!getContent().empty()) {
        ss << " {" << std::endl;
        ss << getProcessedContent() << std::endl;
        ss << "}";
    }
    
    return ss.str();
}

std::string OriginHtmlNode::toDebugString(int indent) const {
    std::string indentStr(indent, ' ');
    std::stringstream ss;
    ss << indentStr << "OriginHtmlNode {" << std::endl;
    ss << indentStr << "  contentLength: " << getContentLength() << std::endl;
    ss << indentStr << "  lineCount: " << getLineCount() << std::endl;
    ss << indentStr << "  isValidHtml: " << (isValidHtml() ? "true" : "false") << std::endl;
    
    if (!doctype_.empty()) {
        ss << indentStr << "  doctype: " << doctype_ << std::endl;
    }
    
    if (!attributes_.empty()) {
        ss << indentStr << "  attributes: {" << std::endl;
        for (const auto& pair : attributes_) {
            ss << indentStr << "    " << pair.first << ": " << pair.second << std::endl;
        }
        ss << indentStr << "  }" << std::endl;
    }
    
    auto tags = extractTags();
    if (!tags.empty()) {
        ss << indentStr << "  tags: [";
        for (size_t i = 0; i < tags.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << tags[i];
        }
        ss << "]" << std::endl;
    }
    
    ss << indentStr << "  position: " << getPosition().toString() << std::endl;
    ss << indentStr << "}";
    return ss.str();
}

std::shared_ptr<Node> OriginHtmlNode::clone() const {
    auto cloned = std::make_shared<OriginHtmlNode>(getPosition());
    cloned->copyBaseProperties(*this);
    cloned->setContent(getContent());
    cloned->setSourceFile(getSourceFile());
    cloned->doctype_ = doctype_;
    cloned->attributes_ = attributes_;
    return cloned;
}

// OriginCssNode实现
OriginCssNode::OriginCssNode(const NodePosition& position)
    : OriginNode(OriginType::CSS, position) {
    setNodeType(NodeType::ORIGIN_CSS);
}

void OriginCssNode::setMedia(const std::string& media) {
    media_ = media;
}

std::string OriginCssNode::getMedia() const {
    return media_;
}

bool OriginCssNode::hasMedia() const {
    return !media_.empty();
}

void OriginCssNode::setScoped(bool scoped) {
    isScoped_ = scoped;
}

bool OriginCssNode::isScoped() const {
    return isScoped_;
}

void OriginCssNode::addImport(const std::string& importUrl) {
    if (std::find(imports_.begin(), imports_.end(), importUrl) == imports_.end()) {
        imports_.push_back(importUrl);
    }
}

void OriginCssNode::removeImport(const std::string& importUrl) {
    imports_.erase(std::remove(imports_.begin(), imports_.end(), importUrl), imports_.end());
}

const std::vector<std::string>& OriginCssNode::getImports() const {
    return imports_;
}

void OriginCssNode::clearImports() {
    imports_.clear();
}

bool OriginCssNode::hasImport(const std::string& importUrl) const {
    return std::find(imports_.begin(), imports_.end(), importUrl) != imports_.end();
}

std::string OriginCssNode::getProcessedContent() const {
    std::stringstream ss;
    
    // 添加imports
    for (const auto& import : imports_) {
        ss << "@import url('" << import << "');" << std::endl;
    }
    
    if (!imports_.empty()) {
        ss << std::endl;
    }
    
    // 添加原始内容
    std::string content = getContent();
    if (isScoped_ && !content.empty()) {
        // 简单的scoped处理：为每个选择器添加作用域前缀
        // 这里是简化实现，实际应该有更复杂的CSS解析
        ss << "/* Scoped CSS */" << std::endl;
        ss << content;
    } else {
        ss << content;
    }
    
    return ss.str();
}

bool OriginCssNode::isValidCss() const {
    std::string content = getContent();
    if (content.empty()) {
        return true; // 空内容认为是有效的
    }
    
    // 简单的CSS验证：检查是否有基本的CSS语法
    std::regex cssRegex(R"([^{}]+\{[^{}]*\})");
    return std::regex_search(content, cssRegex);
}

std::vector<std::string> OriginCssNode::extractSelectors() const {
    std::vector<std::string> selectors;
    std::string content = getContent();
    
    std::regex selectorRegex(R"(([^{}]+)\{)");
    std::sregex_iterator iter(content.begin(), content.end(), selectorRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        const std::smatch& match = *iter;
        if (match.size() >= 2) {
            std::string selector = match[1].str();
            // 清理空白
            selector.erase(0, selector.find_first_not_of(" \t\n\r"));
            selector.erase(selector.find_last_not_of(" \t\n\r") + 1);
            
            if (!selector.empty() && std::find(selectors.begin(), selectors.end(), selector) == selectors.end()) {
                selectors.push_back(selector);
            }
        }
    }
    
    return selectors;
}

bool OriginCssNode::validate() const {
    if (!OriginNode::validate()) {
        return false;
    }
    
    // CSS内容的基本验证
    return isValidCss();
}

std::string OriginCssNode::toString() const {
    std::stringstream ss;
    ss << "[Origin] @Css";
    
    if (!media_.empty()) {
        ss << " media=\"" << media_ << "\"";
    }
    
    if (isScoped_) {
        ss << " scoped";
    }
    
    if (!imports_.empty()) {
        ss << " imports=[";
        for (size_t i = 0; i < imports_.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << imports_[i];
        }
        ss << "]";
    }
    
    if (!getContent().empty()) {
        ss << " {" << std::endl;
        ss << getProcessedContent() << std::endl;
        ss << "}";
    }
    
    return ss.str();
}

std::string OriginCssNode::toDebugString(int indent) const {
    std::string indentStr(indent, ' ');
    std::stringstream ss;
    ss << indentStr << "OriginCssNode {" << std::endl;
    ss << indentStr << "  contentLength: " << getContentLength() << std::endl;
    ss << indentStr << "  lineCount: " << getLineCount() << std::endl;
    ss << indentStr << "  isValidCss: " << (isValidCss() ? "true" : "false") << std::endl;
    ss << indentStr << "  isScoped: " << (isScoped_ ? "true" : "false") << std::endl;
    
    if (!media_.empty()) {
        ss << indentStr << "  media: " << media_ << std::endl;
    }
    
    if (!imports_.empty()) {
        ss << indentStr << "  imports: [";
        for (size_t i = 0; i < imports_.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << imports_[i];
        }
        ss << "]" << std::endl;
    }
    
    auto selectors = extractSelectors();
    if (!selectors.empty()) {
        ss << indentStr << "  selectors: [";
        for (size_t i = 0; i < std::min(size_t(5), selectors.size()); ++i) {
            if (i > 0) ss << ", ";
            ss << selectors[i];
        }
        if (selectors.size() > 5) {
            ss << ", ...(" << (selectors.size() - 5) << " more)";
        }
        ss << "]" << std::endl;
    }
    
    ss << indentStr << "  position: " << getPosition().toString() << std::endl;
    ss << indentStr << "}";
    return ss.str();
}

std::shared_ptr<Node> OriginCssNode::clone() const {
    auto cloned = std::make_shared<OriginCssNode>(getPosition());
    cloned->copyBaseProperties(*this);
    cloned->setContent(getContent());
    cloned->setSourceFile(getSourceFile());
    cloned->media_ = media_;
    cloned->isScoped_ = isScoped_;
    cloned->imports_ = imports_;
    return cloned;
}

// OriginJavaScriptNode实现
OriginJavaScriptNode::OriginJavaScriptNode(const NodePosition& position)
    : OriginNode(OriginType::JAVASCRIPT, position) {
    setNodeType(NodeType::ORIGIN_JAVASCRIPT);
}

void OriginJavaScriptNode::setModule(bool isModule) {
    isModule_ = isModule;
}

bool OriginJavaScriptNode::isModule() const {
    return isModule_;
}

void OriginJavaScriptNode::setAsync(bool async) {
    isAsync_ = async;
}

bool OriginJavaScriptNode::isAsync() const {
    return isAsync_;
}

void OriginJavaScriptNode::setDefer(bool defer) {
    isDefer_ = defer;
}

bool OriginJavaScriptNode::isDefer() const {
    return isDefer_;
}

void OriginJavaScriptNode::addDependency(const std::string& dependency) {
    if (std::find(dependencies_.begin(), dependencies_.end(), dependency) == dependencies_.end()) {
        dependencies_.push_back(dependency);
    }
}

void OriginJavaScriptNode::removeDependency(const std::string& dependency) {
    dependencies_.erase(std::remove(dependencies_.begin(), dependencies_.end(), dependency), dependencies_.end());
}

const std::vector<std::string>& OriginJavaScriptNode::getDependencies() const {
    return dependencies_;
}

void OriginJavaScriptNode::clearDependencies() {
    dependencies_.clear();
}

bool OriginJavaScriptNode::hasDependency(const std::string& dependency) const {
    return std::find(dependencies_.begin(), dependencies_.end(), dependency) != dependencies_.end();
}

std::string OriginJavaScriptNode::getProcessedContent() const {
    std::stringstream ss;
    
    // 添加依赖
    for (const auto& dep : dependencies_) {
        if (isModule_) {
            ss << "import '" << dep << "';" << std::endl;
        } else {
            ss << "// Dependency: " << dep << std::endl;
        }
    }
    
    if (!dependencies_.empty()) {
        ss << std::endl;
    }
    
    // 添加原始内容
    std::string content = getContent();
    if (isModule_ && !content.empty()) {
        ss << "// ES6 Module" << std::endl;
    }
    
    ss << content;
    
    return ss.str();
}

bool OriginJavaScriptNode::isValidJavaScript() const {
    std::string content = getContent();
    if (content.empty()) {
        return true; // 空内容认为是有效的
    }
    
    // 简单的JavaScript验证：检查是否有基本的语法错误
    // 这里只是一个非常基础的检查，实际应该用专门的JS解析器
    size_t openBraces = 0;
    size_t closeBraces = 0;
    
    for (char c : content) {
        if (c == '{') openBraces++;
        else if (c == '}') closeBraces++;
    }
    
    return openBraces == closeBraces;
}

std::vector<std::string> OriginJavaScriptNode::extractFunctions() const {
    std::vector<std::string> functions;
    std::string content = getContent();
    
    std::regex functionRegex(R"(function\s+(\w+)\s*\()");
    std::sregex_iterator iter(content.begin(), content.end(), functionRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        const std::smatch& match = *iter;
        if (match.size() >= 2) {
            std::string funcName = match[1].str();
            if (std::find(functions.begin(), functions.end(), funcName) == functions.end()) {
                functions.push_back(funcName);
            }
        }
    }
    
    return functions;
}

bool OriginJavaScriptNode::validate() const {
    if (!OriginNode::validate()) {
        return false;
    }
    
    // JavaScript内容的基本验证
    return isValidJavaScript();
}

std::string OriginJavaScriptNode::toString() const {
    std::stringstream ss;
    ss << "[Origin] @JavaScript";
    
    if (isModule_) {
        ss << " module";
    }
    
    if (isAsync_) {
        ss << " async";
    }
    
    if (isDefer_) {
        ss << " defer";
    }
    
    if (!dependencies_.empty()) {
        ss << " deps=[";
        for (size_t i = 0; i < dependencies_.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << dependencies_[i];
        }
        ss << "]";
    }
    
    if (!getContent().empty()) {
        ss << " {" << std::endl;
        ss << getProcessedContent() << std::endl;
        ss << "}";
    }
    
    return ss.str();
}

std::string OriginJavaScriptNode::toDebugString(int indent) const {
    std::string indentStr(indent, ' ');
    std::stringstream ss;
    ss << indentStr << "OriginJavaScriptNode {" << std::endl;
    ss << indentStr << "  contentLength: " << getContentLength() << std::endl;
    ss << indentStr << "  lineCount: " << getLineCount() << std::endl;
    ss << indentStr << "  isValidJavaScript: " << (isValidJavaScript() ? "true" : "false") << std::endl;
    ss << indentStr << "  isModule: " << (isModule_ ? "true" : "false") << std::endl;
    ss << indentStr << "  isAsync: " << (isAsync_ ? "true" : "false") << std::endl;
    ss << indentStr << "  isDefer: " << (isDefer_ ? "true" : "false") << std::endl;
    
    if (!dependencies_.empty()) {
        ss << indentStr << "  dependencies: [";
        for (size_t i = 0; i < dependencies_.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << dependencies_[i];
        }
        ss << "]" << std::endl;
    }
    
    auto functions = extractFunctions();
    if (!functions.empty()) {
        ss << indentStr << "  functions: [";
        for (size_t i = 0; i < std::min(size_t(5), functions.size()); ++i) {
            if (i > 0) ss << ", ";
            ss << functions[i];
        }
        if (functions.size() > 5) {
            ss << ", ...(" << (functions.size() - 5) << " more)";
        }
        ss << "]" << std::endl;
    }
    
    ss << indentStr << "  position: " << getPosition().toString() << std::endl;
    ss << indentStr << "}";
    return ss.str();
}

std::shared_ptr<Node> OriginJavaScriptNode::clone() const {
    auto cloned = std::make_shared<OriginJavaScriptNode>(getPosition());
    cloned->copyBaseProperties(*this);
    cloned->setContent(getContent());
    cloned->setSourceFile(getSourceFile());
    cloned->isModule_ = isModule_;
    cloned->isAsync_ = isAsync_;
    cloned->isDefer_ = isDefer_;
    cloned->dependencies_ = dependencies_;
    return cloned;
}

// Manager和Factory实现
OriginManager::OriginManager() {
}

void OriginManager::addOriginNode(std::shared_ptr<OriginNode> node) {
    if (node) {
        originNodes_.push_back(node);
    }
}

void OriginManager::removeOriginNode(std::shared_ptr<OriginNode> node) {
    auto it = std::find(originNodes_.begin(), originNodes_.end(), node);
    if (it != originNodes_.end()) {
        originNodes_.erase(it);
    }
}

const std::vector<std::shared_ptr<OriginNode>>& OriginManager::getAllOriginNodes() const {
    return originNodes_;
}

std::vector<std::shared_ptr<OriginNode>> OriginManager::getOriginNodesByType(OriginNode::OriginType type) const {
    std::vector<std::shared_ptr<OriginNode>> result;
    
    for (const auto& node : originNodes_) {
        if (node->getOriginType() == type) {
            result.push_back(node);
        }
    }
    
    return result;
}

void OriginManager::clearOriginNodes() {
    originNodes_.clear();
}

size_t OriginManager::getOriginNodeCount() const {
    return originNodes_.size();
}

std::string OriginManager::generateCombinedContent(OriginNode::OriginType type) const {
    std::stringstream ss;
    
    auto nodes = getOriginNodesByType(type);
    for (const auto& node : nodes) {
        ss << node->getProcessedContent() << std::endl;
    }
    
    return ss.str();
}

OriginParser::OriginParser() {
}

std::shared_ptr<OriginNode> OriginParser::parseOriginBlock(const std::string& content, OriginNode::OriginType type) {
    switch (type) {
        case OriginNode::OriginType::HTML:
            return parseHtmlBlock(content);
        case OriginNode::OriginType::CSS:
            return parseCssBlock(content);
        case OriginNode::OriginType::JAVASCRIPT:
            return parseJavaScriptBlock(content);
        default:
            return nullptr;
    }
}

std::shared_ptr<OriginHtmlNode> OriginParser::parseHtmlBlock(const std::string& content) {
    auto node = std::make_shared<OriginHtmlNode>();
    node->setContent(content);
    
    // 检测doctype
    std::regex doctypeRegex(R"(<!DOCTYPE\s+([^>]+)>)", std::regex::icase);
    std::smatch match;
    if (std::regex_search(content, match, doctypeRegex)) {
        node->setDoctype(match[1].str());
    }
    
    return node;
}

std::shared_ptr<OriginCssNode> OriginParser::parseCssBlock(const std::string& content) {
    auto node = std::make_shared<OriginCssNode>();
    node->setContent(content);
    
    // 检测@import语句
    std::regex importRegex(R"(@import\s+url\(['"]([^'"]+)['"])\s*;)", std::regex::icase);
    std::sregex_iterator iter(content.begin(), content.end(), importRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        const std::smatch& match = *iter;
        if (match.size() >= 2) {
            node->addImport(match[1].str());
        }
    }
    
    return node;
}

std::shared_ptr<OriginJavaScriptNode> OriginParser::parseJavaScriptBlock(const std::string& content) {
    auto node = std::make_shared<OriginJavaScriptNode>();
    node->setContent(content);
    
    // 检测ES6模块语法
    std::regex moduleRegex(R"(import\s+.*from)", std::regex::icase);
    if (std::regex_search(content, moduleRegex)) {
        node->setModule(true);
    }
    
    return node;
}

OriginFactory::OriginFactory() {
}

std::shared_ptr<OriginNode> OriginFactory::createOriginNode(OriginNode::OriginType type) {
    switch (type) {
        case OriginNode::OriginType::HTML:
            return std::make_shared<OriginHtmlNode>();
        case OriginNode::OriginType::CSS:
            return std::make_shared<OriginCssNode>();
        case OriginNode::OriginType::JAVASCRIPT:
            return std::make_shared<OriginJavaScriptNode>();
        default:
            return nullptr;
    }
}

std::shared_ptr<OriginHtmlNode> OriginFactory::createHtmlNode(const std::string& content) {
    auto node = std::make_shared<OriginHtmlNode>();
    node->setContent(content);
    return node;
}

std::shared_ptr<OriginCssNode> OriginFactory::createCssNode(const std::string& content) {
    auto node = std::make_shared<OriginCssNode>();
    node->setContent(content);
    return node;
}

std::shared_ptr<OriginJavaScriptNode> OriginFactory::createJavaScriptNode(const std::string& content) {
    auto node = std::make_shared<OriginJavaScriptNode>();
    node->setContent(content);
    return node;
}

} // namespace chtl