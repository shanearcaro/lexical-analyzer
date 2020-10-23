# cs280-assignment2

## Lexical Analyzer

The analyzer is used to read in a text file and determine what the next token is. With the knowledge of the next and previous token, the analyzer determines if an error has occured and what the error is. The language that is being analyzer here is an untyped language that can recognzie `strings`, `ints`, and `real numbers`. The language also supports single line `comments` and single line `if then` statements.

## Implementation

This assignment is a combination of an earlier assignment to overload the `<<` operator for output of a `LexItem` token. This assignment will be combined with our next assignment, creating a parser.

## Flag Types

Parameter | Description | Required
----------|-------------|---------
`-v` | Prints tokens when found | No
`-iconsts` | Prints integer tokens | No
`-rconsts` | Prints real tokens | No
`-sconsts` | Print strings | No
`-ids` | Prints identifiers | No
`filename` | File path | Yes
