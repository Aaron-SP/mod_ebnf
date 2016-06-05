#ifndef __SyntaxNode_

#include <string>
#include <memory>
#include <vector>
#include <stack>

class SyntaxNode
{
public:
    enum NodeType {LEAF, ALTER, CONCAT, COMBINE};
private:
    std::string _symbol;
    std::unique_ptr<SyntaxNode> _left;
    std::unique_ptr<SyntaxNode> _right;
    NodeType _type;
public:
    static SyntaxNode reduce_node(std::stack<SyntaxNode>& stack, SyntaxNode::NodeType type);
    static void add_node(std::stack<SyntaxNode>& stack, SyntaxNode& rhs, SyntaxNode::NodeType type);
    static void flush_stack(std::stack<SyntaxNode>& stack);
    static SyntaxNode tokenize(const std::string& token, const std::string& equality);
    SyntaxNode(NodeType type);
    SyntaxNode(const std::string& symbol);
    SyntaxNode(SyntaxNode&& node) = default;
    SyntaxNode& operator=(SyntaxNode&& node) = default;
    NodeType getType() const;
    void setType(NodeType type);
    void setLeft(SyntaxNode& next);
    void setRight(SyntaxNode& next);
    const std::string& getSymbol() const;
    std::vector<std::string> toVector() const;
    std::string print() const;
    bool matches(char ch);
    bool matches(const std::string& match);
};

#endif
