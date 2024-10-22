#include "backend.h"
#include "common.h"
#include "hash_table.h"
#include "linked_list.h"

// ### Internal ###

struct merch
{
  char *name;
  char *desc;
  int price;
  ioopm_list_t *locations;
  char *category;
  char *color;
  char *brand;
};

struct webstore
{
  ioopm_hash_table_t *merchs;
  ioopm_hash_table_t *carts;
  ioopm_hash_table_t *categories;
  int cart_id;
  int cart_quantity;
};

struct shelf
{
  char *name;
  int quantity;
};

struct shopping_carts
{
  ioopm_hash_table_t *shopping_cart;
};

int string_hash(elem_t e)
{
  char *str = e.p;
  int result = 0;
  do
  {
    result += *str;
  } while (*++str != '\0');
  return result;
}

// ### Internal ###
void db_list_all_categories_in_webstore(webstore_t *db)
{
  ioopm_list_t *categories_list = ioopm_hash_table_keys(db->categories);

  int size = (int)ioopm_linked_list_size(categories_list);

  printf("--------------------\n");
  printf("Categories in the webstore:\n");

  for (int i = 0; i < size; i++)
  {
    char *category = ioopm_linked_list_get(categories_list, i).p;
    printf("Category %d = %s\n", i + 1, category);
  }

  ioopm_linked_list_destroy(categories_list);
}

int db_number_of_categories_in_webstore(webstore_t *db)
{
  return (int)ioopm_hash_table_size(db->categories);
}

char *db_get_category_from_webstore(webstore_t *db, int category_choice)
{

  ioopm_list_t *categories_list = ioopm_hash_table_keys(db->categories);

  char *category = ioopm_linked_list_get(categories_list, category_choice).p;

  ioopm_linked_list_destroy(categories_list);

  char *result = category;

  return result;
}

webstore_t *db_create_webstore(void)
{
  webstore_t *db = calloc(1, sizeof(webstore_t));

  db->merchs =
      ioopm_hash_table_create(key_equiv, ioopm_compare_ptr_elems, string_hash);
  db->carts = ioopm_hash_table_create(ioopm_compare_int_elems, NULL, NULL);
  db->cart_id = 1;
  db->categories =
      ioopm_hash_table_create(key_equiv, ioopm_compare_ptr_elems, string_hash);

  return db;
}

merch_t *db_create_merch(char *name, char *desc, int price, char *category, char *brand, char *color)
{
  merch_t *new_merch = calloc(1, sizeof(merch_t));

  new_merch->name = name;
  new_merch->desc = desc;
  new_merch->price = price;
  new_merch->category = category;
  new_merch->brand = brand;
  new_merch->color = color;
  new_merch->locations = ioopm_linked_list_create(ioopm_compare_ptr_elems);

  return new_merch;
}

shelf_t *db_create_shelf(char *shelf_name, int shelf_quantity)
{
  shelf_t *new_shelf = calloc(1, sizeof(shelf_t));

  new_shelf->name = shelf_name;
  new_shelf->quantity = shelf_quantity;

  return new_shelf;
}

shopping_carts_t *db_create_cart(void)
{
  shopping_carts_t *new_cart = calloc(1, sizeof(shopping_carts_t));

  new_cart->shopping_cart = ioopm_hash_table_create(
      ioopm_compare_ptr_elems, ioopm_compare_int_elems, string_hash);

  return new_cart;
}

void db_destroy_a_merch(merch_t *merch)
{
  free(merch->name);
  free(merch->desc);

  // TODO: Fixa detta så att det för varje sak sker max en free(merch->category);

  int size = (int)ioopm_linked_list_size(merch->locations);

  for (int i = 0; i < size; i++)
  {
    shelf_t *location = ioopm_linked_list_get(merch->locations, i).p;
    free(location->name);
    free(location);
  }

  ioopm_linked_list_destroy(merch->locations);
  free(merch);
}

