#include <CUnit/Basic.h>

#include "backend.h"
#include "common.h"
#include "frontend.h"
#include "hash_table.h"
#include "iterator.h"
#include "linked_list.h"

char *ioopm_strdup(char *str);
char *ioopm_strdup(char *str)
{
  size_t len = strlen(str);
  char *result = calloc(len + 1, sizeof(char));
  strncpy(result, str, len);
  return result;
}

void test1_common_functions(void)
{
  char *valid_arr[5] = {"a", "A", "adidas", "Adidas", "helle kelle"};
  char *invalid_1 = "\n";
  char *invalid_2 = "\0";
  char *invalid_3 = "";
  char *invalid_4 = " ";

  for (int i = 0; i < 4; i++)
  {
    CU_ASSERT_TRUE(is_valid_string(valid_arr[i]));
  }

  CU_ASSERT_FALSE(is_valid_string(invalid_1));
  CU_ASSERT_FALSE(is_valid_string(invalid_2));
  CU_ASSERT_FALSE(is_valid_string(invalid_3));
  CU_ASSERT_FALSE(is_valid_string(invalid_4));

  char *valid_shelf[6] = {"A01", "A00", "B25", "B99", "Z12", "D90"};
  char *invalid_shelf[6] = {"", " ", "\n", "A111", "A-01", "A1"};

  for (int i = 0; i < 5; i++)
  {
    CU_ASSERT_TRUE(is_valid_shelf(valid_shelf[i]));
    CU_ASSERT_FALSE(is_valid_shelf(invalid_shelf[i]));
  }

  char *valid_number[6] = {"1", "5", "199", "200", "2", "3"};
  char *number_but_invalid[6] = {"-1", "0", "-99", "-77", "-66", "-61"};
  char *invalid_number[6] = {"-1A", "-1a", "--1", "a1", "a1", "aa"};

  for (int i = 0; i < 3; i++)
  {
    CU_ASSERT_TRUE(is_valid_number(valid_number[i]));
    CU_ASSERT_TRUE(is_valid_number(number_but_invalid[i]));
    CU_ASSERT_FALSE(is_valid_number(invalid_number[i]));
  }

  CU_ASSERT_FALSE(is_positive(0));
  CU_ASSERT_TRUE(is_positive(1));
  CU_ASSERT_TRUE(is_positive(99));
  CU_ASSERT_FALSE(is_positive(0.5));
}

void test2_create_and_destroy(void)
{
  webstore_t *db = db_create_webstore();
  CU_ASSERT_PTR_NOT_NULL(db);
  db_destroy_webstore(db);
}

merch_t *create_merch()
{
  return db_create_merch(ioopm_strdup("adidas"), ioopm_strdup("bra skor"), 100, ioopm_strdup("adidas"), ioopm_strdup("shoes"), ioopm_strdup("black"));
}

void test3_create_and_edit_merch(void)
{
  webstore_t *db = db_create_webstore();

  // Jag vet inte varför men det uppstår problem utan strdup. TODO: fråga om det här
  char *name = ioopm_strdup("power4x runner");
  char *desc = ioopm_strdup("good shoes");
  int price = 100;
  char *brand = ioopm_strdup("adidas");
  char *category = ioopm_strdup("shoes");
  char *color = ioopm_strdup("color");
  merch_t *adidas = db_create_merch(name, desc, price, category, brand, color);

  CU_ASSERT_PTR_NOT_NULL(adidas);
  CU_ASSERT_STRING_EQUAL(db_get_name(adidas), name);
  CU_ASSERT_STRING_EQUAL(db_get_desc(adidas), desc);
  CU_ASSERT_EQUAL(db_get_price(adidas), price);
  CU_ASSERT_STRING_EQUAL(db_get_brand(adidas), brand);
  CU_ASSERT_STRING_EQUAL(db_get_category(adidas), category);
  CU_ASSERT_STRING_EQUAL(db_get_color(adidas), color);

  char *new_nameA = ioopm_strdup("nixe");
  char *new_descA = ioopm_strdup("byxor");
  char *new_brand = ioopm_strdup("nixe");
  char *new_category = ioopm_strdup("shoes");
  char *new_color = ioopm_strdup("black");
  int new_priceA = 55;
  merch_t *nixe = db_edit_merch(db, adidas, new_nameA, new_brand, new_category, new_color, new_descA, new_priceA);

  CU_ASSERT_STRING_EQUAL(db_get_name(nixe), new_nameA);
  CU_ASSERT_STRING_EQUAL(db_get_desc(nixe), new_descA);
  CU_ASSERT_EQUAL(db_get_price(nixe), new_priceA);
  CU_ASSERT_STRING_EQUAL(db_get_brand(nixe), new_brand);
  CU_ASSERT_STRING_EQUAL(db_get_category(nixe), new_category);
  CU_ASSERT_STRING_EQUAL(db_get_color(nixe), new_color);

  db_destroy_a_merch(adidas);
  db_destroy_webstore(db);
}

