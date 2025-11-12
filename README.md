## SLang (Интерпретируемы ООП язык с C-подобным синтаксисом)

> Проект-курсовая работа для УлГУ 2022-2025г.

SLang - высокоуровневый, интерпретируемы, объектно-ориентированный язык программирования с С-подобным синтаксисом. Написан на C++20.
Парсер работает по принципу рекурсивного спуска. Выполнение кода работает по принципу Tree-Walk interpreter.
Данный проект вдохновлен и основан на информации из книги Роберта Нистрома ["Crafting interpreters"](https://craftinginterpreters.com/).

Описание синтаксиса в виде [bnf](https://github.com/KamilM1205/slang/blob/dev/bnf.bnf).

### Примеры работы парсера

___

Привет мир!
Код:

```rust
fn entry() {
  print("Hello, world!");
}
```

Результат работы парсера:

```
FnDef(entry; ;
  Block(
    ExprStmt(
      FunCall(print;
        Value(LITTERAL; Hello, world!)
      )
    )
  )
)
```

Рекурсивный алгоритм Фибоначчи

```rust
fn fib(x: int): int {
  if (x <= 1) {
      return x;
  }

  return fib(x - 1) + fib(x - 2);
}

fn entry() {
  var n: int;
  n = 23;

  for (var i = 1; i < n; i++) {
    fib(i);
  }
}
```

Результат работы парсера:

```
FnDef(fib; Arg(x; INT); Block(If(Binary(
    Value(IDENTIFIER; x);
    LE;
    Value(NUMBER; 1)
); Block(Return(Value(IDENTIFIER; x))
)
)
, Return(Binary(
    FunCall(fib; Binary(
    Value(IDENTIFIER; x);
    MINUS;
    Value(NUMBER; 1)
));
    PLUS;
    FunCall(fib; Binary(
    Value(IDENTIFIER; x);
    MINUS;
    Value(NUMBER; 2)
))
))
)
)
FnDef(entry; ; Block(Define(n; ; {}), ExprStmt(Assign(n; Value(NUMBER; 23))
)
, For(Define(i; 
    Value(NUMBER; 1)); Binary(
    Value(IDENTIFIER; i);
    LS;
    Value(IDENTIFIER; n)
); ; Block()
)
, Block(ExprStmt(FunCall(fib; Value(IDENTIFIER; i)))
)
)
)
```

### Пример вывода ошибок

---

Код:

```rust
fn entry() {
  var a = 5
  print("A = ", a)
}
```

Вывод ошибки:

```
Error: at [2; 11] Unexpected end of line.
    2 |   var a = 5
                  ^

A fatal error has occurred. Panic!
```

Код:

```rust
fn entry() {
  var a = 5;
  print("A = ", a ;
}
```

Вывод ошибки:

```
Error: at [3; 19] Expected token: BR_END. Found: SEMICOLON.
    3 |   print("A = ", a ;
                          ^

A fatal error has occurred. Panic!
```

### План работы:

---

- [X] Токенизатор
- [X] Парсер
- [ ] Интерпретатор
