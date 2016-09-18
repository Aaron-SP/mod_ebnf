// Copyright 2016 <Aaron Springstroh>
#ifndef __SyntaxNode_

#include <memory>
#include <set>
#include <stack>
#include <string>
#include <vector>

class SyntaxNode
{
 public:
    enum NodeType { SIMPLE_LEAF, LEAF, ALTER, CONCAT };
 protected:
    std::string _symbol;
    std::unique_ptr<SyntaxNode> _left;
    std::unique_ptr<SyntaxNode> _right;
    std::vector<std::string> _simple;
    NodeType _type;
    bool _repeat;
 protected:
    static SyntaxNode reduce_node(std::stack<SyntaxNode>& stack, SyntaxNode::NodeType type);
    static void add_node(std::stack<SyntaxNode>& stack, SyntaxNode& rhs, SyntaxNode::NodeType type);
    void extract_symbols(std::vector<std::string>& out);
    void set_left(SyntaxNode& next);
    void set_right(SyntaxNode& next);
    void set_type(NodeType type) { _type = type; }
    void set_repeat(bool repeat) { _repeat = repeat; }
    bool can_simplify() const;
    void simplify();
 public:
    static SyntaxNode tokenize(const std::string& token, const std::string& equality);
    explicit SyntaxNode(NodeType type);
    explicit SyntaxNode(const std::string& symbol);
    SyntaxNode(SyntaxNode&& node) = default;
    SyntaxNode& operator=(SyntaxNode&& node) = default;
    NodeType get_type() const { return _type; }
    SyntaxNode const* get_left() const { return _left.get(); }
    SyntaxNode const* get_right() const { return _right.get(); }
    bool get_repeat() const { return _repeat; }
    const std::string& get_symbol() const { return _symbol; }
    void extract_symbols(std::set<std::string>& out) const;
    const std::vector<std::string>& get_simplified() const { return _simple; }
};

#endif
