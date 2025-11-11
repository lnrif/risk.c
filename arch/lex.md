# Lexer — not about linguistics

So, a lexer is a simplified analysis of source code into minimal meaningful units (called tokens), without complex morphology or context-sensitive rules.

> Lexical analysis usually involves identifying prefixes, suffixes, and roots...

Let's move on to examples:

```
`y` => Id
`0` => Int
`=` => Eq
`*` => Star
```

All simple:

```
`x = 2 * w` => [Id, Eq, Int, Star, Id]
```

The lexer facilitates further analysis:

- Discards unnecessary information (spaces, comments)
- Groups similar parts (identifiers, operators)
- Independent of specific encoding (UTF-8, WTF-8)

# Groups

Consider the example `b + 1` and `2 * x`. The expression has the form:

```
`b + 1` => [Atom, Op, Atom]
```

**Atom** — number or identifier
```
Atom ::= Int | Id
Id   ::= `b` | `x`
Int  ::= `1` | `2`
```

**Op** — plus or multiply
```
Op   ::= Plus | Star
Plus ::= `+`
Star ::= `*`
```

In this case:

- Atom & Op — groups
- Id & Int, Plus & Star — tokens

# Lexer in RISK

The compiler has a manual lexer. We simply collect tokens into the `Tokens` array.

```c
typedef enum : u8 {
    TOKEN_IDENT,
    TOKEN_INTEGER,
    TOKEN_ADD,
    TOKEN_MUL,
} Kind;

typedef struct {
    u16 start;
    u16 len;
} Span;

typedef struct {
    Span span;
    Kind kind;
} Token;

typedef struct {
    Span *span;
    Kind *kind;
    usize len;
    usize cap;
} Tokens;

```

> No ideas? Open `risk.c`, press `CTRL + F`, and enter `Lexer`. Good luck!

# Outro

The whole idea of types and groups is reminiscent of higher mathematics.

