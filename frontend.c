#include "frontend.h"
#include "common.h"

// ### Internal ###
static void print_menu(void);
static int menu_choice(void);
static void ui_event_loop(webstore_t *db);

static void print_menu(void) {
  printf("***\n"
         "Choose one of the listed alternatives: \n"
         "1. Add merchandise \n"
         "2. List merchandise\n"
         "3. Remove merchandise\n"
         "4. Edit merchandise\n"
         "5. Show stock\n"
         "6. Replenish\n"
         "7. Create cart\n"
         "8. Remove cart\n"
         "9. Add to cart\n"
         "10. Remove from cart\n"
         "11. Calculate cost\n"
         "12. Checkout\n"
         "13. Quit\n"
         "***\n");
}

static int menu_choice(void) {
  char *raw_choice;
  int choice = 0;

  do {
    raw_choice = ask_question_string("State the choice: ");
    choice = atoi(raw_choice);
  } while (!(choice <= 13 && choice >= 1));

  free(raw_choice);
  return choice;
}

// ### Internal ###

void ui_create_merch(webstore_t *db) {
  char *name = ask_question_string("State the name of the merchandise: ");
  char *desc = ask_question_string("State the description of the merchandise:");
  int price = ask_question_int("State the price of the merchandise: ");
  char *category =
      ask_question_string("State the category of the merhandise: ");

  if (!is_positive(price)) {
    printf("The price of the merchandise must be positive. Try again from the "
           "beginning.\n");
  } else if (db_has_key(db, name)) {
    printf("The given name already exists in the Webstore. Try again from the "
           "beginning.\n");
  } else {
    merch_t *merch = db_create_merch(name, desc, price, category);
    db_add_merch(db, merch);
    printf("--- The merchandise has been successfully added ---\n");
  }
}

void ui_list_merchs(webstore_t *db) {
  int merch_count = db_merch_count(db);

  if (merch_count == 0) {
    printf("--- The Webstore is empty. There is nothing to display. ---\n");
  } else {
    printf("There are %d merchandises in the Webstore: \n", merch_count);
    db_list_merchs(db);
  }
}

void ui_remove_merch(webstore_t *db) {
  char *name =
      ask_question_string("State the name of the merchandise to remove: ");

  if (db_has_key(db, name)) {
    db_remove_merch(db, name);
    printf("--- The merchandise has been removed. ---\n");
  } else {
    printf("--- A merchandise with the given name does not exist. Return to "
           "the main menu and try again. ---\n");
  }
}

void ui_edit_merch(webstore_t *db) {
  char *current_name =
      ask_question_string("State the name of the merchandise to edit: ");
  bool has_current_name = db_has_key(db, current_name);

  if (has_current_name) {
    char *new_name =
        ask_question_string("State the new name of the merchandise: ");
    char *new_desc =
        ask_question_string("State the new description of the merchandise: ");
    int new_price =
        ask_question_int("State the new price of the merchandise: ");
    char *new_category =
        ask_question_string("State the new category of the merchandise: ");

    bool has_new_name = db_has_key(db, new_name);

    if (has_new_name) {
      printf("--- It is not possible to edit the name '%s' to '%s' since the "
             "latter already exists in the Webstore ---\n",
             current_name, new_name);
    } else {
      merch_t *current_merch = db_get_merch(db, current_name);
      merch_t *edited_merch =
          db_edit_merch(db, current_merch, new_name, new_desc, new_price, new_category);

      db_remove_merch(db, current_name);
      db_add_merch(db, edited_merch);
    }
  } else {
    printf("--- A merchandise with the given name does not exist. Try agian "
           "from the beginning. ---\n");
  }
}

void ui_show_stock(webstore_t *db) {
  char *merch_name = ask_question_string("State the name of the merchandise: ");

  if (db_has_key(db, merch_name)) {
    merch_t *merch = db_get_merch(db, merch_name);
    int size = db_merch_locations_size(merch);

    if (size == 0) {
      printf("--- The given merch is newly added and does not have locations. "
             "---\n");
    } else {
      printf("--- The merch '%s' does have %d locations: \n",
             db_get_name(merch), size);
      db_display_stock(merch);
    }
  } else {
    printf("--- A merchandise with the given name does not exist. Try agian "
           "from the beginning. ---\n");
  }
}