void test4_remove_merch(void)
{
  webstore_t *db = db_create_webstore();

  char *nameA = ioopm_strdup("adidas");
  merch_t *adidas = create_merch();

  db_add_merch(db, adidas);
  CU_ASSERT_TRUE(db_has_key(db, db_get_name(adidas)));
  CU_ASSERT_TRUE(db_has_key(db, nameA));

  db_remove_merch(db, db_get_name(adidas));
  // CU_ASSERT_FALSE(db_has_key(db, db_get_name(adidas)));
  // Ovanstående test ger invalid read eftersom db_get_name förutsätter att en merch existerar
  CU_ASSERT_FALSE(db_has_key(db, nameA));

  // TODO: add merch to cart

  db_destroy_webstore(db);
}

void test5_replenish_new(void)
{
  webstore_t *db = db_create_webstore();

  char *name = ioopm_strdup("power4x runner");
  char *desc = ioopm_strdup("good shoes");
  int price = 100;
  char *brand = ioopm_strdup("adidas");
  char *category = ioopm_strdup("shoes");
  char *color = ioopm_strdup("color");
  merch_t *adidas = db_create_merch(name, desc, price, category, brand, color);
  char *locationA = ioopm_strdup("A99");
  int location_quantityA = 1;

  db_add_merch(db, adidas);

  CU_ASSERT_TRUE(db_has_key(db, name));
  CU_ASSERT_FALSE(db_location_name_exists_in_webstore(db, locationA));

  db_add_location_to_merch(adidas, locationA, location_quantityA);
  CU_ASSERT_TRUE(db_location_name_exists_in_webstore(db, locationA));

  db_destroy_webstore(db);
}

void test6_replenish_edit(void)
{
  webstore_t *db = db_create_webstore();

  char *nameA = ioopm_strdup("power4x runner");
  char *descA = ioopm_strdup("good shoes");
  int priceA = 100;
  char *brandA = ioopm_strdup("adidas");
  char *categoryA = ioopm_strdup("shoes");
  char *colorA = ioopm_strdup("color");

  char *locationA = ioopm_strdup("A99");
  int location_quantityA = 1;
  char *location_does_not_exist = ioopm_strdup("B255");
  int dummy = 22;

  merch_t *adidas = db_create_merch(nameA, descA, priceA, categoryA, brandA, colorA);
  db_add_merch(db, adidas);

  CU_ASSERT_TRUE(db_has_key(db, nameA));
  CU_ASSERT_FALSE(db_location_name_exists_in_webstore(db, locationA));

  db_add_location_to_merch(adidas, locationA, location_quantityA);
  CU_ASSERT_TRUE(db_location_name_exists_in_webstore(db, locationA));

  CU_ASSERT_FALSE(db_location_name_exists_in_webstore(db, location_does_not_exist));
  CU_ASSERT_FALSE(db_edit_location_quantity(adidas, location_does_not_exist, dummy));
  CU_ASSERT_FALSE(db_location_name_exists_in_webstore(db, location_does_not_exist));

  char *nameB = ioopm_strdup("nixe");
  char *descB = ioopm_strdup("bra byx");
  int priceB = 9998;
  char *brandB = ioopm_strdup("adidas");
  char *categoryB = ioopm_strdup("shoes");
  char *colorB = ioopm_strdup("color");
  merch_t *nixe = db_create_merch(nameB, descB, priceB, categoryB, brandB, colorB);
  char *locationB = ioopm_strdup("A11");
  int quantityB = 11;
  int quantityBnew = 25;

  db_add_merch(db, nixe);

  CU_ASSERT_FALSE(db_edit_location_quantity(nixe, locationB, quantityB));
  CU_ASSERT_EQUAL(-1, db_get_merch_location_quantity(nixe, locationB));
  db_add_location_to_merch(nixe, locationB, quantityB);
  CU_ASSERT_EQUAL(11, db_get_merch_location_quantity(nixe, locationB));
  CU_ASSERT_TRUE(db_edit_location_quantity(nixe, locationB, quantityBnew));
  CU_ASSERT_EQUAL(25, db_get_merch_location_quantity(nixe, locationB));

  free(location_does_not_exist);
  // db_remove_merch(db, nameA);
  db_destroy_webstore(db);
}

