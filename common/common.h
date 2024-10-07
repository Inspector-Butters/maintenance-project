#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <assert.h>
extern char *strdup(const char *);

/**
 * @file common.h
 * @author Erdem Garip
 * @date 15 Nov 2022
 * @brief Common utility functions
 *  
 */

typedef union
{
    int int_value;
    float float_value;
    char *string_value;
} answer_t;

typedef bool (*check_func)(char *);
typedef answer_t (*convert_func)(char *);
int read_string(char *buf, int buf_siz);

#define empty_elem() \
    (elem_t) { .__dummy = 0 }
#define bool_elem(x) \
    (elem_t) { .b = (x) }
#define int_elem(x) \
    (elem_t) { .i = (x) }
#define float_elem(x) \
    (elem_t) { .f = (x) }
#define ptr_elem(x) \
    (elem_t) { .p = (x) }
#define size_elem(x) \
    (elem_t) { .s = (x) }

typedef union elem elem_t;
union elem
{
    bool __dummy; // dummy value not to be used
    int i;        // integer value
    bool b;       // boolean value
    float f;      // float value
    void *p;      // pointer value
    size_t s;     // size value
    char *str;
};

/// Compares two elements and returns true if they are equal
typedef bool (*ioopm_eq_function)(elem_t a, elem_t b);
typedef bool (*ioopm_predicate)(elem_t key, elem_t value, void *extra);
typedef void (*ioopm_apply_function)(elem_t key, elem_t *value, void *extra);

/// @brief compare two elem_t's bool value
/// @param a the first elem_t
/// @param b the second elem_t
/// @return true if the bools of a and b are equal
bool ioopm_compare_bool_elems(elem_t a, elem_t b);

/// @brief compare two elem_t's int value
/// @param a the first elem_t
/// @param b the second elem_t
/// @return true if the ints of a and b are equal
bool ioopm_compare_int_elems(elem_t a, elem_t b);

/// @brief compare two elem_t's float value
/// @param a the first elem_t
/// @param b the second elem_t
/// @return true if the floats of a and b are equal
bool ioopm_compare_float_elems(elem_t a, elem_t b);

/// @brief compare two elem_t's void* value
/// @param a the first elem_t
/// @param b the second elem_t
/// @return true if the pointers of a and b are equal
bool ioopm_compare_ptr_elems(elem_t a, elem_t b);

/// @brief compare two elem_t's size_t value
/// @param a the first elem_t
/// @param b the second elem_t
/// @return true if the sizes of a and b are equal
bool ioopm_compare_size_elems(elem_t a, elem_t b);

/// @brief Checks if the string value of two elems are equal
/// @param a the first elem_t
/// @param b the second elem_t
/// @return true if the string values of a and b are equal
bool key_equiv(elem_t a, elem_t b);

/// @brief Checks if the int value of two elems are equal
/// @param a the first elem_t
/// @param b the second elem_t
/// @return true if the ints of a and b are equal
bool value_equiv(elem_t a, elem_t b);

/// @brief Generic ask_question function that prompts a question as long as check is not satisfied
/// @param  question The question prompted to user
/// @param  check The function that checks if the answer is valid
/// @param  convert The function that converts the answer to a data type
/// @return A value of converted type
answer_t ask_question(char *question, check_func check, convert_func convert);

/// @brief A function that prompts the user to input an int
/// @param  question The question prompted to user
/// @return An int
int ask_question_int(char *question);

/// @brief A function that prompts the user to input a string
/// @param  question The question prompted to user
/// @return A string
char *ask_question_string(char *question);

/// @brief A function that prompts the user to input a Shelf
/// @param  question The question prompted to user
/// @return A shelf
char *ask_question_shelf(char *question);

/// @brief Checks if an user input is a positive number
/// @param int the number
/// @return True if the number is positive
bool is_positive(int num);

/// @brief Checks if an user input is a number
/// @param str The raw string
/// @return True if the processed string is an int
bool is_valid_number(char *str);

/// @brief Checks if an user input is a is valid
/// @param str The raw string
/// @return True if the processed string does not entirely consist of whitespace or new line character
bool is_valid_string(char *str);

/// @brief Checks if an user input is a Shelf
/// @param shelf The raw string
/// @return True if the processed shelf is of size 3 where first element is alpha numeric and latter two are numbers
bool is_valid_shelf(char *shelf);
