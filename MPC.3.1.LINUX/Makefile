CPPC=gcc -DLINUX -DUNIX
ifeq ($(ISDEBUG),1)
	CPPC+=-gstabs 
	DESTDIR=Debug
else
	DESTDIR=Release
endif

MAINS=main
CLASSGENS=constant_pool classgen bytecode preverify
LEXS=lex.yy
UTILS=error memory strings 
# compat
PARSERS=parser stdpas
STRUCTURES=block identifier type type_list name_table string_list unit
PREVERS=file convert_md classresolver stubs classloader util \
   check_class sys_support jar_support check_code jar \
   inlinejsr main

DIRS=$(DESTDIR) $(DESTDIR)/classgen $(DESTDIR)/lex $(DESTDIR)/main \
   $(DESTDIR)/parser $(DESTDIR)/preverifier $(DESTDIR)/structures \
   $(DESTDIR)/util

ITEMS=$(CLASSGENS:%=classgen/%) $(LEXS:%=lex/%) $(MAINS:%=main/%) \
   $(PARSERS:%=parser/%) $(PREVERS:%=preverifier/%) \
   $(STRUCTURES:%=structures/%) $(UTILS:%=util/%)

all: release

re: clean all

clean:
	rm -f $(DESTDIR)/mp3CC $(ITEMS:%=$(DESTDIR)/%.o)

debug:
	ISDEBUG=1 make


release: $(DIRS) $(DESTDIR)/mp3CC

$(DIRS): 
	mkdir -p $@

$(DESTDIR)/%.o : %.c
	$(CPPC) -c -o $@ $< 

$(DESTDIR)/mp3CC: $(ITEMS:%=$(DESTDIR)/%.o)
	$(CPPC) -o $@ $^ -lm
