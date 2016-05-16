#include "ParseTree.h"
#include <algorithm>
#include <iterator>

SyntaxNode::SyntaxNode(char ch) : _type(SyntaxNode::LEAF)
{
    if (ch == ',')
    {
        _type = SyntaxNode::CONCATE;
    }
    else if (ch == '|')
    {
        _type = SyntaxNode::ALTER;
    }
}

SyntaxNode::SyntaxNode(const std::string& symbol, char ch) : _symbol(symbol), _type(SyntaxNode::LEAF)
{
    if (ch == ',')
    {
        _type = SyntaxNode::CONCATE;
    }
    else if (ch == '|')
    {
        _type = SyntaxNode::ALTER;
    }
}

void SyntaxNode::setLeft(SyntaxNode& left)
{
    _left.reset(new SyntaxNode(std::move(left)));
}

void SyntaxNode::setRight(SyntaxNode& right)
{
    _right.reset(new SyntaxNode(std::move(right)));
}

bool SyntaxNode::isLeaf() const
{
    return (_type == SyntaxNode::LEAF);
}

const std::string& SyntaxNode::getSymbol() const
{
    return _symbol;
}

const SyntaxNode::NodeType& SyntaxNode::getType()  const
{
    return _type;
}

const bool SyntaxNode::sameType(const SyntaxNode& node) const
{
    return (this->getType() == node.getType());
}

SyntaxNode::NodeType SyntaxNode::highestType(const SyntaxNode& node) const
{
    if (this->getType() >= node.getType())
    {
        return this->getType();
    }
    else
    {
        return node.getType();
    }
}

std::vector<std::string> SyntaxNode::toVector() const
{
    std::vector<std::string> out;
    bool left = _left.get() != nullptr;
    bool right = _right.get() != nullptr;
    if (left)
    {
        std::vector<std::string> rec = _left.get()->toVector();
        std::move(rec.begin(), rec.end(), std::back_inserter(out));
    }
    if (right)
    {
        std::vector<std::string> rec = _right.get()->toVector();
        std::move(rec.begin(), rec.end(), std::back_inserter(out));
    }
    if (!left && !right)
    {
        out.emplace_back(this->getSymbol());
    }
    return out;
}

std::string SyntaxNode::print() const
{
    std::string out;
    bool bleft = _left.get() != nullptr;
    bool bright = _right.get() != nullptr;
    SyntaxNode* left = _left.get();
    SyntaxNode* right = _right.get();
    if (bleft && bright)
    {
        if (left->getType() == SyntaxNode::ALTER || left->getType() == SyntaxNode::LEAF)
        {
            out += left->print() + "|" + right->print();
        }
        else
        {
            out += left->print() + "," + right->print();
        }
    }

    if (!bleft && !bright)
    {
        out = this->getSymbol();
    }
    return out;
}

ParseTree::ParseTree(const std::string& symbol) : _root(' ')
{

}

const SyntaxNode& ParseTree::getRoot()
{
    return _root;
}