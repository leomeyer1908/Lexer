This repository contains my implementation of a Lexer, which I plan to use
on other projects.

Files Breakdown:
NFA.h is the header file for the NFA and NFA Node data structure, and all 
functions associated with them. 
NFA.c contains the implementation for general functions for the NFA and 
NFA Node data structures.
regexToNFA.c contains the implementation for the RegEx related functions
for the NFA, which are the createNFAFromRegex() and detectErrorRegEx()
functions.
eliminateEpisilonNFA.c contains the implementation for an episilon elimination
algorithm on the eliminateEpisilonNFA() function.
nfaToDFA.c contains an implementation for the powerset construction algorithm
to convert an NFA to a DFA. The function that does this is createDFAFromNFA().