void test7_create_carts(void)
{
  webstore_t *db = db_create_webstore();

  shopping_carts_t *cartA = db_create_cart();
  CU_ASSERT_EQUAL(0, db_carts_size(db));
  db_add_cart(db, cartA); // Eftersom denna är första så har den idn 1
  CU_ASSERT_EQUAL(1, db_carts_size(db));
  CU_ASSERT_FALSE(db_remove_cart(db, 0)); // False eftersom id inte existerar
  CU_ASSERT_FALSE(db_remove_cart(db, 2));

  // När jag först anropade den här funktionen så tog den bort cart:en från hash tabellen men den friade aldrig
  // minnet som allokerades av strukten. Eftersom den var borttaget från hash tabellen så kunde den inte hittas
  // i destroy_webstore.
  // Lösningen var att lägga till db_destroy_a_cart i db_remove_cart
  CU_ASSERT_TRUE(db_remove_cart(db, 1));

  db_destroy_webstore(db);
}

void test8_add_and_remove_cart(void)
{
  webstore_t *db = db_create_webstore();

  shopping_carts_t *cartA = db_create_cart();
  db_add_cart(db, cartA);

  char *nameA = ioopm_strdup("adidas");
  char *descB = ioopm_strdup("bra skor");
  int priceB = 100;
  char *brandB = ioopm_strdup("adidas");
  char *categoryB = ioopm_strdup("shoes");
  char *colorB = ioopm_strdup("color");

  char *locationA1 = ioopm_strdup("A01");
  int location_quantityA1 = 200;
  char *locationA2 = ioopm_strdup("A31");
  int location_quantityA2 = 100;
  merch_t *adidas = db_create_merch(nameA, descB, priceB, categoryB, brandB, colorB);
  db_add_merch(db, adidas);
  db_add_location_to_merch(adidas, locationA1, location_quantityA1);
  db_add_location_to_merch(adidas, locationA2, location_quantityA2);
  // Nu borde det vara så här:
  //  adidas -> (adidas, bra skor, 100) locations -> ([A01, 200], [A31, 100])
  // Totalt 300 exemplar i två shelf

  int lookup_cart_1a = db_get_merch_quantity_in_a_cart(cartA, nameA);
  int lookup_cart_1b = db_lookup_merch_quantity_in_locations(adidas);
  CU_ASSERT_EQUAL(0, lookup_cart_1a);
  CU_ASSERT_EQUAL(300, lookup_cart_1b);

  db_add_merch_to_cart(cartA, nameA, 250);
  int lookup_cart_2a = db_get_merch_quantity_in_a_cart(cartA, nameA);
  int lookup_cart_2b = db_lookup_merch_quantity_in_locations(adidas);
  CU_ASSERT_EQUAL(300, lookup_cart_2b);
  CU_ASSERT_EQUAL(250, lookup_cart_2a);

  db_remove_merch_from_cart(cartA, nameA);
  int lookup_cart_3a = db_get_merch_quantity_in_a_cart(cartA, nameA);
  CU_ASSERT_EQUAL(0, lookup_cart_3a);

  db_destroy_webstore(db);
}