bool db_has_key(webstore_t *db, char *name)
{
  if (ioopm_hash_table_has_key(db->merchs, ptr_elem(name)))
  {
    return true;
  }
  else
  {
    return false;
  }
}

void db_add_merch(webstore_t *db, merch_t *merch)
{
  ioopm_hash_table_insert(db->merchs, ptr_elem(merch->name), ptr_elem(merch));
  ioopm_hash_table_insert(db->categories, ptr_elem(merch->category),
                          ptr_elem(merch));
}

int db_merch_count(webstore_t *db)
{
  return ioopm_hash_table_size(db->merchs);
}

void db_list_merchs(webstore_t *db)
{
  ioopm_list_t *merchs_list = ioopm_hash_table_values(db->merchs);
  int size = (int)ioopm_linked_list_size(merchs_list);

  for (int i = 0; i < size; i++)
  {
    merch_t *merch = ioopm_linked_list_get(merchs_list, i).p;
    db_list_a_merch(merch);
  }

  ioopm_linked_list_destroy(merchs_list);
}

void db_list_a_merch(merch_t *merch)
{
  printf("\nMerchandise name: %s\n"
         "Merchandise description: %s\n"
         "Merchandise price: %d\n"
         "Merchandise brand: %s\n"
         "Merchandise category: %s\n"
         "Merchandise color(s): %s\n\n",
         merch->name, merch->desc, merch->price, merch->brand, merch->category, merch->color);
}

merch_t *db_get_merch_from_name(webstore_t *db, char *name)
{
  elem_t ptr;
  ioopm_hash_table_lookup(db->merchs, ptr_elem(name), &ptr);
  return ptr.p;
}

void db_remove_merch(webstore_t *db, char *name)
{
  // TODO: This function only removes from db->merchs but it should also remove
  //  the merchandise from the carts also
  elem_t ptr;
  ioopm_hash_table_remove(db->merchs, ptr_elem(name), &ptr);

  ioopm_list_t *carts_list = ioopm_hash_table_values(db->carts);
  int size = (int)ioopm_linked_list_size(carts_list);

  for (int i = 0; i < size; i++)
  {
    shopping_carts_t *cart = ioopm_linked_list_get(carts_list, i).p;
    db_remove_merch_from_cart(cart, name);
  }

  db_destroy_a_merch(ptr.p);
  ioopm_linked_list_destroy(carts_list);
}

void db_remove_merch_from_cart(shopping_carts_t *cart, char *name)
{
  elem_t ptr;
  ioopm_hash_table_remove(cart->shopping_cart, ptr_elem(name), &ptr);
  ptr.i = 0;
}

merch_t *
db_edit_merch(webstore_t *db, merch_t *current_merch, char *new_name, char *new_brand, char *new_category, char *new_color, char *new_desc, int new_price)
{
  merch_t *edited_merch = calloc(1, sizeof(merch_t));

  edited_merch->name = new_name;
  edited_merch->desc = new_desc;
  edited_merch->brand = new_brand;
  edited_merch->category = new_category;
  edited_merch->color = new_color;
  edited_merch->price = new_price;
  edited_merch->locations = ioopm_linked_list_create(ioopm_compare_ptr_elems);

  int size = (int)ioopm_linked_list_size(current_merch->locations);

  for (int i = 0; i < size; i++)
  {
    shelf_t *shelf = ioopm_linked_list_get(current_merch->locations, i).p;
    ioopm_linked_list_append(edited_merch->locations, ptr_elem(shelf));
  }

  return edited_merch;
}

merch_t *db_get_merch(webstore_t *db, char *name)
{
  elem_t merch_ptr;
  ioopm_hash_table_lookup(db->merchs, ptr_elem(name), &merch_ptr);

  return merch_ptr.p;
}

int db_merch_locations_size(merch_t *merch)
{
  return (int)ioopm_linked_list_size(merch->locations);
}

