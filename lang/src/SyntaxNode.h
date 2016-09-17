#ifndef __SyntaxNode_

#include <string>
#include <memory>
#include <set>
#include <stack>

class SyntaxNode
{
public:
    enum NodeType { LEAF, ALTER, CONCAT };
protected:
    std::string _symbol;
    std::unique_ptr<SyntaxNode> _left;
    std::unique_ptr<SyntaxNode> _right;
    NodeType _type;
    bool _repeat;
protected:
    static SyntaxNode reduce_node(std::stack<SyntaxNode>& stack, SyntaxNode::NodeType type);
    static void add_node(std::stack<SyntaxNode>& stack, SyntaxNode& rhs, SyntaxNode::NodeType type);
    void setLeft(SyntaxNode& next);
    void setRight(SyntaxNode& next);
    void setType(NodeType type) { _type = type; };
    void setRepeat(bool repeat) { _repeat = repeat; }
public:
    static SyntaxNode tokenize(const std::string& token, const std::string& equality);
    SyntaxNode(NodeType type);
    SyntaxNode(const std::string& symbol);
    SyntaxNode(SyntaxNode&& node) = default;
    SyntaxNode& operator=(SyntaxNode&& node) = default;
    NodeType getType() const { return _type; };
    SyntaxNode const* getLeft() const { return _left.get(); }
    SyntaxNode const* getRight() const { return _right.get(); }
    bool getRepeat() const { return _repeat; }
    const std::string& getSymbol() const { return _symbol; };
    void toSet(std::set<std::string>& out) const;
};

#endif