void test9_calculate_cost(void)
{
  webstore_t *db = db_create_webstore();

  shopping_carts_t *cartA = db_create_cart();
  db_add_cart(db, cartA);
  shopping_carts_t *cartB = db_create_cart();
  db_add_cart(db, cartB);

  char *nameA = ioopm_strdup("adidas");
  char *descA = ioopm_strdup("bra skor");
  int priceA = 100;
  char *brandA = ioopm_strdup("adidas");
  char *categoryA = ioopm_strdup("shoes");
  char *colorA = ioopm_strdup("color");
  merch_t *adidas = db_create_merch(nameA, descA, priceA, categoryA, brandA, colorA);

  char *locationA1 = ioopm_strdup("A01");
  int location_quantityA1 = 200;
  char *locationA2 = ioopm_strdup("A31");
  int location_quantityA2 = 100;
  db_add_merch(db, adidas);
  db_add_location_to_merch(adidas, locationA1, location_quantityA1);
  db_add_location_to_merch(adidas, locationA2, location_quantityA2);

  CU_ASSERT_EQUAL(0, db_calculate_cost(db, cartA));
  db_add_merch_to_cart(cartA, nameA, 2);
  CU_ASSERT_EQUAL(200, db_calculate_cost(db, cartA));

  db_destroy_webstore(db);
}

void test10_checkout(void)
{
  webstore_t *db = db_create_webstore();

  shopping_carts_t *cartA = db_create_cart();
  db_add_cart(db, cartA);
  shopping_carts_t *cartB = db_create_cart();
  db_add_cart(db, cartB);

  char *nameA = ioopm_strdup("adidas");
  char *descA = ioopm_strdup("bra skor");
  int priceA = 100;
  char *brandA = ioopm_strdup("adidas");
  char *categoryA = ioopm_strdup("shoes");
  char *colorA = ioopm_strdup("color");

  char *locationA1 = ioopm_strdup("A01");
  int location_quantityA1 = 200;
  char *locationA2 = ioopm_strdup("A31");
  int location_quantityA2 = 50;
  char *locationA3 = ioopm_strdup("C88");
  int location_quantityA3 = 80;
  merch_t *adidas = db_create_merch(nameA, descA, priceA, categoryA, brandA, colorA);

  db_add_merch(db, adidas);
  db_add_location_to_merch(adidas, locationA1, location_quantityA1);
  db_add_location_to_merch(adidas, locationA2, location_quantityA2);
  db_add_location_to_merch(adidas, locationA3, location_quantityA3);

  CU_ASSERT_PTR_NOT_NULL(cartA);
  CU_ASSERT_EQUAL(db_lookup_merch_quantity_in_locations(adidas), 330);
  db_add_merch_to_cart(cartA, nameA, 100);
  CU_ASSERT_EQUAL(100, db_get_merch_quantity_in_a_cart(cartA, nameA));
  db_update_merch_quantity_in_cart(cartA, nameA, 160);
  CU_ASSERT_EQUAL(260, db_get_merch_quantity_in_a_cart(cartA, nameA));
  CU_ASSERT_EQUAL(db_get_merch_location_quantity(adidas, "A01"), 200);
  CU_ASSERT_EQUAL(db_get_merch_location_quantity(adidas, "A31"), 50);
  CU_ASSERT_EQUAL(db_get_merch_location_quantity(adidas, "C88"), 80);

  db_checkout(db, cartA);
  CU_ASSERT_EQUAL(db_get_merch_location_quantity(adidas, "A01"), 0);
  CU_ASSERT_EQUAL(db_get_merch_location_quantity(adidas, "A31"), 0);
  CU_ASSERT_EQUAL(db_get_merch_location_quantity(adidas, "C88"), 70);
  CU_ASSERT_EQUAL(db_lookup_merch_quantity_in_locations(adidas), 70);

  db_destroy_webstore(db);
}

