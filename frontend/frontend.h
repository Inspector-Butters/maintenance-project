#include "../common/common.h"
#include "../backend/backend.h"

/**
 * @file frontend.h
 * @author Erdem Garip
 * @date 15 Nov 2022
 * @brief Frontend of the Warehouse application
 *
 */

/// @brief Adds a new merchandise to the Webstore
/// @param db The webstore
void ui_create_merch(webstore_t *db);

/// @brief Lists the merchandises in the Webstore
/// @param db The webstore
void ui_list_merchs(webstore_t *db);

/// @brief Removes a merchandise from the Webstore and the shopping carts
/// @param db The webstore
void ui_remove_merch(webstore_t *db);

/// @brief Edits the name, description and the price of a merchandise
/// @param db The webstore
void ui_edit_merch(webstore_t *db);

/// @brief Propmpts the user to either add a new location to a merchandise or edit an existing location of a merchandise
/// @param db The Webstore
void ui_replenish(webstore_t *db);

/// @brief Adds a new location to an existing merchandise
/// @param db The Webstore
/// @param merch An existing merchandise
void ui_add_new_location(webstore_t *db, merch_t *merch);

/// @brief Edits the existing location of a merchandise
/// @param db The Webstore
/// @param merch The merchandise whose location will be edited
void ui_edit_location(webstore_t *db, merch_t *merch);

/// @brief Creates a cart
/// @param db The Webstore
void ui_create_cart(webstore_t *db);

/// @brief Removes a cart
/// @param db The webstore
void ui_remove_cart(webstore_t *db);

/// @brief Adds a merchandise to a shopping cart
/// @param db The webstore
void ui_add_to_cart(webstore_t *db);

/// @brief Removes a merchandise from a shopping cart
/// @param db The webstore
void ui_remove_from_cart(webstore_t *db);

//
//
/// @brief Destroys the merchandises, carts and the Webstore and frees all their memories
/// @param db The webstore to destroy
void ui_destroy_webstore(webstore_t *db);
