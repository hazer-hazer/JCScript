# JacyScript

## Specification

### Variables init/define/declare
Variables are auto-typed and be only declared by two ways:
- `var`
- `val`

`var`/`val` are inspired by Kotlin and have same definition.

`var` is for variables. It can be changed, it's auto-typed.
`val` is for runtime constants constants.

Now there's nothing about compilation-time constants, but in the future it will be 
implemented with # or something like that define-style.

## Tokens

- Identifiers = variable names / keywords
- Operators
- Quote = `'` / `"`
- Keyword - identifier determined as keyword

### Identifiers
Identifiers can contain letters, undersore symbol (`_`) or dollar symbol (`$`)

### Operators

#### List of all operators
33 `!` 
35 `#` 
37 `%` 
38 `&` 
40 `(` 
41 `)` 
42 `*` 
43 `+` 
44 `,` 
45 `-` 
46 `.` 
47 `/` 
58 `:` 
59 `;` 
60 `<` 
61 `=` 
62 `>` 
63 `?` 
64 `@` 
91 `[` 
92 `\` 
93 `]` 
94 `^` 
95 `_` 
123 `{`
124 `|`
125 `}`
126 `~`