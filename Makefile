
# TARGET=bin/lexicon

# $(TARGET): lexicon.c lexicon.h
# 	@if cat /etc/os-release | grep -q Fedora; then \
# 		echo "Compiling with Fedora settings.";    \
#         gcc lexicon.c -o $@;                       \
# 	else                                           \
# 		echo "Compiling with Ubuntu settings.";    \
#         gcc lexicon.c -lfl -o $@;                  \
# 	fi

# lexicon.c: lexicon.l lexicon.h
# 	flex -o $@ lexicon.l

# clean:
# 	rm -f $(TARGET) lexicon.c

# run: $(TARGET) examples/example_program2.mp
# 	./$(TARGET) <examples/example_program2.mp || true


TARGET=bin/COMPILER

$(TARGET): main.c syntax.tab.c lex.yy.c
	gcc $^ -lfl -o $@

lex.yy.c: lexicon.l syntax.tab.h
	flex $<

syntax.tab.c syntax.tab.h : syntax.y
	bison -d $<

clean:
	rm -f $(TARGET) syntax.tab.* lex.yy.c

run : $(TARGET) examples/example_program.mp
	./$(TARGET) <examples/example_program.mp