void db_display_stock(merch_t *merch)
{
  int size = (int)ioopm_linked_list_size(merch->locations);

  if (size == 0)
  {
    printf("--- The merch does not have any locations yet. ---\n");
  }
  else
  {
    for (int i = 0; i < size; i++)
    {
      shelf_t *shelf = ioopm_linked_list_get(merch->locations, i).p;
      printf("Shelf name: %s | Shelf quantity: %d \n", shelf->name, shelf->quantity);
    }
  }
}

bool db_location_name_exists_in_webstore(webstore_t *db, char *shelf_name)
{
  ioopm_list_t *merchs_list = ioopm_hash_table_values(db->merchs);
  int merchs_size = (int)ioopm_linked_list_size(merchs_list);

  for (int i = 0; i < merchs_size; i++)
  {
    merch_t *merch = ioopm_linked_list_get(merchs_list, i).p;
    int locations_size = ioopm_linked_list_size(merch->locations);

    for (int j = 0; j < locations_size; j++)
    {
      shelf_t *location = ioopm_linked_list_get(merch->locations, i).p;

      if (strcmp(location->name, shelf_name) == 0)
      {
        ioopm_linked_list_destroy(merchs_list); // hade glömt det här
        return true;
      }
    }
  }
  ioopm_linked_list_destroy(merchs_list);
  return false;
}

void db_add_location_to_merch(merch_t *merch, char *shelf_name, int new_quantity)
{
  shelf_t *shelf = db_create_shelf(shelf_name, new_quantity);
  ioopm_linked_list_append(merch->locations, ptr_elem(shelf));
}

bool db_edit_location_quantity(merch_t *merch, char *shelf_name, int new_quantity)
{
  int size = (int)ioopm_linked_list_size(merch->locations);

  for (int i = 0; i < size; i++)
  {
    shelf_t *location = ioopm_linked_list_get(merch->locations, i).p;

    if (strcmp(location->name, shelf_name) == 0)
    {
      location->quantity = new_quantity;
      return true;
    }
  }
  return false;
}

void db_add_cart(webstore_t *db, shopping_carts_t *cart)
{
  ioopm_hash_table_insert(db->carts, int_elem(db->cart_id), ptr_elem(cart));
  db->cart_id++;
  db->cart_quantity++;
}

int db_carts_size(webstore_t *db)
{
  return (int)ioopm_hash_table_size(db->carts);
}

bool db_remove_cart(webstore_t *db, int id_choice)
{
  elem_t cart_ptr;
  bool result = ioopm_hash_table_remove(db->carts, int_elem(id_choice), &cart_ptr);

  if (result)
  {
    db_destroy_a_cart(cart_ptr.p);
    db->cart_quantity--;
  }
  return result;
}

void db_display_cart_ids(webstore_t *db)
{
  ioopm_list_t *carts_id_list = ioopm_hash_table_keys(db->carts);

  int size = (int)ioopm_linked_list_size(carts_id_list);

  for (int i = 0; i < size; i++)
  {
    int cart_id = ioopm_linked_list_get(carts_id_list, i).i;
    printf("Cart id: %d\n", cart_id);
  }

  ioopm_linked_list_destroy(carts_id_list);
}

shopping_carts_t *db_get_cart_from_id(webstore_t *db, int id_choice)
{
  elem_t cart_ptr;
  bool lookup = ioopm_hash_table_lookup(db->carts, int_elem(id_choice), &cart_ptr);

  if (lookup)
  {
    return cart_ptr.p;
  }
  else
  {
    return NULL;
  }
}

int db_lookup_merch_quantity_in_locations(merch_t *merch)
{
  int size = (int)ioopm_linked_list_size(merch->locations);
  int counter = 0;

  for (int i = 0; i < size; i++)
  {
    shelf_t *location = ioopm_linked_list_get(merch->locations, i).p;
    counter = counter + location->quantity;
  }
  return counter;
}

