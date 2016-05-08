#include "ParseTree.h"

const std::string& SyntaxNode::toString(const SyntaxNode& syn)
{
    return syn.getSymbol();
}

SyntaxNode::SyntaxNode(const NodeType& type, const std::string& symbol, const bool isLeaf) :
    _type(type), _next(0), _symbol(symbol), _isLeaf(isLeaf) { }

void SyntaxNode::setNext(SyntaxNode* next)
{
    _next = next;
}

bool SyntaxNode::isRoot() const
{
    if (_type == SyntaxNode::ROOT)
    {
        return true;
    }
    return false;
}

const std::string& SyntaxNode::getSymbol() const
{
    return _symbol;
}

ParseTree::ParseTree(const std::string& symbol) : _root(SyntaxNode::ROOT, symbol, false)
{

}

const SyntaxNode& ParseTree::getRoot()
{
    return _root;
}