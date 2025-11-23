<div align="center">
    <h1>RISK</h1>

[Get Started] | [Learn] | [Architecture]
</div>

[Get Started]: guide/get_started.md
[Learn]: guide/learn.md
[Architecture]: arch/arch.md

> [!WARNING]
> THIS COMPILER IS A WORK IN PROGRESS! USE THIS COMPILER AT YOUR OWN ***RISK***!

**RISK** is compiler for learning purposes and experiments with ABI.

# Layers complete

- [x] Tokens & lex
- [ ] AST & parse (in progress)
- [ ] MIR & semantic checks
- [ ] IR & lowering to concrete platform
- [ ] ASM

# Features

- Linear type system & mutability
- Extensible explicit drop system & defer
- Comptime evaluation & meta programming (aka Zig comptime)
- Experiments with optimizations & ABI

<!-- use `rust` for highlighting -->

## Hello World

```rust
fn main() {
    std.print("Hello, World!");
}
```

## Fibonacci

```rust
fn fib(n: u64) u64 {
    match n {
        ..=1 => n,
        _    => fib(n - 1) + fib(n - 2),
    }
}
```

## IO

```rust
fn main() i32 {
    let buf = [0] * 64;
    let Ok(len) = std.read(&buf);
    let Ok(num) = buf[0..len].parse();
    std.print("You write: {num:X}");
    return num;
}
```

# Platforms

For now only Windows x64.

# Build

You must have `clang`

1. Download:
    ```cmd
    git clone https://github.com/lnrif/risk.c.git
    ```
2. Compile:
    ```cmd
    clang -O2 -Wall -Wextra -Wno-unused-function risk.c -o risk.exe
    ```
3. Use:
    ```cmd
    risk
    ```