int db_lookup_valid_quantity(webstore_t *db, merch_t *merch, char *merch_name)
{
  int quantity_in_locations = db_lookup_merch_quantity_in_locations(merch);
  int quantity_in_carts = db_lookup_merch_quantity_in_carts(db, merch_name);

  return quantity_in_locations - quantity_in_carts;
}

bool db_cart_has_key(shopping_carts_t *cart, char *merch_name)
{
  // elem_t quantity_ptr;
  // return ioopm_hash_table_lookup(cart->shopping_cart, ptr_elem(merch_name), &quantity_ptr);

  ioopm_list_t *names = ioopm_hash_table_keys(cart->shopping_cart);
  int size = (int)ioopm_linked_list_size(names);

  for (int i = 0; i < size; i++)
  {
    if (strcmp(merch_name, ioopm_linked_list_get(names, i).str) == 0)
    {
      return true;
    }
  }
  return false;
}

void db_update_merch_quantity_in_cart(shopping_carts_t *cart, char *merch_name, int new_quantity)
{
  elem_t quantity_ptr;
  ioopm_hash_table_remove(cart->shopping_cart, ptr_elem(merch_name), &quantity_ptr);

  int sum_quantity = quantity_ptr.i + new_quantity;

  ioopm_hash_table_insert(cart->shopping_cart, ptr_elem(merch_name), int_elem(sum_quantity));
}

void db_add_merch_to_cart(shopping_carts_t *cart, char *merch_name, int new_quantity)
{
  ioopm_hash_table_insert(cart->shopping_cart, ptr_elem(merch_name), int_elem(new_quantity));
}

int db_lookup_merch_quantity_in_carts2(webstore_t *db, char *merch_name)
{
  ioopm_list_t *carts_list = ioopm_hash_table_values(db->carts);
  int size = (int)ioopm_linked_list_size(carts_list);
  int counter = 0;

  for (int i = 0; i < size; i++)
  {
    shopping_carts_t *cart = ioopm_linked_list_get(carts_list, i).p;
    elem_t quantity_ptr;
    bool lookup = ioopm_hash_table_lookup(cart->shopping_cart, ptr_elem(merch_name), &quantity_ptr);

    if (lookup)
    {
      counter = counter + quantity_ptr.i;
    }
  }
  ioopm_linked_list_destroy(carts_list);
  return counter;
}

int db_lookup_merch_quantity_in_carts(webstore_t *db, char *merch_name)
{
  ioopm_list_t *carts_list = ioopm_hash_table_values(db->carts);
  int carts_size = (int)ioopm_linked_list_size(carts_list);
  int counter = 0;

  for (int i = 0; i < carts_size; i++)
  {
    shopping_carts_t *cart = ioopm_linked_list_get(carts_list, i).p;

    ioopm_list_t *names = ioopm_hash_table_keys(cart->shopping_cart);
    ioopm_list_t *quantities = ioopm_hash_table_values(cart->shopping_cart);
    int size = (int)ioopm_linked_list_size(names);

    for (int i = 0; i < size; i++)
    {
      if (strcmp(merch_name, ioopm_linked_list_get(names, i).p) == 0)
      {
        counter = counter + ioopm_linked_list_get(quantities, i).i;
      }
    }
    ioopm_linked_list_destroy(names);
    ioopm_linked_list_destroy(quantities);
  }
  ioopm_linked_list_destroy(carts_list);
  return counter;
}

int db_calculate_cost(webstore_t *db, shopping_carts_t *cart)
{
  ioopm_list_t *names_list = ioopm_hash_table_keys(cart->shopping_cart);
  int names_size = (int)ioopm_linked_list_size(names_list);
  ioopm_list_t *quantities_list = ioopm_hash_table_values(cart->shopping_cart);
  int counter = 0;

  for (int i = 0; i < names_size; i++)
  {
    char *merch_name = ioopm_linked_list_get(names_list, i).p;
    merch_t *merch = db_get_merch_from_name(db, merch_name);
    int merch_quantity = ioopm_linked_list_get(quantities_list, i).i;
    int merch_price = db_get_price(merch);

    counter = counter + merch_price * merch_quantity;
  }
  ioopm_linked_list_destroy(names_list);
  ioopm_linked_list_destroy(quantities_list);

  return counter;
}

