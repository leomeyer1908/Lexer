This repository contains my implementation of a Lexer, which I plan to use
on other projects.

Files Breakdown:
NFA/NFA.h is the header file for the NFA and NFA Node data structure, and all 
functions associated with them. 
NFA/NFA.c contains the implementation for general functions for the NFA and 
NFA Node data structures.
NFA/regexToNFA.c contains the implementation for the RegEx related functions
for the NFA, which are the createNFAFromRegex() and detectErrorRegEx()
function.