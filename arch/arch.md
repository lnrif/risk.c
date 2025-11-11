The **RISK** compiler consists of **3 layers**:

1. **`Parse`**
    - [`Tokens`](lex.md) (text pattens)
    - [`AST`](ast.md) (syntax & precedence)
2. **`Check`**
    - [`HIR`](hir.md) (explicitness & types) - *syntax sugar*
    - [`MIR`](mir.md) (mutations & lifetimes) + *optimize/*
3. **`Compile`**
    - [`LIR`](lir.md) & [`FASM`](https://flatassembler.net/)
    - [`QBE`](https://c9x.me/compile/) (soon)
    - [`LLVM`](https://llvm.org/) (soon)

The general scheme looks like this:

```
file.rk --> Parse --> Check --> Compile --> file.exe
```

## Parse

```
file.rk -----> Tokens -------> AST
         lex           parse
```

## Check

```
        types            mutations
AST --------------> HIR -----------> MIR
     explicitness        lifetimes
```

## Compile

```
     LIR -> file.asm -> FASM
   /                         \
MIR ---------> QBE ---------> file.exe 
   \                         /
    * -------> LLVM ------> * 
```

> NOT CHATGPT (Claude AI, joke)
