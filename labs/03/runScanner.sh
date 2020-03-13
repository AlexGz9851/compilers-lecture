#! /bin/sh
#rebuild scanner if necesary
make lexical_scanner
#run program
./lexical_scan "$@" > lex.out