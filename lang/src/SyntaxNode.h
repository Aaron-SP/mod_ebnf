#ifndef __PARSETREE_

#include <string>
#include <memory>
#include <vector>

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
};

#endif