void test11_checkout_without_update(void)
{
  webstore_t *db = db_create_webstore();

  shopping_carts_t *cartA = db_create_cart();
  db_add_cart(db, cartA);

  char *nameA = ioopm_strdup("adidas");
  char *descA = ioopm_strdup("bra skor");
  int priceA = 100;
  char *brandA = ioopm_strdup("adidas");
  char *categoryA = ioopm_strdup("shoes");
  char *colorA = ioopm_strdup("color");

  char *locationA1 = ioopm_strdup("A01");
  int location_quantityA1 = 200;
  char *locationA2 = ioopm_strdup("A31");
  int location_quantityA2 = 100;
  merch_t *adidas = db_create_merch(nameA, descA, priceA, categoryA, brandA, colorA);

  db_add_merch(db, adidas);
  db_add_location_to_merch(adidas, locationA1, location_quantityA1);
  db_add_location_to_merch(adidas, locationA2, location_quantityA2);

  CU_ASSERT_EQUAL(db_lookup_merch_quantity_in_locations(adidas), 300);
  db_add_merch_to_cart(cartA, nameA, 230);
  CU_ASSERT_EQUAL(db_get_merch_location_quantity(adidas, "A01"), 200);
  CU_ASSERT_EQUAL(db_get_merch_location_quantity(adidas, "A31"), 100);

  db_checkout(db, cartA);
  CU_ASSERT_EQUAL(db_lookup_merch_quantity_in_locations(adidas), 70);
  CU_ASSERT_EQUAL(db_get_merch_location_quantity(adidas, "A01"), 0);
  CU_ASSERT_EQUAL(db_get_merch_location_quantity(adidas, "A31"), 70);

  db_destroy_webstore(db);
}

void test12_checkout_with_update(void)
{
  webstore_t *db = db_create_webstore();

  shopping_carts_t *cartA = db_create_cart();
  db_add_cart(db, cartA);
  shopping_carts_t *cartB = db_create_cart();
  db_add_cart(db, cartB);

  char *nameA = ioopm_strdup("adidas");
  char *descA = ioopm_strdup("bra skor");
  int priceA = 100;
  char *brandA = ioopm_strdup("adidas");
  char *categoryA = ioopm_strdup("shoes");
  char *colorA = ioopm_strdup("color");
  merch_t *adidas = db_create_merch(nameA, descA, priceA, categoryA, brandA, colorA);

  char *locationA1 = ioopm_strdup("A01");
  int location_quantityA1 = 200;
  char *locationA2 = ioopm_strdup("A31");
  int location_quantityA2 = 100;
  db_add_merch(db, adidas);
  db_add_location_to_merch(adidas, locationA1, location_quantityA1);
  db_add_location_to_merch(adidas, locationA2, location_quantityA2);

  char *nameB = ioopm_strdup("nixe");
  char *descB = ioopm_strdup("bra byx");
  int priceB = 50;
  char *brandB = ioopm_strdup("adidas");
  char *categoryB = ioopm_strdup("shoes");
  char *colorB = ioopm_strdup("color");
  merch_t *nixe = db_create_merch(nameB, descB, priceB, categoryB, brandB, colorB);

  char *locationB1 = ioopm_strdup("A05");
  int location_quantityB1 = 200;
  char *locationB2 = ioopm_strdup("A62");
  int location_quantityB2 = 200;
  char *locationB3 = ioopm_strdup("A99");
  int location_quantityB3 = 50;
  db_add_merch(db, nixe);
  db_add_location_to_merch(nixe, locationB1, location_quantityB1);
  db_add_location_to_merch(nixe, locationB2, location_quantityB2);
  db_add_location_to_merch(nixe, locationB3, location_quantityB3);

  CU_ASSERT_EQUAL(db_lookup_merch_quantity_in_locations(adidas), 300);
  CU_ASSERT_EQUAL(db_lookup_merch_quantity_in_locations(nixe), 450);

  db_add_merch_to_cart(cartA, nameA, 10);
  db_update_merch_quantity_in_cart(cartA, nameA, 40);
  CU_ASSERT_EQUAL(db_lookup_merch_quantity_in_locations(adidas), 300);
  CU_ASSERT_EQUAL(db_lookup_merch_quantity_in_locations(nixe), 450);
  CU_ASSERT_EQUAL(db_get_merch_location_quantity(adidas, "A01"), 200);
  CU_ASSERT_EQUAL(db_get_merch_location_quantity(adidas, "A31"), 100);

  db_checkout(db, cartA);
  CU_ASSERT_EQUAL(db_lookup_merch_quantity_in_locations(adidas), 250);
  CU_ASSERT_EQUAL(db_lookup_merch_quantity_in_locations(nixe), 450);
  CU_ASSERT_EQUAL(db_get_merch_location_quantity(adidas, "A01"), 150);
  CU_ASSERT_EQUAL(db_get_merch_location_quantity(adidas, "A31"), 100);

  CU_ASSERT_PTR_NOT_NULL(cartB);
  db_add_merch_to_cart(cartB, nameA, 200);
  db_add_merch_to_cart(cartB, nameB, 200);
  db_update_merch_quantity_in_cart(cartB, nameB, 50);
  CU_ASSERT_EQUAL(db_lookup_merch_quantity_in_locations(adidas), 250);
  CU_ASSERT_EQUAL(db_lookup_merch_quantity_in_locations(nixe), 450);

  db_checkout(db, cartB);
  CU_ASSERT_EQUAL(db_lookup_merch_quantity_in_locations(adidas), 50);
  CU_ASSERT_EQUAL(db_lookup_merch_quantity_in_locations(nixe), 200);
  CU_ASSERT_EQUAL(db_get_merch_location_quantity(adidas, "A01"), 0);
  CU_ASSERT_EQUAL(db_get_merch_location_quantity(adidas, "A31"), 50);
  CU_ASSERT_EQUAL(db_get_merch_location_quantity(nixe, "A05"), 0);
  CU_ASSERT_EQUAL(db_get_merch_location_quantity(nixe, "A62"), 150);
  CU_ASSERT_EQUAL(db_get_merch_location_quantity(nixe, "A99"), 50);

  db_destroy_webstore(db);
}

