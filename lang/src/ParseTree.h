#ifndef __PARSETREE_

#include <string>

class SyntaxNode
{
public:
    enum NodeType { ROOT = 0, ALTER, CONCATE, EQUALS };
    static const std::string& toString(const SyntaxNode& s);
private:
    NodeType _type;
    std::string _symbol;
    SyntaxNode* _next;
    bool _isLeaf;
public:
    SyntaxNode(const NodeType& type, const std::string& symbol, const bool leaf);
    void setNext(SyntaxNode * const next);
    bool isRoot() const;
    const std::string& getSymbol() const;
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
