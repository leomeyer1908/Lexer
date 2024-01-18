This repository contains my implementation of a Lexer, which I plan to use on other projects.

Usage:
Once compiled, the program takes a token spec as its first argument and an input string as the second argument.

Token Spec:
The token spec should contain regular expressions and its corresponding token delimited by a space on each line of the file. The regular expressions accept '+' as concatenation, '|' as or, '\*' as Kleene star, parenthesis for prescedence, brackets for including a list of characters that should be accepted, and brackets beginning with a '^' to match all characters except those inside the brackets following the '^'. To capture the characters, '+', '|', '\*', '(', ')', '[', ']', '^', space, and '\\', as operands to the regular expression, a backslash should be prepended before them. A backslash can also be used to represent '\\n' and '\\t'.

Implementation details:
This lexer works by taking in an input file, which is tokenized by the program based on the token spec provided. The tokenization works by first converting each regular expression into a DFA. Then we loop through each character of the input and use that character to traverse through each DFA's transition. The program does this until all DFA die, and then it tokenizes the current input based on the longest DFA that reached an accept state. If two DFA's tie, the first one in the input spec is chosen. We then create a token with that input string that we just traversed, and save its line and colmun number as well. We then start off with all DFAs back in their start state and start iterating through the input string again from the first character after the string we just tokenized. We repeat this process until we have tokenized the entire input string.

The lexer has only been tested with the C token spec (currently not complete), but I manually verified that all token combinations work for this spec on tests/allTokenCombinations.test.