void test13_costs_and_valid_quantities(void)
{
  webstore_t *db = db_create_webstore();

  shopping_carts_t *cartA = db_create_cart();
  db_add_cart(db, cartA);
  shopping_carts_t *cartB = db_create_cart();
  db_add_cart(db, cartB);

  char *nameA = ioopm_strdup("adidas");
  char *descA = ioopm_strdup("bra skor");
  int priceA = 100;
  char *brandA = ioopm_strdup("adidas");
  char *categoryA = ioopm_strdup("shoes");
  char *colorA = ioopm_strdup("color");
  merch_t *adidas = db_create_merch(nameA, descA, priceA, categoryA, brandA, colorA);

  char *locationA1 = ioopm_strdup("A01");
  int location_quantityA1 = 200;
  char *locationA2 = ioopm_strdup("A31");
  int location_quantityA2 = 100;
  db_add_merch(db, adidas);
  db_add_location_to_merch(adidas, locationA1, location_quantityA1);
  db_add_location_to_merch(adidas, locationA2, location_quantityA2);

  CU_ASSERT_EQUAL(300, db_lookup_valid_quantity(db, adidas, nameA));
  CU_ASSERT_EQUAL(300, db_lookup_merch_quantity_in_locations(adidas));
  CU_ASSERT_EQUAL(0, db_lookup_merch_quantity_in_carts(db, nameA));

  db_add_merch_to_cart(cartA, nameA, 100);
  CU_ASSERT_EQUAL(200, db_lookup_valid_quantity(db, adidas, nameA));
  CU_ASSERT_EQUAL(100, db_lookup_merch_quantity_in_carts(db, nameA));

  db_add_merch_to_cart(cartB, nameA, 150);
  CU_ASSERT_EQUAL(300, db_lookup_merch_quantity_in_locations(adidas));
  CU_ASSERT_EQUAL(250, db_lookup_merch_quantity_in_carts(db, nameA));
  CU_ASSERT_EQUAL(50, db_lookup_valid_quantity(db, adidas, nameA));

  db_destroy_webstore(db);
}

