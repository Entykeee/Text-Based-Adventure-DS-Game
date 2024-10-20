#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_DESC 200
#define MAX_NAME 50
#define MAX_INPUT 100

typedef struct Item {
    char name[MAX_NAME];
    char description[MAX_DESC];
    struct Item* next;
} Item;

typedef struct Location {
    char name[MAX_NAME];
    char description[MAX_DESC];
    struct Location* north;
    struct Location* south;
    struct Location* east;
    struct Location* west;
    Item* items;
} Location;

typedef struct {
    Location* current_location;
    Item* inventory;
} Player;

Location* create_location(const char* name, const char* description) {
    Location* loc = malloc(sizeof(Location));
    strncpy(loc->name, name, MAX_NAME);
    strncpy(loc->description, description, MAX_DESC);
    loc->north = loc->south = loc->east = loc->west = NULL;
    loc->items = NULL;
    return loc;
}

Item* create_item(const char* name, const char* description) {
    Item* item = malloc(sizeof(Item));
    strncpy(item->name, name, MAX_NAME);
    strncpy(item->description, description, MAX_DESC);
    item->next = NULL;
    return item;
}

void add_item_to_location(Location* loc, Item* item) {
    item->next = loc->items;
    loc->items = item;
}

void remove_item_from_location(Location* loc, Item* item) {
    if (loc->items == item) {
        loc->items = item->next;
    } else {
        Item* current = loc->items;
        while (current->next != NULL && current->next != item) {
            current = current->next;
        }
        if (current->next == item) {
            current->next = item->next;
        }
    }
    item->next = NULL;
}

void add_item_to_inventory(Player* player, Item* item) {
    item->next = player->inventory;
    player->inventory = item;
}

void remove_item_from_inventory(Player* player, Item* item) {
    if (player->inventory == item) {
        player->inventory = item->next;
    } else {
        Item* current = player->inventory;
        while (current->next != NULL && current->next != item) {
            current = current->next;
        }
        if (current->next == item) {
            current->next = item->next;
        }
    }
    item->next = NULL;
}

void print_location(Location* loc) {
    printf("\n== %s ==\n", loc->name);
    printf("%s\n", loc->description);
    if (loc->items != NULL) {
        printf("You see: ");
        Item* current = loc->items;
        while (current != NULL) {
            printf("%s ", current->name);
            current = current->next;
        }
        printf("\n");
    }
}

void print_inventory(Player* player) {
    if (player->inventory == NULL) {
        printf("Your inventory is empty.\n");
    } else {
        printf("Inventory: ");
        Item* current = player->inventory;
        while (current != NULL) {
            printf("%s ", current->name);
            current = current->next;
        }
        printf("\n");
    }
}

bool move_player(Player* player, char direction) {
    Location* next_location = NULL;
    switch (direction) {
        case 'n': next_location = player->current_location->north; break;
        case 's': next_location = player->current_location->south; break;
        case 'e': next_location = player->current_location->east; break;
        case 'w': next_location = player->current_location->west; break;
    }
    
    if (next_location != NULL) {
        player->current_location = next_location;
        return true;
    }
    return false;
}

Item* find_item(Location* loc, const char* item_name) {
    Item* current = loc->items;
    while (current != NULL) {
        if (strcmp(current->name, item_name) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

Item* find_item_in_inventory(Player* player, const char* item_name) {
    Item* current = player->inventory;
    while (current != NULL) {
        if (strcmp(current->name, item_name) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

int main() {
    // Create locations
    Location* forest = create_location("Forest", "You are in a dense forest. Trees surround you.");
    Location* cave = create_location("Cave", "You are in a dark cave. It's cold and damp.");
    Location* river = create_location("River", "You are at a fast-flowing river. The water looks deep.");
    Location* mountain = create_location("Mountain", "You are on a tall mountain. The view is breathtaking.");

    // Connect locations
    forest->north = cave;
    forest->east = river;
    cave->south = forest;
    cave->east = mountain;
    river->west = forest;
    river->north = mountain;
    mountain->west = cave;
    mountain->south = river;

    // Create items
    Item* key = create_item("key", "A rusty old key.");
    Item* sword = create_item("sword", "A shiny sword.");
    Item* potion = create_item("potion", "A mysterious looking potion.");

    // Place items in locations
    add_item_to_location(forest, key);
    add_item_to_location(cave, sword);
    add_item_to_location(mountain, potion);

    // Initialize player
    Player player = {forest, NULL};

    char input[MAX_INPUT];
    printf("Welcome to the Text Adventure!\n");
    printf("Commands: north, south, east, west, look, inventory, take [item], drop [item], quit\n");

    while (1) {
        print_location(player.current_location);
        printf("\nWhat do you want to do? ");
        fgets(input, MAX_INPUT, stdin);
        input[strcspn(input, "\n")] = 0;  // Remove newline

        if (strcmp(input, "quit") == 0) {
            break;
        } else if (strcmp(input, "north") == 0 || strcmp(input, "south") == 0 ||
                   strcmp(input, "east") == 0 || strcmp(input, "west") == 0) {
            if (!move_player(&player, input[0])) {
                printf("You can't go that way.\n");
            }
        } else if (strcmp(input, "look") == 0) {
            // Do nothing, location will be printed at start of loop
        } else if (strcmp(input, "inventory") == 0) {
            print_inventory(&player);
        } else if (strncmp(input, "take ", 5) == 0) {
            char* item_name = input + 5;
            Item* item = find_item(player.current_location, item_name);
            if (item != NULL) {
                remove_item_from_location(player.current_location, item);
                add_item_to_inventory(&player, item);
                printf("You took the %s.\n", item_name);
            } else {
                printf("There's no %s here.\n", item_name);
            }
        } else if (strncmp(input, "drop ", 5) == 0) {
            char* item_name = input + 5;
            Item* item = find_item_in_inventory(&player, item_name);
            if (item != NULL) {
                remove_item_from_inventory(&player, item);
                add_item_to_location(player.current_location, item);
                printf("You dropped the %s.\n", item_name);
            } else {
                printf("You don't have a %s.\n", item_name);
            }
        } else {
            printf("I don't understand that command.\n");
        }
    }

    printf("Thanks for playing!\n");
    return 0;
}