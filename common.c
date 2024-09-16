#include "common.h"

bool ioopm_compare_bool_elems(elem_t a, elem_t b)
{
  return a.b == b.b;
}

bool ioopm_compare_int_elems(elem_t a, elem_t b)
{
  return a.i == b.i;
}

bool ioopm_compare_float_elems(elem_t a, elem_t b)
{
  return a.f == b.f;
}

bool ioopm_compare_ptr_elems(elem_t a, elem_t b)
{
  return a.p == b.p;
}

bool ioopm_compare_size_elems(elem_t a, elem_t b)
{
  return a.s == b.s;
}

bool key_equiv(elem_t a, elem_t b)
{
  if (strcmp(a.p, b.p) == 0)
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool value_equiv(elem_t a, elem_t b)
{
  if (a.i == b.i)
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool valid_quantity(int choice, int max)
{
  if (choice > max || choice < 0)
  {
    return false;
  }
  else
  {
    return true;
  }
}

answer_t ask_question(char *question, check_func check, convert_func convert)
{
  int buffer_size = 255;
  char buffer[buffer_size];

  do
  {
    printf("%s\n", question);
    read_string(buffer, buffer_size);

  } while (!check(buffer));

  return convert(buffer);
}

int ask_question_int(char *question)
{
  answer_t answer = ask_question(question, is_valid_number, (convert_func)atoi);
  return answer.int_value;
}

char *ask_question_string(char *question)
{
  return ask_question(question, is_valid_string, (convert_func)strdup).string_value;
}

char *ask_question_shelf(char *question)
{
  return ask_question(question, is_valid_shelf, (convert_func)strdup).string_value;
}

bool is_positive(int num)
{
  if(num > 0)
  {
    return true;
  }
  else 
  {
    return false;
  }
}

bool is_valid_number(char *str)
{

  if (str[0] == '-')
  {
    for (int i = 1; i < strlen(str); i++)
    {
      if (!isdigit(str[i]))
      {
        return false;
      }
    }
  }
  else
  {
    for (int i = 0; i < strlen(str); i++)
    {
      if (!isdigit(str[i]))
      {
        return false;
      }
    }
  }
  return true;
}

bool is_valid_string(char *str)
{
  while (*str)
  {
    if (!isspace(*str) && *str != '\n' && *str && strlen(str) > 0)
    {
      return true;
    }
    str++;
  }
  return false;
}

bool is_valid_shelf(char *shelf)
{
  int size = strlen(shelf);

  if (size == 3 && isalpha(shelf[0]) && isupper(shelf[0]) && isdigit(shelf[1]) && isdigit(shelf[2]))
  {
    return true;
  }
  else
  {
    return false;
  }
}

int read_string(char *buf, int buf_siz)
{
  int c = getchar();
  int counter = 0;
  do
  {
    if (counter < buf_siz - 1)
    {
      buf[counter++] = c;
      c = getchar();
    }

  } while (c != '\n' && c != EOF);

  buf[counter] = '\0';

  return counter;
}