void test14_something(void)
{
  webstore_t *db = db_create_webstore();

  shopping_carts_t *cartA = db_create_cart();
  db_add_cart(db, cartA);

  char *nameA = ioopm_strdup("adidas");
  char *descA = ioopm_strdup("bra skor");
  int priceA = 3;
  char *brandA = ioopm_strdup("adidas");
  char *categoryA = ioopm_strdup("shoes");
  char *colorA = ioopm_strdup("color");
  merch_t *adidas = db_create_merch(nameA, descA, priceA, categoryA, brandA, colorA);

  char *locationA1 = ioopm_strdup("A01");
  int location_quantityA1 = 200;
  db_add_merch(db, adidas);
  db_add_location_to_merch(adidas, locationA1, location_quantityA1);

  // CU_ASSERT_EQUAL(200, db_lookup_merch_quantity_in_locations(adidas));
  // CU_ASSERT_EQUAL(0, db_lookup_merch_quantity_in_carts(db, nameA));
  // CU_ASSERT_EQUAL(200, db_lookup_valid_quantity(db, adidas, nameA));

  db_add_merch_to_cart(cartA, nameA, 100);
  CU_ASSERT_EQUAL(300, db_calculate_cost(db, cartA));
  db_remove_merch_from_cart(cartA, nameA);
  CU_ASSERT_EQUAL(0, db_calculate_cost(db, cartA));

  // CU_ASSERT_EQUAL(200, db_lookup_merch_quantity_in_locations(adidas));
  // CU_ASSERT_EQUAL(100, db_lookup_merch_quantity_in_carts(db, nameA));
  // CU_ASSERT_EQUAL(100, db_lookup_valid_quantity(db, adidas, nameA));

  db_lookup_merch_quantity_in_carts(db, nameA);

  db_destroy_webstore(db);
}

int init_suite(void)
{
  return 0;
}

int clean_suite(void)
{
  return 0;
}

int main()
{
  CU_pSuite test_suite1 = NULL;

  if (CUE_SUCCESS != CU_initialize_registry())
    return CU_get_error();

  test_suite1 = CU_add_suite("Test Suite 1", init_suite, clean_suite);
  if (NULL == test_suite1)
  {
    CU_cleanup_registry();
    return CU_get_error();
  }

  if (
      (NULL == CU_add_test(test_suite1, "test 0 common functions", test1_common_functions)) ||
      (NULL == CU_add_test(test_suite1, "test 1 create and destroy", test2_create_and_destroy)) ||
      (NULL == CU_add_test(test_suite1, "test 2 create merch", test3_create_and_edit_merch)) ||
      (NULL == CU_add_test(test_suite1, "test 3 remove merch", test4_remove_merch)) ||
      (NULL == CU_add_test(test_suite1, "test 5 replenish new", test5_replenish_new)) ||
      (NULL == CU_add_test(test_suite1, "test 6 replenish edit", test6_replenish_edit)) ||
      (NULL == CU_add_test(test_suite1, "test 7 create carts", test7_create_carts)) ||
      (NULL == CU_add_test(test_suite1, "test 8 add and remove cart", test8_add_and_remove_cart)) ||
      (NULL == CU_add_test(test_suite1, "test 9 calculate cost", test9_calculate_cost)) ||
      (NULL == CU_add_test(test_suite1, "test 10 checkout", test10_checkout)) ||
      (NULL == CU_add_test(test_suite1, "test 11 checkout without update", test11_checkout_without_update)) ||
      (NULL == CU_add_test(test_suite1, "test 12 checkout with update", test12_checkout_with_update)) ||
      (NULL == CU_add_test(test_suite1, "test 13 costs and valid quantities", test13_costs_and_valid_quantities)) ||
      (NULL == CU_add_test(test_suite1, "test 14 something", test14_something))

  )
  {
    CU_cleanup_registry();
    return CU_get_error();
  }

  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  CU_cleanup_registry();
  return CU_get_error();
}