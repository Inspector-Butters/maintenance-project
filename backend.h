#include "common.h"
#include "linked_list.h"
#include "hash_table.h"
#include "iterator.h"

typedef struct merch merch_t;
typedef struct shelf shelf_t;
typedef struct webstore webstore_t;
typedef struct shopping_carts shopping_carts_t;



/**
 * @file backend.h
 * @author Erdem Garip
 * @date 15 Nov 2022
 * @brief Backend of the Warehouse application
 *  
 */

int string_hash(elem_t e);

/// @brief Creates a Webstore
/// @return A webstore consisting of two hastables db->merches and db->carts
webstore_t *db_create_webstore(void);

/// @brief Allocates memory for a merchandise
/// @param name the merchandises name
/// @param desc the description of the merchandise
/// @param price the price of the merchandise
/// @pre The price is positive, this property is checked and ensured in frontend
/// @return A merchandise
merch_t *db_create_merch(char *name, char *desc, int price);

/// @brief Allocates memory for a shelf
/// @param shelf_name the shelfs name
/// @param shelf_quantity the quantity held in shelf
/// @return A shelf
shelf_t *db_create_shelf(char *shelf_name, int shelf_quantity);

/// @brief Creates a new shopping cart and allocates memory for it
/// @return A new shopping cart
shopping_carts_t *db_create_cart(void);

/// @brief Destroys a merchandise and frees all the memory associated with it
/// @param db The merchandise struct
void db_destroy_a_merch(merch_t *merch);

/// @brief Checks if the Webstore has a merchandise with the given name
/// @param  db The Webstore
/// @param name The given name
/// @return True if the given name does not exist in the Webstore
bool db_has_key(webstore_t *db, char *name);

/// @brief Inserts a new merchandise into the Webstore
/// @param  db The Webstore
/// @param merch A merchandise
void db_add_merch(webstore_t *db, merch_t *merch);

/// @brief Checks the number of merchandises in the Webstore
/// @param db The webstore to check
/// @return The number of merchandises in the Webstore
int db_merch_count(webstore_t *db);

/// @brief Prints out the merchandises in the Webstore
/// @param db The webstore to print
void db_list_merchs(webstore_t *db);

/// @brief Prints out the contents of a merchandise
/// @param db The merchandise to display
void db_list_a_merch(merch_t *merch);

/// @brief Searhes after a merchandise given its name
/// @param db The webstore to lookup
/// @param name The name of the merchandise
/// @return The merchandise of given name
/// @pre A merchandise with the given name exists
merch_t *db_get_merch_from_name(webstore_t *db, char *name);

/// @brief Removes a merchandise from everywhere
/// @param db The webstore
/// @param name The name of the merchandise
void db_remove_merch(webstore_t *db, char *name);

/// @brief Removes a merchandise from the shopping cart
/// @param db the shopping cart
/// @param name The name of the merchandise
void db_remove_merch_from_cart(shopping_carts_t *cart, char *name);

/// @brief Edits the name, description and price of an existing merchandise.
/// @param db the Webstore
/// @param current_merch currently existing merch
/// @param new_name the new name of the current merchandise
/// @param new_desc the new description of the current merchandise
/// @param new_price the new price of the current merchandise
/// @return Returns the edited merchandise
merch_t *db_edit_merch(webstore_t *db, merch_t *current_merch, char *new_name, char *new_desc, int new_price);

/// @brief  Gets a merchandise from its name
/// @param db The webstore
/// @param name The name of the merchandise
/// @return The merchandise
/// @pre A merchandise of given name exists
merch_t *db_get_merch(webstore_t *db, char *name);

/// @brief Checks how many shelves a merchandise have
/// @param merch The merchandise to lookup
/// @return The quantity of shelves
int db_merch_locations_size(merch_t *merch);

/// @brief Prints out the shelf contents of a merchandise's locations
/// @param merch The merchandise to display
void db_display_stock(merch_t *merch);

/// @brief Iterates through the Webstore to check if a location with shelf_name exists
/// @param db The Webstore
/// @param shelf_name The new shelf name
/// @return True if there is a location(shelf) with the same name as shelf_name
bool db_location_name_exists_in_webstore(webstore_t *db, char *shelf_name);

/// @brief Adds a new location(shelf) to a merchandise
/// @param merch The merchandise
/// @param shelf_name The name of the new location
/// @param new_quantity The quantity of the new location
void db_add_location_to_merch(merch_t *merch, char *shelf_name, int new_quantity);

