TARGETS := demo 

target_OBJECTS = root.o rte_json.o rte_vmconfig.o

LIBS = -lm
INCLUDES =
CFLAGS = -g -Wall
CC = gcc

COMPILE = $(CC) $(CFLAGS) $(INCLUDES) $(LIBS)
LINK = $(COMPILE) -o $@

all: $(TARGETS)
	
$(TARGETS): $(target_OBJECTS)
	$(LINK) $(target_OBJECTS)

root.o: root.c
	$(COMPILE) -c -o $@ $<

rte_json.o: rte_json.c rte_json.h
	$(COMPILE) -c -o $@ $<

rte_vmconfig.o: rte_vmconfig.c rte_vmconfig.h
	$(COMPILE) -c -o $@ $<

clean:
	rm -rf *.o 
	rm -rf $(TARGETS)

