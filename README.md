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

### Files Extensions
.jcs

.jc is taken (((

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

## Description and Docs

### Marks

#### Stages
- deprecated
- outdated (like deprecated, but feature was removed from this version)
- experimental
- stable


## Notes and Ideas

1. Lightweight bit-mask

If there's need to store a bunch of boolean values, it can be written with something like enum.

2. SQL like queries for loops and more.

Syntax:

for 

3. What about different code style support?

Python?
for x in array:
	pass

C-like?
for(x in array){
	
}

4. Idea: No switch-case statements! `Match`

I'm not sure that switch case will be useful and modern-like in JCS.
So I'm gonna use something like compilation of ideas from Elm-Lang and GDScript (`match` keyword).

Syntax:
```
match(x){
	"a" | "b": return true;
	"c": return false;
	_: throw "What??"
}
```

This is just my initial view of syntax, maybe i should improve it.

Here's some evident optimization possibilities like for `any` keyword.
If `any` is upper of something then everything under `any` will be skipped (or it can be removed by optiomization-level).
Why `any`?
I think it's pretty convenient to debug with it. If something is wrong at part of code where is `match`, I can put `any` at first line and check for any match.

5. Operator overloading in OOP

Now, I can't say anything about it, before OOP syntax is described.

6. Operator overloading with functions (?)

I have one strange idea.
What if there will be something like functions that register operators for type.

Syntax:
```
operator + (left: string, right: string) : string {
	return String.concat(left, right);
}
```

It has some problems with global registration and scopes.
So I'm not sure that it is good idea, but...

7. Special arrays for any case.

For example.
If need to iterate through an array:
```
a = []
a.iteratable = true

while(a.peek() != "i_search_for_it"){
	a.next();
}
```
It's just an example of syntax and it must be improved.

8. Inline Modules

```
val scope = {
	func greeting(){
		print('Hello')
	}

	func lambda_greeting() -> print('Hello')
	
	export { greeting }
}

scope.greeting() // prints "Hello"
scope.lambda_greeting() // throws an error

// Watch Note(9)
// Exporting one element to root

val some_stuff = {
	func do_smth_1(){
		// Do something
	}

	func do_smth_2(){
		// Do something 
	}

	func result(){
		do_smth_1()
		do_smth_2()
	}

	export result
}

// Now `some_stuff` is its `result` function
some_stuff()
```

9. Modules

10. Function prototyping

NO....

11. Primitive Types

In JacyScript there's no primitive types as they are.
All types have submembers that are built in.
E.g. All standard types have function `toBool()` that is used to put them to conditions or `toString` to convert to string
So, I think I can say that there's no "primitive" types. Or then array is primitive too. :)

Types are pretty same to TypeScript type declaration

Variables can be declared only by two ways: `var` / `val`
But the type can be a primitive or user-defined

List of primitive types:
* `any`
* `null`
* `num`
* `str`
* `type[]` - array
* `[user-defined]`
* `enum` (?)

List of built-in non-primitive types
* `range` - represents a range from one number to another (watch 16.)

e.g.
```
var a = 10
// `a` can be of `any` type and now contains value `10`
// Same to: val a: any = 10

var b: num = 10
// b can be only of type `num`

val c = getC()
// c is `val` so it cannot be re-assigned. Also `val` variables are always of type that was given.
// So, if `getC` returns `num` then c will be type of `num`
```

Note: generics (???)

12. Types casting

Different statements automaticaly cast to needed
e.g. if auto-cast condition to bool if it's possible

13. Non-primitive types


14. While in if-condition (?)

What about `while` in if conditions ?

Idea:
```
while(1){
	print('yes infinite times')
}else{
	print('no')
}
```
But that won't work instead of 'elif' word so I have to create new keyword,
and this's is the cause why I think it's a bad idea.

Or maybe it can be something like that
```
while(a){
	//
}elif(b){

}else{

}

// Or

while(a){

}elif while(b){

}else while(c){

}
```
WOW, It's pretty strange......

15. For loop

Watch Note.16

Oh, that's going to be pretty complex.

In JacyScript there's no for(`declaration`; `condition`; `loop-changes`)

Syntax:
```
for(var i in [1, 2, 3]){
	print(i) // Prints element
}

for(var x in [1, 2, 3].range){
	print(x) // Prints index
}
// In the example above field `range` equals to `0:[1, 2, 3].length` so it's possible to write this:
for(var x in 0:[1, 2, 3].length){/**/}
// But it looks pretty strange

// Print numbers in range 1-5 end-exclusive
for(var x in 1:5){
	print(x)
}
```

Idea:
Multiple iterators like in Julia Lang (?)
```
for(var x in 5, var y in )
```

Note: Steps (?!!!)

16. Ranges (`:` range operator)

Range is non-primitive types, but it determines (Think: maybe not...) as `range` by `typeof` operator (watch 17.)

`range` represents a range between two numbers (`num` type) and can have one optional number (step)

Note: End-number is exclusive

Syntax:
```
val range = 0:100

// Print numbers from 0 to 99
for(var i in range){
	print(i)
}

val range_with_step = 0:100:2

// Print numbers from 0 to 99 with step 2
for(var i in range_with_step){
	print(i)
}
```

Idea:
What if ranges could be used as arrays:
```
val x = 5:100

print(x[5]) // Prints `10`
```
But wtf??? What is the case when it's useful?!


17. `typeof` keyword

18. Idea: Objects

Do I need them?? Objects like in JS (?)

I don't think that it's pretty good idea to add them.

pros:
- Ease of use
- Quick work with JSON and JSON-like syntax

cons:
- Problems with typing
- Uncontrollable

19. Classes

Sooo... Ok, I'm not quite sure about `object`, but I know that there must be classes.

Syntax of class declaration:
```
class Vehicle{
	var color: string = 'red'

	Vehicle(color: string){
		this.color = color
	}
}
```

In Jacy classes `this` keyword behaviour is same to C++.
So, if in the example above argument `color` had different name then `this` was unnecessary.

By defualt `color` is private and it's ok in Jacy not write `private` keyword 

Future features:
- `get` / `set`

20. Idea: Ranges generators

Idea:
'..' operator will automatically generate numbers or strings(?) in given range start-end-inclusive.

Syntax:
```
val numbers = [0..100]

// `numbers` contains all numbers in range from 0 to 100 inclusive


```

21. Idea: Dialects

Maybe, in the future I'll add special dialects feature.
At most to create more C-like or more Python-like implementations of Jacy.

22. Tuples

Tuples are fixed-size arrays with specified types.

(Watch 23.)

Syntax:
```
var tuple: [string, number]
```

23. User-defined types, `type` keyword

Syntax:
```
// Define new type `pair` as tuple of two strings
type pair = [string, string]
```

24. `match` statement and match-functions

In JacyScript there's no `switch-case` statement (Watch 4.)

Features:
- No `break` keyword
- No `continue` to implement multiple cases, use `|` instead
- Types-matching
- `_` instead of `default` like in GDScript

Syntax:
```
val x = 'abc'
match(x){
	'abc': print('yes!')
	'cab' | 
}
```

If you need to put bitwise or in case, it's possible.
Just need to put expression in parentheses, like that:
```
val a = 101
val b = 010
val x = 111

match(x){
	(a | b): print('Ohhhh, yeaaas')
	_: print(':(')
}
```

Types-matching (Watch 27.):
```
var x = 'string'
// Way 1.
match(x){
	type string: print('Is string')
	_: print('Not a string')
}

// Way 2.
match(typeof x){
	'string': print('Is string')
	_: print('Not a string')
}
```

25. Checking for `instance of`, `is` keyword

26. Type casting, `as` keyword

27. Type checking, `typeof` keyword