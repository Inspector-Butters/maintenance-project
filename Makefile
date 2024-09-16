CUNIT := -lcunit
DEBUG := -g -ggdb
CC := gcc -Wall -pedantic -std=c11
VALGRIND := valgrind --leak-check=full

common.o: common.c common.h
	$(CC) $(DEBUG) -c common.c

hash_table.o: hash_table.c hash_table.h linked_list.o common.o iterator.h
	$(CC) $(DEBUG) -c hash_table.c

httests: hash_table.o linked_list.o hash_table_tests.c
	$(CC) $(DEBUG) $(CUNIT) common.o hash_table.o linked_list.o hash_table_tests.c -o httests

htvalgrind: httests
	$(VALGRIND) ./httests

linked_list.o: common.o linked_list.c linked_list.h iterator.h
	$(CC) $(DEBUG) -c linked_list.c

lltests: common.o linked_list.o linked_list_tests.c
	$(CC) $(DEBUG) $(CUNIT) common.o linked_list.o linked_list_tests.c -o lltests

llvalgrind: lltests
	$(VALGRIND) ./lltests

ittests: linked_list.o iterator_tests.c common.o
	$(CC) $(DEBUG) $(CUNIT) common.o linked_list.o iterator_tests.c -o ittests

itvalgrind: ittests
	$(VALGRIND) ./ittests

backend.o: linked_list.o hash_table.o common.o backend.c backend.h
	$(CC) $(DEBUG) -c backend.c

db: frontend.c frontend.h common.o linked_list.o hash_table.o backend.o
	$(CC) $(DEBUG) common.o hash_table.o linked_list.o backend.o frontend.c -o db

dbvalgrind: db
	$(VALGRIND) ./db


tests: common.o linked_list.o hash_table.o backend.o tests.c
	$(CC) $(DEBUG) $(CUNIT) common.o hash_table.o linked_list.o backend.o tests.c -o tests


testsvalgrind: tests
	$(VALGRIND) ./tests
.PHONY: clean

make clean:
	rm -f *.o httests lltests ittests frontend db tests