void ui_replenish(webstore_t *db) {
  char *merch_name = ask_question_string("State the name of the merchandise: ");

  if (db_has_key(db, merch_name)) {
    merch_t *merch = db_get_merch(db, merch_name);

    printf("The merch '%s' does have the following locations: \n",
           db_get_name(merch));
    db_display_stock(merch);

    printf("Choose one alternative: \n"
           "14. Replenish on a new location.\n"
           "15. Replenish on existing location.\n");

    int choice = ask_question_int("State the choice: ");

    if (choice == 14) {
      ui_add_new_location(db, merch);
    } else if (choice == 15) {
      ui_edit_location(db, merch);
    }
  } else {
    printf("--- There is not a merchandise with the given name. Try again from "
           "the beginning. ---\n");
  }
}

void ui_add_new_location(webstore_t *db, merch_t *merch) {
  char *shelf_name = ask_question_shelf("State the name of the new shelf: ");
  int new_quantity = ask_question_int("State the quantity of the shelf: ");

  if (!is_positive(new_quantity)) {
    printf("The quantity must be positive. Try again from the beginning.\n");
  } else if (db_location_name_exists_in_webstore(db, shelf_name)) {
    printf("There is already a merchandise that does have the given shelf "
           "name. Try again from the beginning.\n");
  } else {
    db_add_location_to_merch(merch, shelf_name, new_quantity);
    printf("--- The new location(shelf) has been successfully added to the "
           "merchandise. ---\n");
  }
}

void ui_edit_location(webstore_t *db, merch_t *merch) {
  char *shelf_name =
      ask_question_shelf("State the name of the shelf to edit: ");
  int new_quantity = ask_question_int("State the new quantity of the shelf: ");

  if (!is_positive(new_quantity)) {
    printf("The quantity must be positive. Try again from the beginning.\n");
  } else if (db_edit_location_quantity(merch, shelf_name, new_quantity)) {
    printf("--- Quantity successfully changed. ---\n");
  } else {
    printf("---A shelf with the given name does not exist. Try again from the "
           "beginning.---\n");
  }
}

void ui_create_cart(webstore_t *db) {
  shopping_carts_t *cart = db_create_cart();
  db_add_cart(db, cart);
  printf("--- The cart has been successfully added. ---\n");
}

void ui_remove_cart(webstore_t *db) {
  int carts_size = db_carts_size(db);

  if (carts_size == 0) {
    printf("--- There are no active shopping carts and thus nothing to "
           "destroy. Try again from the beginning. ---\n");
  } else {

    printf("There are %d active carts in the Webstore.\n", carts_size);
    printf("Active cart ids: \n");
    db_display_cart_ids(db);

    int id_choice = ask_question_int("State the cart id you wish to remove: ");

    bool result = db_remove_cart(db, id_choice);

    if (result) {
      printf("--- The cart has been successfully removed.---\n");
    } else {
      printf("--- There is not a cart with given id. Try again from the "
             "beginning.---\n");
    }
  }
}

void ui_add_to_cart(webstore_t *db) {
  int carts_size = db_carts_size(db);

  if (carts_size == 0) {
    printf("--- There are no active shopping carts in the Webstore. Try again "
           "from the beginning. ---\n");
    ui_event_loop(db);
  }

  printf("There are %d active carts in the Webstore.\n", carts_size);
  printf("Active cart ids: \n");
  db_display_cart_ids(db);

  int id_choice = ask_question_int("State the cart id: ");
  shopping_carts_t *cart = db_get_cart_from_id(db, id_choice);

  if (cart == NULL) {
    printf("--- There is not a cart with the given id. Try again from the "
           "beginning. ---\n");
  } else {
    char *merch_name = ask_question_string(
        "State the name of the merch you wish to add to cart: ");
    bool merch_exists = db_has_key(db, merch_name);

    if (!merch_exists) {
      printf("--- There is not a merchandise with given name. Try again from "
             "the beginning.--- \n");
    } else {
      merch_t *merch = db_get_merch_from_name(db, merch_name);

      int quantity_in_locations = db_lookup_merch_quantity_in_locations(merch);
      int quantity_in_carts = db_lookup_merch_quantity_in_carts(db, merch_name);

      int valid_quantity = quantity_in_locations - quantity_in_carts;

      printf("There %d units available.\n", valid_quantity);

      int quantity_choice =
          ask_question_int("State how many units to add to the cart: ");

      if (quantity_choice > valid_quantity) {
        printf("--- The input exceeds available units. Try again from the "
               "beginning.---\n");
      } else if (db_cart_has_key(cart, merch_name)) {
        db_update_merch_quantity_in_cart(cart, merch_name, quantity_choice);
      } else {
        {
          db_add_merch_to_cart(cart, merch_name, quantity_choice);
        }
      }
    }
  }
}

