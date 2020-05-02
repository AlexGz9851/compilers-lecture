all:
	flex rdp.l
	bison -d rdp.y
	gcc -o rdp rdp.tab.c lex.yy.c -ly -ll

clean:
	rm rdp
	rm rdp.tab.h
	rm rdp.tab.c
	rm lex.yy.c 