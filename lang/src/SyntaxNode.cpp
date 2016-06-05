#include "SyntaxNode.h"
#include "Parse.h"
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

SyntaxNode SyntaxNode::reduce_node(std::stack<SyntaxNode>& stack, SyntaxNode::NodeType type)
{
    SyntaxNode root(type);
    SyntaxNode& lhs = stack.top();
    root.setLeft(lhs);
    stack.pop();

    return root;
}

void SyntaxNode::add_node(std::stack<SyntaxNode>& stack, SyntaxNode& rhs, SyntaxNode::NodeType type)
{
    if (stack.size() > 0)
    {
        SyntaxNode root = reduce_node(stack, type);
        root.setRight(rhs);
        stack.push(std::move(root));
    }
    else
    {
        stack.push(std::move(rhs));
    }
}

void SyntaxNode::flush_stack(std::stack<SyntaxNode>& stack)
{
    while (stack.size() > 1)
    {
        // rhs first
        SyntaxNode rhs = std::move(stack.top());
        stack.pop();

        //lhs next
        add_node(stack, rhs, SyntaxNode::COMBINE);
    }
}

SyntaxNode SyntaxNode::tokenize(const std::string& token, const std::string& equality)
{
    size_t start = 0; size_t end = 0;
    const size_t size = equality.size();
    bool quotes = false;
    bool brackets = false;
    bool bracket_action = false;
    bool recurse = false;
    bool cache = false;
    char quote_char = 0;
    char bracket_char = 0;

    // Stack for resolving binary operators
    std::stack<SyntaxNode> stack;

    // end points to line after current char
    for (size_t end = 0; end < size; end++)
    {
        char ch = equality[end];
        parse::in_quotes(ch, quotes, quote_char);
        bracket_action = parse::in_brackets(ch, quotes, brackets, bracket_char);

        // Enter bracket
        if (bracket_action && brackets)
        {
            // Check if there is text before a bracket
            if (end - start > 0)
            {
                throw std::runtime_error("Braces must be fully delimited, found text before start brace");
            }
            // Skip everything to after bracket
            start = end + 1;
        }
        // Escape bracket
        else if (bracket_action && !brackets)
        {
            recurse = true;
        }

        if (((ch == '|' || ch == ',' || recurse || cache) && !quotes && !brackets) || end == size - 1)
        {
            // Cache allows node reuse in operator
            if (recurse)
            {
                SyntaxNode again = tokenize(token, equality.substr(start, end - start));
                stack.push(std::move(again));
                recurse = false;
                cache = true;
            }
            // If end of line get the correct length and mark EOL
            if (end == size - 1)
            {
                end++;
                ch = '!';
                if (quotes || brackets)
                {
                    throw std::runtime_error("Found EOL before end of quote/brace");
                }
            }
            // Process operator
            if (ch == '|' || ch == ',' || ch == '!')
            {
                if (!cache)
                {
                    // Check for empty symbol
                    if (end - start <= 0)
                    {
                        throw std::runtime_error("Empty symbol found while constructing rule");
                    }
                    // Make a node for the input
                    std::string s = equality.substr(start, end - start);
                    parse::strip_quotes(s);
                    SyntaxNode rhs(s);
                    add_node(stack, rhs, SyntaxNode::ALTER);
                }
                else
                {
                    // Pop cached node off the stack
                    SyntaxNode rhs = std::move(stack.top());
                    stack.pop();
                    add_node(stack, rhs, SyntaxNode::ALTER);
                    cache = false;
                }
                if (ch == ',')
                {
                    // Recurse the rest of the input; skip this character
                    start = end + 1;
                    SyntaxNode again = tokenize(token, equality.substr(start, size - start));
                    add_node(stack, again, SyntaxNode::CONCAT);
                    flush_stack(stack);

                    // end the loop
                    end = size - 1;
                }
                else if (ch == '!')
                {
                    flush_stack(stack);
                }
            }
            start = end + 1;
        }
    }
    return std::move(stack.top());
}

std::vector<std::string> SyntaxNode::toVector() const
{
    std::vector<std::string> out;
    SyntaxNode* left = _left.get();
    SyntaxNode* right = _right.get();
    if (left)
    {
        std::vector<std::string> rec = left->toVector();
        std::move(rec.begin(), rec.end(), std::back_inserter(out));
    }
    if (right)
    {
        std::vector<std::string> rec = right->toVector();
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
    SyntaxNode* left = _left.get();
    SyntaxNode* right = _right.get();
    if (left && right)
    {
        if (_type == NodeType::ALTER)
        {
            out += "[" + left->print() + "|" + right->print() + "]";
        }
        else if (_type == NodeType::CONCAT)
        {
            out += "{" + left->print() + "," + right->print() + "}";
        }
        else if (_type == NodeType::COMBINE)
        {
            out += "(" + left->print() + "!" + right->print() + ")";
        }
    }

    if (!left && !right)
    {
        out = this->getSymbol();
    }
    return out;
}

bool SyntaxNode::matches(char ch)
{
    SyntaxNode* left = _left.get();
    SyntaxNode* right = _right.get();
    if (!left && !right)
    {
        return _symbol[0] == ch;
    }
    else
    {
        return left->matches(ch) || right->matches(ch);
    }
}

bool SyntaxNode::matches(const std::string& match)
{
    for (auto& ch : match)
    {
        if (!matches(ch))
        {
            return false;
        }
    }
    return true;
}