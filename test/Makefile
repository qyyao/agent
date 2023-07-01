# Copyright 2019, Winfield Chen and Lloyd T. Elliott.

SRCDIRS := ../src ../lib/src
SOURCES := $(wildcard $(foreach i,$(SRCDIRS),$i/*.c)) 
SOURCES := $(filter-out ../src/main.c,$(SOURCES))
OBJECTS := $(SOURCES:%.c=%.o)

test: test.c
	gcc -c util.c -std=c17 -I../include -I../lib/include -I/usr/local/include -I/home/annieyao/FiniteStateEntropy/lib -I/home/annieyao/software/zstd/include
	gcc test.c -o test util.o ../src/linkedlist.o ../src/read.o ../src/readthread.o ../src/samplefile.o ../src/threadpipe.o ../src/bgenformat.o ../src/file.o ../src/computetask.o ../src/writethread.o ../src/write.o ../src/phenotype.o ../src/agentformat.o ../src/memman.o ../src/options.o ../src/decompress.o ../src/computethread.o ../src/statistics.o ../src/agent.o ../src/dosageencode.o ../src/matrix.o ../src/compress.o -std=c17 -I../include -I../lib/include -I/usr/local/include -I/home/annieyao/FiniteStateEntropy/lib -I/home/annieyao/software/zstd/include -I/home/annieyao/lib/ -L/usr/local/lib -L/home/annieyao/FiniteStateEntropy/lib -L/home/annieyao/software/zstd/lib -L/home/annieyao/lib/ -lfse -lzstd -lpthread -lblas -lcmocka -lm -lgsl -fopenmp

clean:
	rm test
	rm util.o
