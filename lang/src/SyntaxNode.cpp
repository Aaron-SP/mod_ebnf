#include "SyntaxNode.h"
#include <algorithm>
#include <iterator>

SyntaxNode::SyntaxNode(NodeType type)
{
    _type = type;
}

SyntaxNode::SyntaxNode(const std::string& symbol) : _symbol(symbol), _type(NodeType::LEAF)
{

}

void SyntaxNode::setLeft(SyntaxNode& left)
{
    _left.reset(new SyntaxNode(std::move(left)));
}

void SyntaxNode::setRight(SyntaxNode& right)
{
    _right.reset(new SyntaxNode(std::move(right)));
}

const std::string& SyntaxNode::getSymbol() const
{
    return _symbol;
}

SyntaxNode::NodeType SyntaxNode::getType() const
{
    return _type;
}

void SyntaxNode::setType(NodeType type)
{
    _type = type;
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
        if (left->getType() == NodeType::ALTER || left->getType() == NodeType::LEAF)
        {
            out += "[" + left->print() + right->print() + "]";
        }
        else if (left->getType() == NodeType::CONCAT)
        {
            out += "{" + left->print() + right->print() + "}";
        }
        else if (left->getType() == NodeType::COMBINE)
        {
            out += "(" + left->print() + right->print() + ")";
        }
    }

    if (!bleft && !bright)
    {
        out = this->getSymbol();
    }
    return out;
}