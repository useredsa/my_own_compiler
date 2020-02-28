
TARGET=bin/lexicon

$(TARGET): lexicon.c lexicon.h
	@if cat /etc/os-release | grep -q Fedora; then \
		echo "Compiling with Fedora settings.";    \
        gcc lexicon.c -o $@;                       \
	else                                           \
		echo "Compiling with Ubuntu settings.";    \
        gcc lexicon.c -lfl -o $@;                  \
	fi

lexicon.c: lexicon.l lexicon.h
	flex -o $@ lexicon.l

clean:
	rm -f $(TARGET) lexicon.c

run: $(TARGET) examples/example_program2.mp
	./$(TARGET) <examples/example_program2.mp || true