/// @brief Attempts to edit and existing location(shelf) of a merchandise
/// @param merch The merchandise
/// @param shelf_name The location's name
/// @param new_quantity The new quantity of the location
/// @return True if a location with given name exists
bool db_edit_location_quantity(merch_t *merch, char *shelf_name, int new_quantity);

/// @brief Adds a new shopping cart to the Webstore
/// @param db The webstore
/// @param cart the new cart
void db_add_cart(webstore_t *db, shopping_carts_t *cart);

/// @brief Checks the amount of carts in the Webstore
/// @param db The webstore
/// @return Quantity of carts in Webstore
int db_carts_size(webstore_t *db);

/// @brief Removes a cart with given id from the Webstore
/// @param db The Webstore
/// @param id_choice The id of the cart to be removed
/// @return True if a cart with id is removed
bool db_remove_cart(webstore_t *db, int id_choice);

/// @brief Prints the active cart ids in the Webstore
/// @param db The webstore
void db_display_cart_ids(webstore_t *db);

/// @brief Gets a cart given its id
/// @param db The webstore
/// @param id_choice The id of the sought cart
/// @return The cart if id exists in the Webstore
shopping_carts_t *db_get_cart_from_id(webstore_t *db, int id_choice);

/// @brief Checks the total amount of a merchandise in its locations
/// @param merch The merchandise
/// @return The sum of all quantities in a merchandise's shelves
int db_lookup_merch_quantity_in_locations(merch_t *merch);

/// @brief Checks the total amount of a merchandise in the Webstore's carts
/// @param db The webstore
/// @param merch_name The sought merchandise
/// @return The sum of all quantities of a merchandise in the Webstore's carts.
int db_lookup_merch_quantity_in_carts(webstore_t *db, char *merch_name);

/// @brief Checks if a cart has a merchandise
/// @param cart The shopping cart
/// @param merch_name The merchandise name
/// @return True if the merchandise exists in the cart
bool db_cart_has_key(shopping_carts_t *cart, char *merch_name);

/// @brief Reinserts the total sum of existing and new quantity of a merchandise
/// @param cart The shopping cart
/// @param merch_name The merchandise's name
/// @param new_quantity The additional quantity
void db_update_merch_quantity_in_cart(shopping_carts_t *cart, char *merch_name, int new_quantity);

/// @brief Adds the given quantity of a merchandise in the shopping cart
/// @param cart The shopping cart
/// @param merch_name The merchandise's name
/// @param new_quantity The quantity of the merchandise
void db_add_merch_to_cart(shopping_carts_t *cart, char *merch_name, int new_quantity);

/// @brief Calculates the cost of all merchandises in a shopping cart
/// @param db The database
/// @param cart The shopping cart
/// @return The total price of all merchandises in a cart
int db_calculate_cost(webstore_t *db, shopping_carts_t *cart);

/// @brief Removes all of the merchandises and their amounths from the Webstore's merchandise's locations.
/// @param db The Webstore
/// @param cart The Shopping cart
void db_checkout(webstore_t *db, shopping_carts_t *cart);

/// @brief Decreases the locations's(shelves) quantities untill given quantity is zero
/// @param merch The merchandise
/// @param quantity The amount of quantity to decrease in locations
void db_decrease_locations_quantities(merch_t *merch, int quantity);

/// @brief Destroys the webstore frees all the memory allocated by the datastructures
/// @param db The webstore to destroy
void db_destroy_webstore(webstore_t *db);

/// @brief Destroys the merchandises in the database and frees the memories allocated by them
/// @param db The hashtable where key-value pairs are key=>merch_name, value=>merch
void db_destroy_merchs(ioopm_hash_table_t *merchs);

/// @brief Destroys the shopping carts in the database and frees the memories allocated by them
/// @param db Key-value pairs are key=>cart_id, value=>shopping_cart. The shopping_cart itslef is key=>name, value=>quantity
void db_destroy_carts(ioopm_hash_table_t *carts);

/// @brief Destroys a cart and frees all the memory associated with it. It does not destroy the merchandises.
/// @param db The shopping cart struct
void db_destroy_a_cart(shopping_carts_t *cart);


/// @brief Calculates the valid quantity an user can enter
/// @param db The webstore
/// @param merch The merchandise 
/// @param merch_name The merchandise's name
/// @return The viable quantity that can be chosen
int db_lookup_valid_quantity(webstore_t *db, merch_t *merch, char *merch_name);


//### functions for testing ###

char *db_get_name(merch_t *merch);
char *db_get_desc(merch_t *merch);
int db_get_price(merch_t *merch);
int db_get_merch_location_quantity(merch_t *merch, char *shelf_name);
int db_get_merch_quantity_in_a_cart(shopping_carts_t *cart, char *merch_name);
