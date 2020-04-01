#
# 'make'        build executable file
# 'make clean'  removes all .o and executable files
# 'make cleandep' deletes all .d files
#

### Define according to folder structure ###
INCDIR=include#	headers folder
SRCDIR=src#		source folder/s and file extension
BUILDDIR=build#	object files folder
DEPDIR=build#	dependencies folder
BINDIR=bin#		binaries folder
# define the main files
TARGET=$(BINDIR)/mpc

#
# This uses Suffix Replacement within a macro:
#   $(name:string1=string2)
#         For each word in 'name' replace 'string1' with 'string2'
# Below we are replacing the suffix .c of all words in the macro SRCS
# with the .o suffix
#
# define the C source files
SRCS=$(shell find $(SRCDIR) -type f -name "*.cpp")
# define the C object files 
OBJS=$(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SRCS:.cpp=.o))
# define the auto-generated dependency files
DEPS=$(patsubst $(SRCDIR)/%,$(DEPDIR)/%,$(SRCS:.cpp=.d))

### Define compiling process ###
CXX=g++ # C compiler
INC=-I $(INCDIR)
CXXFLAGS=-Wall -Werror -Wno-unused -std=c++17 # Compile-time flags
LDLIBS=


	
# rule for .o files
$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INC) $(LDLIBS) -c $< -o $@

# rule to generate a dep file by using the C preprocessor
# (see man cpp for details on the -MM and -MT options)
$(DEPDIR)/%.d: $(SRCDIR)/%.cpp
	@mkdir build 2>/dev/null
	($(CXX) $(CXXFLAGS) $(INC) $(LDLIBS) $< -MM -MT $(@:.d=.o); echo '\t$(CXX) $(CXXFLAGS) $(INC) $(LDLIBS) -c $< -o $(@:.d=.o)';) >$@

# Include dependencies files
-include $(DEPS)

.PHONY: clean cleandep

$(TARGET): $(INCDIR)/syntax.tab.hh $(SRCDIR)/syntax.tab.cc lex.yy.c $(OBJS)
	$(CXX) $(CXXFLAGS) $(INC) $(LDLIBS) $(SRCDIR)/syntax.tab.cc lex.yy.c $(OBJS) -o $(TARGET)

clean:
	rm -f $(TARGET) $(OBJS) $(INCDIR)/syntax.tab.hh $(SRCDIR)/syntax.tab.cc lex.yy.c

cleandep:
	rm -f $(DEPS)



lex.yy.c: lexicon.ll $(INCDIR)/syntax.tab.hh
	flex $<

$(SRCDIR)/syntax.tab.cc $(INCDIR)/syntax.tab.hh : syntax.yy $(SRCS)
	bison --defines=$(INCDIR)/syntax.tab.hh -o $(SRCDIR)/syntax.tab.cc $<

run : $(TARGET) examples/example_program.mp
	./$(TARGET) <examples/example_program.mp
