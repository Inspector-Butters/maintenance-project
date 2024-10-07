CUNIT := -lcunit
DEBUG := -g -ggdb
CC := gcc -Wall -pedantic -std=c11
VALGRIND := valgrind --leak-check=full

common.o: common/common.c common/common.h
	$(CC) $(DEBUG) -c common/common.c -o common/common.o

hash_table.o: structs/hash_table.c structs/hash_table.h structs/linked_list.o common/common.o structs/iterator.h
	$(CC) $(DEBUG) -c structs/hash_table.c -o structs/hash_table.o

httests: hash_table.o linked_list.o hash_table_tests.c
	$(CC) $(DEBUG) $(CUNIT) common.o hash_table.o linked_list.o hash_table_tests.c -o httests

htvalgrind: httests
	$(VALGRIND) ./httests

linked_list.o: common/common.o structs/linked_list.c structs/linked_list.h structs/iterator.h
	$(CC) $(DEBUG) -c structs/linked_list.c -o structs/linked_list.o

lltests: common.o linked_list.o linked_list_tests.c
	$(CC) $(DEBUG) $(CUNIT) common.o linked_list.o linked_list_tests.c -o lltests

llvalgrind: lltests
	$(VALGRIND) ./lltests

ittests: linked_list.o iterator_tests.c common.o
	$(CC) $(DEBUG) $(CUNIT) common.o linked_list.o iterator_tests.c -o ittests

itvalgrind: ittests
	$(VALGRIND) ./ittests

backend.o: structs/linked_list.o structs/hash_table.o common/common.o backend/backend.c backend/backend.h
	$(CC) $(DEBUG) -c backend/backend.c -o backend/backend.o

db: frontend/frontend.c frontend/frontend.h common/common.o structs/linked_list.o structs/hash_table.o backend/backend.o
	$(CC) $(DEBUG) common/common.o structs/hash_table.o structs/linked_list.o backend/backend.o frontend/frontend.c -o db

dbvalgrind: db
	$(VALGRIND) ./db


tests: common.o linked_list.o hash_table.o backend.o tests.c
	$(CC) $(DEBUG) $(CUNIT) common.o hash_table.o linked_list.o backend.o tests.c -o tests


testsvalgrind: tests
	$(VALGRIND) ./tests
.PHONY: clean

make clean:
	rm -f *.o httests lltests ittests frontend/frontend db tests