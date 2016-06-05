#ifndef __SyntaxNode_

#include <string>
#include <memory>
#include <vector>
#include <stack>

class SyntaxNode
{
public:
    enum NodeType { LEAF, ALTER, CONCAT };
private:
    std::string _symbol;
    std::unique_ptr<SyntaxNode> _left;
    std::unique_ptr<SyntaxNode> _right;
    NodeType _type;
    bool _repeat;
public:
    static SyntaxNode reduce_node(std::stack<SyntaxNode>& stack, SyntaxNode::NodeType type);
    static void add_node(std::stack<SyntaxNode>& stack, SyntaxNode& rhs, SyntaxNode::NodeType type);
    static SyntaxNode tokenize(const std::string& token, const std::string& equality);
    SyntaxNode(NodeType type);
    SyntaxNode(const std::string& symbol);
    SyntaxNode(SyntaxNode&& node) = default;
    SyntaxNode& operator=(SyntaxNode&& node) = default;
    NodeType getType() const { return _type; };
    void setType(NodeType type) { _type = type; };
    SyntaxNode* getLeft() const { return _left.get(); }
    SyntaxNode* getRight() const { return _right.get(); }
    void setLeft(SyntaxNode& next);
    void setRight(SyntaxNode& next);
    bool getRepeat() const { return _repeat; }
    void setRepeat(bool repeat);
    const std::string& getSymbol() const { return _symbol; };
    std::vector<std::string> toVector() const;
    std::string print() const;
};

#endif
