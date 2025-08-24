#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#include "kr_one_ex_one.h"
#include "kr_one_ex_two.h"
#include "kr_one_ex_three.h"
#include "kr_two_ex_one.h"
#include "kr_two_ex_two.h"
#include "kr_two_ex_three.h"
#include "kr_three_ex_one.h"
#include "kr_three_ex_two.h"
#include "kr_three_ex_three.h"


#define KEY_UP      72
#define KEY_DOWN    80
#define KEY_ENTER   13
#define KEY_RIGHT   77
#define KEY_LEFT    75
#define KEY_ESCAPE  27

char key;                       // current pressed key
int current_menu_paragraph = 0; // current menu item

// structure for the menu element
typedef struct MenuItem {
    char name[20];          // name of the menu item
    void (*action)(void);   // function performed when choosing a item
    struct Menu* submenu;   // submenu (if any)
} MenuItem;

// Structure for the menu
typedef struct Menu {
    char title[20];
    int item_count;
    MenuItem* items;        // assist for menu items
    struct Menu* parent;    // parent menu
} Menu;

// create a submenu
Menu MENU_A, MENU_B, MENU_C;

// points of the main menu
MenuItem main_items[] = {
    {"Test 1", NULL, &MENU_A},
    {"Test 2", NULL, &MENU_B},
    {"Test 3", NULL, &MENU_C}
};

// menu A
MenuItem menuA_items[] = {
    {"exes 1", kr_one_ex_one, NULL},
    {"exes 2", kr_one_ex_two, NULL},
    {"exes 3", kr_one_ex_three, NULL}
};

// menu B
MenuItem menuB_items[] = {
    {"exes 1", kr_two_ex_one, NULL},
    {"exes 2", kr_two_ex_two, NULL},
    {"exes 3", kr_two_ex_three, NULL}
};

// menu C
MenuItem menuC_items[] = {
    {"exes 1", kr_three_ex_one, NULL},
    {"exes 2", kr_three_ex_two, NULL},
    {"exes 3", kr_three_ex_three, NULL}
};

// init menu
Menu MAIN_MENU = {
    "1 year mr.B. Keith",
    3,
    main_items,
    NULL
};

Menu MENU_A = {
    "Test 1",
    3,
    menuA_items,
    &MAIN_MENU
};

Menu MENU_B = {
    "Test 2",
    3,
    menuB_items,
    &MAIN_MENU
};

Menu MENU_C = {
    "Test 3",
    3,
    menuC_items,
    &MAIN_MENU
};

Menu *current_menu = &MAIN_MENU; // index for the current menu

void print_current_menu(void) {
    system("cls");
    printf("\t\t\t%s\n\n", current_menu->title);
    for (int i = 0; i < current_menu->item_count; ++i) {
        if (i == current_menu_paragraph) {
            printf("-> ");
        }
        printf("%s\n", current_menu->items[i].name);
    }
}

void key_processing(char key) {
    if (key == KEY_DOWN) {
        current_menu_paragraph++;
        if (current_menu_paragraph >= current_menu->item_count) {
            current_menu_paragraph = 0;
        }
    }

    if (key == KEY_UP) {
        current_menu_paragraph--;
        if (current_menu_paragraph < 0) {
            current_menu_paragraph = current_menu->item_count - 1;
        }
    }

    if (key == KEY_RIGHT) {
        if (current_menu->items[current_menu_paragraph].submenu != NULL) {
            current_menu = current_menu->items[current_menu_paragraph].submenu;
            current_menu_paragraph = 0;
        }
    }

    if (key == KEY_LEFT) {
        if (current_menu->parent != NULL) {
            current_menu = current_menu->parent;
            current_menu_paragraph = 0;
        }
    }

    if (key == KEY_ENTER) {
        if (current_menu->items[current_menu_paragraph].action != NULL) {
            current_menu->items[current_menu_paragraph].action();
        } else if (current_menu->items[current_menu_paragraph].submenu != NULL) {
            current_menu = current_menu->items[current_menu_paragraph].submenu;
            current_menu_paragraph = 0;
        }
    }
}

int main() {

    do {
        print_current_menu();
        key = getch();
        key_processing(key);
    } while(key != KEY_ESCAPE);

    return 0;
}