void db_checkout(webstore_t *db, shopping_carts_t *cart)
{
  int cart_size = (int)ioopm_hash_table_size(cart->shopping_cart);
  ioopm_list_t *names_list = ioopm_hash_table_keys(cart->shopping_cart);
  for (int i = 0; i < cart_size; i++)
  {
    elem_t merch_name = ioopm_linked_list_get(names_list, i);
    elem_t quantity_ptr;
    bool lookup = ioopm_hash_table_lookup(cart->shopping_cart, merch_name, &quantity_ptr);

    if (lookup)
    {
      merch_t *merch = db_get_merch_from_name(db, merch_name.p);
      db_decrease_locations_quantities(merch, quantity_ptr.i);
    }
  }
  ioopm_linked_list_destroy(names_list);
}

void db_decrease_locations_quantities(merch_t *merch, int quantity)
{
  for (int i = 0; quantity > 0; i++)
  {
    shelf_t *location = ioopm_linked_list_get(merch->locations, i).p;

    if (quantity > location->quantity)
    {
      quantity = quantity - location->quantity;
      location->quantity = 0;
    }
    else
    { // if(location->quantity > quantity)
      location->quantity = location->quantity - quantity;
      quantity = 0;
    }
  }
}

//
//
//
void db_destroy_webstore(webstore_t *db)
{
  db_destroy_merchs(db->merchs);
  db_destroy_carts(db->carts);
  free(db); // hade glömt det här, orsakde 16b stillreachable
}

void db_destroy_merchs(ioopm_hash_table_t *merchs)
{
  ioopm_list_t *merchs_list = ioopm_hash_table_values(merchs);
  int size = (int)ioopm_linked_list_size(merchs_list);

  for (int i = 0; i < size; i++)
  {
    merch_t *merch = ioopm_linked_list_get(merchs_list, i).p;
    db_destroy_a_merch(merch);
  }

  ioopm_linked_list_destroy(merchs_list);
  ioopm_hash_table_destroy(
      merchs); // Hade glömt det här, orsakde still reachable error i valgrind
}

void db_destroy_carts(ioopm_hash_table_t *carts)
{
  ioopm_list_t *carts_list = ioopm_hash_table_values(carts);
  int size = (int)ioopm_linked_list_size(carts_list);

  for (int i = 0; i < size; i++)
  {
    shopping_carts_t *cart = ioopm_linked_list_get(carts_list, i).p;
    db_destroy_a_cart(cart);
  }

  ioopm_linked_list_destroy(carts_list);
  ioopm_hash_table_destroy(carts);
}

void db_destroy_a_cart(shopping_carts_t *cart)
{
  ioopm_hash_table_destroy(cart->shopping_cart);
  free(cart);
}

// ### functions for testing ###

char *db_get_name(merch_t *merch)
{
  return merch->name;
}

char *db_get_desc(merch_t *merch)
{
  return merch->desc;
}

int db_get_price(merch_t *merch)
{
  return merch->price;
}

int db_get_merch_location_quantity(merch_t *merch, char *shelf_name)
{
  int size = (int)ioopm_linked_list_size(merch->locations);

  for (int i = 0; i < size; i++)
  {
    shelf_t *location = ioopm_linked_list_get(merch->locations, i).p;

    if (strcmp(location->name, shelf_name) == 0)
    {
      return location->quantity;
    }
  }
  return -1;
}

int db_get_merch_quantity_in_a_cart(shopping_carts_t *cart, char *merch_name)
{
  elem_t quantity_ptr;
  bool lookup = ioopm_hash_table_lookup(cart->shopping_cart,
                                        ptr_elem(merch_name), &quantity_ptr);

  if (lookup)
  {
    return quantity_ptr.i;
  }
  else
  {
    return 0;
  }
}
