<div align="center">
    <h1>RISK</h1>

[Get Started] | [Learn] | [Architecture]
</div>

[Get Started]: guide/get_started.md
[Learn]: guide/learn.md
[Architecture]: arch/arch.md

> [!IMPORTANT]
> **RISK** is unstable. There is ***risk***y code.
> 
> *Currently working only with Windows x64.*

**RISK** is compiler for learning and experiments with syntax and ABI.

<!-- use `rust` for highlighting -->

## Hello World

```rust
fn main() {
    std::print("Hello, World!");
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
    std::print("You write: {num:X}");
    num
}
```

# Build

You must have `clang`:

1. Download repository:
    ```cmd
    git clone https:\\github\...
    ```
2. ...
