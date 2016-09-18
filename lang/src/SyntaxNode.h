// Copyright 2016 <Aaron Springstroh>
#ifndef __SyntaxNode_

#include <memory>
#include <set>
#include <stack>
#include <string>

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
    void set_left(SyntaxNode& next);
    void set_right(SyntaxNode& next);
    void set_type(NodeType type) { _type = type; }
    void set_repeat(bool repeat) { _repeat = repeat; }
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
};

#endif
