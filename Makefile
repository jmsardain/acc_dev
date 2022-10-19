CC = g++
INC_DIR = include
SRC_DIR = host
# CFLAGS = -c -g -Wall `root-config --cflags`
CFLAGS = -c -g -Wall -std=c++11 -I$(INC_DIR)
MYOBJS = plotHelper.o HoughHelper.o
DEPS = $(INC_DIR)/plotHelper.h $(INC_DIR)/HoughHelper.h

dataProcessor : dataProcessor.o $(MYOBJS)
	$(CC) dataProcessor.o $(MYOBJS) -o dataProcessor

dataProcessor.o: $(SRC_DIR)/dataProcessor.cxx $(DEPS)  $<
	$(CC) $(CFLAGS) $(SRC_DIR)/dataProcessor.cxx

plotHelper.o: $(INC_DIR)/plotHelper.cxx  $(INC_DIR)/plotHelper.h $<
	$(CC) $(CFLAGS) $(INC_DIR)/plotHelper.cxx

HoughHelper.o: $(INC_DIR)/HoughHelper.cxx $(INC_DIR)/HoughHelper.h $(INC_DIR)/plotHelper.h $<
	$(CC) $(CFLAGS) $(INC_DIR)/HoughHelper.cxx

clean:
	rm *o