void ui_remove_from_cart(webstore_t *db) {
  int carts_size = db_carts_size(db);

  if (carts_size == 0) {
    printf("--- There are no active shopping carts in the Webstore. Try again "
           "from the beginning. ---\n");
    ui_event_loop(db);
  }

  printf("There are %d active carts in the Webstore.\n", carts_size);
  printf("Active cart ids: \n");
  db_display_cart_ids(db);

  int id_choice = ask_question_int("State the cart id: ");
  shopping_carts_t *cart = db_get_cart_from_id(db, id_choice);

  if (cart == NULL) {
    printf("--- There is not a cart with the given id. Try again from the "
           "beginning. ---\n");
  } else {
    char *merch_name = ask_question_string(
        "State the name of the merch you wish to remove from the cart: ");
    bool merch_exists = db_has_key(db, merch_name);

    if (!merch_exists) {
      printf("--- There is not a merchandise with given name. Try again from "
             "the beginning.--- \n");
    } else {
      if (!db_cart_has_key(cart, merch_name)) {
        printf("--- The cart does not contain a merchandise with given name. "
               "Try again from the beginning. -- \n");
      } else {
        db_remove_merch_from_cart(cart, merch_name);
        printf("--- The merchandise have been successfully removed from the "
               "cart ---");
      }
    }
  }
}

void ui_calculate_cost(webstore_t *db) {
  int carts_size = db_carts_size(db);

  if (carts_size == 0) {
    printf("--- There are no active shopping carts in the Webstore. Try again "
           "from the beginning. ---\n");
    ui_event_loop(db);
  }

  printf("There are %d active carts in the Webstore.\n", carts_size);
  printf("Active cart ids: \n");
  db_display_cart_ids(db);

  int id_choice = ask_question_int("State the cart id: ");
  shopping_carts_t *cart = db_get_cart_from_id(db, id_choice);

  if (cart == NULL) {
    printf("--- There is not a cart with the given id. Try again from the "
           "beginning. ---\n");
  } else {
    int cost = db_calculate_cost(db, cart);
    printf(
        "--- The total cost of all merchandises in the shopping cart is: %d\n",
        cost);
  }
}

void ui_checkout(webstore_t *db) {
  int carts_size = db_carts_size(db);

  if (carts_size == 0) {
    printf("--- There are no active shopping carts in the Webstore. Try again "
           "from the beginning. ---\n");
    ui_event_loop(db);
  }

  printf("There are %d active carts in the Webstore.\n", carts_size);
  printf("Active cart ids: \n");
  db_display_cart_ids(db);

  int id_choice = ask_question_int("State the cart id: ");
  shopping_carts_t *cart = db_get_cart_from_id(db, id_choice);

  if (cart == NULL) {
    printf("--- There is not a cart with the given id. Try again from the "
           "beginning. ---\n");
  } else {
    db_checkout(db, cart);
    db_remove_cart(db, id_choice);
    printf("--- Transactiong successfull. ---\n");
  }
}

///
///
///
void ui_destroy_webstore(webstore_t *db) {
  db_destroy_webstore(db);
  printf("--- The Webstore is now shut down. ---\n");
  exit(0); // utan den terminerar inte programmet
}

static void ui_event_loop(webstore_t *db) {
  while (true) {
    print_menu();
    int choice = menu_choice();

    if (choice == 1) {
      ui_create_merch(db);
    } else if (choice == 2) {
      ui_list_merchs(db);
    } else if (choice == 3) {
      ui_remove_merch(db);
    } else if (choice == 4) {
      ui_edit_merch(db);
    } else if (choice == 5) {
      ui_show_stock(db);
    } else if (choice == 6) {
      ui_replenish(db);
    } else if (choice == 7) {
      ui_create_cart(db);
    } else if (choice == 8) {
      ui_remove_cart(db);
    } else if (choice == 9) {
      ui_add_to_cart(db);
    } else if (choice == 10) {
      ui_remove_from_cart(db);
    } else if (choice == 11) {
      ui_calculate_cost(db);
    } else if (choice == 12) {
      ui_checkout(db);
    } else if (choice == 13) {
      ui_destroy_webstore(db);
    }
  }
}

int main(void) {
  webstore_t *db = db_create_webstore();
  ui_event_loop(db);
}
