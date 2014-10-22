TARGETS := demo 

target_OBJECTS = root.o rte_json.o rte_parser.o rte_vmconfig.o

INCLUDES =
CFLAGS = -g
CC = gcc

COMPILE = $(CC) $(CFLAGS) $(INCLUDES)
LINK = $(COMPILE) -o $@

all: $(TARGETS)
	
$(TARGETS): $(target_OBJECTS)
	$(LINK) $(target_OBJECTS)

root.o: root.c
	$(COMPILE) -c -o $@ $<

rte_json.o: rte_json.c rte_json.h
	$(COMPILE) -c -o $@ $<

rte_parser.o: rte_parser.c rte_parser.h
	$(COMPILE) -c -o $@ $<

rte_vmconfig.o: rte_vmconfig.c rte_vmconfig.h
	$(COMPILE) -c -o $@ $<

clean:
	rm -rf *.o 
	rm -rf $(TARGETS)

