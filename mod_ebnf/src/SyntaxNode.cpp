// Copyright 2016 <Aaron Springstroh>
#include "SyntaxNode.h"
#include "Parse.h"
#include <algorithm>
#include <iterator>
#include <memory>
#include <set>
#include <stack>
#include <string>
#include <utility>
#include <vector>

SyntaxNode::SyntaxNode(NodeType type) : _type(type), _repeat(false) {}

SyntaxNode::SyntaxNode(const std::string& symbol)
    : _symbol(symbol), _type(NodeType::LEAF), _repeat(false) {}

void SyntaxNode::set_left(SyntaxNode& left)
{
    _left = std::make_unique<SyntaxNode>(std::move(left));
}

void SyntaxNode::set_right(SyntaxNode& right)
{
    _right = std::make_unique<SyntaxNode>(std::move(right));
}

SyntaxNode SyntaxNode::reduce_node(std::stack<SyntaxNode>& stack, SyntaxNode::NodeType type)
{
    SyntaxNode root(type);
    SyntaxNode& lhs = stack.top();
    root.set_left(lhs);
    stack.pop();

    return root;
}

void SyntaxNode::add_node(std::stack<SyntaxNode>& stack, SyntaxNode& rhs, SyntaxNode::NodeType type)
{
    if (stack.size() > 0)
    {
        SyntaxNode root = reduce_node(stack, type);
        root.set_right(rhs);
        stack.push(std::move(root));
    }
    else
    {
        stack.push(std::move(rhs));
    }
}

SyntaxNode SyntaxNode::tokenize(const std::string& token, const std::string& equality)
{
    size_t start = 0; size_t end = 0;
    const size_t size = equality.size();
    bool quotes = false;
    bool brackets = false;
    bool bracket_action = false;
    bool cache = false;
    char quote_char = 0;
    char bracket_char = 0;
    char last_bracket = 0;
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
            last_bracket = bracket_char;
            // Check if there is text before a bracket
            if (end - start > 0)
            {
                throw std::runtime_error("Braces must be fully delimited, "
                    "found text before start brace");
            }
            // Skip everything to after bracket
            start = end + 1;
        }
        // Escape bracket
        else if (bracket_action && !brackets)
        {
            // Cache allows node reuse in operator
            SyntaxNode again = tokenize(token, equality.substr(start, end - start));
            if (last_bracket == '{')
            {
                again.set_repeat(true);
            }
            stack.push(std::move(again));
            cache = true;
            // Skip everything to after bracket
            start = end + 1;
        }

        // If end of line get the correct length and mark EOL
        if (end == size - 1)
        {
            end++;
            ch = '!';
            if (quotes || brackets)
            {
                throw std::runtime_error("Found end of file before end of quote/brace");
            }
        }

        // Process operator
        bool process = (ch == '|' || ch == ',' || ch == '!');

        if (process && (!quotes && !brackets))
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

                // end the loop
                end = size - 1;
            }
            start = end + 1;
        }
    }
    if (stack.size() != 1)
    {
        throw std::runtime_error("Stack is in an indeterminant state");
    }
    // If we can simplify this to a simple vector
    SyntaxNode& ref = stack.top();
    if (ref.can_simplify())
    {
        ref.simplify();
    }
    return std::move(ref);
}

void SyntaxNode::extract_symbols(std::set<std::string>& out) const
{
    SyntaxNode const* left = _left.get();
    SyntaxNode const* right = _right.get();
    if (left)
    {
        left->extract_symbols(out);
    }
    if (right)
    {
        right->extract_symbols(out);
    }
    if (_type == NodeType::LEAF)
    {
        out.insert(this->get_symbol());
    }
    else if (_type == NodeType::SIMPLE_LEAF)
    {
        std::move(_simple.begin(), _simple.end(), std::inserter(out, out.end()));
    }
}

void SyntaxNode::extract_leafs(std::vector<std::string>& out)
{
    SyntaxNode * left = _left.get();
    SyntaxNode * right = _right.get();
    if (left)
    {
        left->extract_leafs(out);
        _left.release();
    }
    if (right)
    {
        right->extract_leafs(out);
        _right.release();
    }
    if (_type == NodeType::LEAF)
    {
        out.push_back(std::move(this->get_symbol()));
    }
}

bool SyntaxNode::can_simplify() const
{
    bool out = true;
    SyntaxNode const* left = _left.get();
    SyntaxNode const* right = _right.get();
    if (left)
    {
        out = out && left->can_simplify();
    }
    if (right)
    {
        out = out && right->can_simplify();
    }
    // Only alternations for all children == true
    return out && (_type != NodeType::CONCAT) && !_repeat;
 }

void SyntaxNode::simplify()
{
    // Make a vector of symbols for speed optimization
    extract_leafs(_simple);
    _type = NodeType::SIMPLE_LEAF;
}