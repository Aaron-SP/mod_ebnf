#ifndef __PARSETREE_

#include <string>
#include <memory>
#include <vector>

class SyntaxNode
{
public:
    enum NodeType { LEAF = 0, EQUALS, ALTER, CONCATE };
private:
    NodeType _type;
    std::string _symbol;
    std::unique_ptr<SyntaxNode> _left;
    std::unique_ptr<SyntaxNode> _right;
public:
    SyntaxNode(char ch);
    SyntaxNode(const std::string& symbol, char ch);
    SyntaxNode(SyntaxNode&& node) = default;
    SyntaxNode& operator=(SyntaxNode&& node) = default;
    void setLeft(SyntaxNode& next);
    void setRight(SyntaxNode& next);
    bool isLeaf() const;
    const NodeType& getType()  const;
    const bool sameType(const SyntaxNode& node) const;
    NodeType highestType(const SyntaxNode& node) const;
    const std::string& getSymbol() const;
    std::vector<std::string> toVector() const;
    std::string print() const;
};

class ParseTree
{
private:
    SyntaxNode _root;
public:
    ParseTree(const std::string& symbol);
    const SyntaxNode& getRoot();
};

#endif
