
TARGET=bin/lexicon

$(TARGET): lexicon.c lexicon.h
	gcc lexicon.c -lfl -o $@

lexicon.c: lexicon.l lexicon.h
	flex -o $@ lexicon.l

clean:
	rm -f $(TARGET) lexicon.c

run: $(TARGET) examples/example_program.mp
	./$(TARGET) <examples/example_program.mp
