#                                                                      AshC
| **Element**     | **Składnia**                                                                    | **Opis**                                |
|------------------|----------------------------------------------------------------------------------|------------------------------------------|
| `program`        | `statement*`                                                                     | Cały program to lista instrukcji         |
| `statement`      | `let_decl` \| `fn_decl` \| `expr_stmt` \| `if_stmt` \| `while_stmt`              | Instrukcje                               |
| `let_decl`       | `let IDENTIFIER [: TYPE]? = expression ;`                                        | Deklaracja zmiennej                      |
| `fn_decl`        | `fn IDENTIFIER (params?) -> TYPE { statement* }`                                 | Definicja funkcji                        |
| `params`         | `param (, param)*`                                                               | Lista parametrów                         |
| `param`          | `IDENTIFIER : TYPE`                                                              | Jeden parametr                           |
| `if_stmt`        | `if expression { statement* } (else { statement* })?`                            | Instrukcja warunkowa                     |
| `while_stmt`     | `while expression { statement* }`                                                | Pętla while                              |
| `expr_stmt`      | `expression ;`                                                                   | Wyrażenie zakończone średnikiem          |
| `expression`     | `literal` \| `IDENTIFIER` \| `call` \| `binary_expr` \| `grouping`                | Podstawowe wyrażenie                     |
| `call`           | `IDENTIFIER (args?)`                                                             | Wywołanie funkcji                        |
| `args`           | `expression (, expression)*`                                                     | Argumenty funkcji                        |
| `binary_expr`    | `expression OPERATOR expression`                                                 | Działania arytmetyczne / logiczne        |
| `grouping`       | `( expression )`                                                                 | Grupowanie wyrażeń                       |
| `literal`        | `NUMBER` \| `STRING` \| `true` \| `false`                                         | Literały                                 |
| `TYPE`           | `int` \| `float` \| `bool` \| `string` \| `void`                                  | Typy                                     |
| `OPERATOR`       | `+` \| `-` \| `*` \| `/` \| `==` \| `!=` \| `<` \| `>` \| `<=` \| `>=` \| `&&` \| `\|\|` | Operatory                     |
