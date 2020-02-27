calculator : main.c calculator.tab.c lex.yy.c
	gcc $^ -lfl -o $@

lex.yy.c : calculator.l calculator.tab.h
	flex $<

calculator.tab.c calculator.tab.h : calculator.y
	bison -d $<

clean :
	rm -f calculator calculator.tab.* lex.yy.c

run : calculator test.txt
	./calculator <test.txt
