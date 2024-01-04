build-test:
	gcc -g -Wall test.c NFA/NFA.c NFA/regexToNFA.c NFA/eliminateEpisilonNFA.c NFA/nfaToDFA.c DataStructures/HashMap.c DataStructures/HashSet.c DataStructures/LinkedList.c DataStructures/Node.c DataStructures/DoublyNode.c DataStructures/Stack.c -o prog
