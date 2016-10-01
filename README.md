# mod_ebnf
Repository for the Modified Extended Backus–Naur Form Parser

This EBNF parser will validate text with a modified EBNF rule set. Alternations, concatenations, and repetition operations are allowed.

| = alternation
, = concatenation
{} = repetition
()/[] = priority brackets
""/'' can be used for expressing leaf nodes, note that to escape "/' write '"' or "'".

GCC and VC build scripts are available. For GCC, see mod_ebnf/build and mod_ebnf/runtests. For VC, see mod_ebnf.sln. -std=C++14 must be used for compilation.

GPL3 license, see LICENSE.md in project root for details.
