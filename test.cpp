#include <gtk/gtk.h>
#include <vector>
#include <iostream>
#include <algorithm>
#include <cmath>
#include "SudokuSquare.h"
#include "createSudokuPuzzle.h"
using namespace std;

SudokuSquare* selected_square = nullptr;

struct var_storage {
    // vector<vector<int>> board;
    // int empty_squares;
    int size;
    int difficulty;
    int empty_squares;
    GtkWidget* grid;
    GtkWidget* menu;
    GtkWidget* window;
    vector<vector<int>> board;
    var_storage(int new_size, int new_difficulty, GtkWidget* new_grid, GtkWidget* new_menu, GtkWidget* new_window, vector<vector<int>> new_board, int empty_tiles):\
     size(new_size), difficulty(difficulty), grid(new_grid), menu(new_menu), window(new_window), board(new_board), empty_squares(empty_tiles){}
};

struct var_storage_2 {
    vector<vector<int>> board;
    int empty_squares;
    var_storage_2(vector<vector<int>> new_board, int num_empty): board(new_board), empty_squares(num_empty){};

};

void setSelectedButton(GtkWidget* tile, gpointer data, SudokuSquare* clicked_button){
    if (clicked_button->isSelected()){
        clicked_button->unselect();
        selected_square = nullptr;
        return;
    } else if (selected_square != nullptr){
        selected_square->unselect();
    }
    selected_square = clicked_button;
    clicked_button->select();
}

void generate_grid(int size, int difficulty, vector<vector<int>> sudoku_puzzle, GtkWidget* grid){
    // GtkWidget *sudoku_grid = gtk_grid_new();
    GtkWidget* entry_box;
    vector<GtkWidget*> sub_squares;
    int subsize = sqrt(size);
    // vector<vector<int>> sudoku_puzzle = get_puzzle(size, difficulty);
    for (int i = 0; i < size; i++){
        GtkWidget* subgrid = gtk_grid_new();
        sub_squares.push_back(subgrid);
        g_object_set(subgrid, "margin", 3, NULL);
        gtk_grid_attach(GTK_GRID(grid), subgrid, i%subsize+1, i/subsize+1, 1, 1);
    }

    for (int i = 0; i < size; i++){
        for (int j = 0; j < size; j++){
            entry_box = gtk_button_new_with_label("");
            g_object_set(entry_box, "margin", 1, NULL);
            // gtk_widget_set_can_focus (entry_box, false);
            SudokuSquare* new_button = new SudokuSquare(entry_box, sudoku_puzzle[i][j], i, j);
            gtk_widget_set_name(entry_box, "sudoku_tile");
            if (sudoku_puzzle[i][j] > 0){
                char buffer[2] = {char(sudoku_puzzle[i][j]+48)};
                if (sudoku_puzzle[i][j] > 9){
                    buffer[0] += 7;
                }
                // sprintf(buffer, "%d", sudoku_puzzle[i][j]);
                gtk_button_set_label (GTK_BUTTON(entry_box), buffer);
            }
            g_signal_connect(entry_box, "clicked", G_CALLBACK(setSelectedButton), new_button);
            gtk_grid_attach(GTK_GRID(sub_squares[subsize*(i/subsize) + j/subsize]), entry_box, j%subsize, i%subsize, 1, 1);
        }
    }
    // return grid;
}

bool isValidMove(vector<vector<int>> board, int size, int y, int x, int user_num){
    int subsqr = sqrt(size);
    for (int i = 0; i < size; i++){
        if (i != y && board[i][x] == user_num){
            return false;
        } else if (i != x && board[y][i] == user_num){
            return false;
        } else if (!(subsqr*(y/subsqr)+i/subsqr == y && subsqr*(x/subsqr)+i%subsqr == x) && board[subsqr*(y/subsqr)+i/subsqr][subsqr*(x/subsqr)+i%subsqr] == user_num){
            return false;
        }
    }
    return true;
}

gboolean onKeyPress(GtkWidget* tile, GdkEventKey *event, gpointer data) {
    vector<vector<int>>* board = &(((var_storage*)data)->board);
    int empty_squares = ((var_storage*)data)->empty_squares;
    int size = (*board).size();
    if (selected_square == nullptr || selected_square->isHint()){
        return FALSE;
    }
    if (event->keyval == 65288){
        gtk_button_set_label(GTK_BUTTON(selected_square->getButtonWidget()), "");
        (*board)[selected_square->getX()][selected_square->getY()] = 0;
        empty_squares--;
    }
    if (size <= 9 && (event->keyval < 49 || event->keyval > 49+size)){ // size <= 9 && 
        return FALSE;
    }
    else if (size > 9 && (event->keyval < 49 || (event->keyval > 57 && event->keyval < 97) || event->keyval > 97+size-9)){
        return FALSE;
    }
    int val = event->keyval-48;
    if (val > 9){
        val -= 39;
    }
    if (!isValidMove(*board, (*board).size(), selected_square->getX(), selected_square->getY(), val)){ //  - (event->keyval >= 97)*40
        return FALSE;
    }
    char buffer[2] = {char(val+48)};
    if (val > 9){
        buffer[0] += 7;
    }
    gtk_button_set_label (GTK_BUTTON(selected_square->getButtonWidget()), buffer);
    (*board)[selected_square->getX()][selected_square->getY()] = event->keyval-48;
    empty_squares++;
    return TRUE;
}

void grid_deleter(gpointer data, gpointer user_data){
    GtkWidget* widget = (GtkWidget*)data;
    if (GTK_IS_GRID(widget)){
        gtk_widget_destroy(widget);
    }
}

void update_size(GtkMenuItem *item, gpointer user_data) {
    const gchar* label = gtk_menu_item_get_label(item);
    var_storage* datar = (var_storage*)user_data;
    // const gchar* size_4 = "4x4";
    // const gchar* size_9 = "9x9";
    int* size = &(((var_storage*)datar)->size);
    if ((*label) == (*"4x4")){
        *size = 4;
    } else if ((*label) == (*"9x9")){
        *size = 9;
    } else if ((*label) == (*"16x16")){
        *size = 16;
    }
    GList *children = gtk_container_get_children(GTK_CONTAINER(datar->grid));
    g_list_foreach(children, grid_deleter, NULL);
    vector<vector<int>> new_board = get_puzzle(*size, datar->difficulty);
    datar->board = new_board;
    datar->empty_squares = 0;
    for (auto& i: new_board){
        for (auto& j: i){
            if (j == 0){
                datar->empty_squares++;
            }
        }
    }
    generate_grid(datar->size, datar->difficulty, new_board, datar->grid);
    gtk_widget_show_all(datar->window);
}

void createSudokuGrid(GtkApplication *app, gpointer user_data){
    GtkWidget *window;
    GtkWidget *button;
    GtkWidget *button_box;
    GtkWidget *entry_box;
    GtkWidget *sudoku_grid;

    GtkWidget *menu_bar;

    GtkWidget *sub_menu_size;
    GtkWidget *menu_size;
    GtkWidget *size_small;
    GtkWidget *size_medium;
    GtkWidget *size_large;

    GtkWidget *menu_difficulty;
    GtkWidget *sub_menu_difficulty;
    GtkWidget *difficulty_easy;
    GtkWidget *difficulty_medium;
    GtkWidget *difficulty_hard;

    GtkWidget *menu_tools;
    GtkWidget *sub_menu_tools;
    GtkWidget *hint;
    GtkWidget *solve;
    GtkWidget *regenerate;

    int size = 9;
    int subsize = sqrt(size);
    int difficulty = 0;

    menu_bar = gtk_menu_bar_new();

    menu_tools = gtk_menu_item_new_with_mnemonic ("_Tools");
    sub_menu_tools = gtk_menu_new();
    hint = gtk_menu_item_new_with_label ("Hint");
    solve = gtk_menu_item_new_with_label ("Solve");
    regenerate = gtk_menu_item_new_with_label ("Regenerate Puzzle");

    menu_size = gtk_menu_item_new_with_mnemonic ("_Change Size");
    sub_menu_size = gtk_menu_new();
    size_small = gtk_menu_item_new_with_label ("4x4");
    size_medium = gtk_menu_item_new_with_label ("9x9");
    size_large = gtk_menu_item_new_with_label ("16x16");

    menu_difficulty = gtk_menu_item_new_with_mnemonic ("_Change Difficulty");
    sub_menu_difficulty = gtk_menu_new();
    difficulty_easy = gtk_menu_item_new_with_label ("Easy");
    difficulty_medium = gtk_menu_item_new_with_label ("Moderate");
    difficulty_hard = gtk_menu_item_new_with_label ("Hard");

    gtk_menu_shell_append (GTK_MENU_SHELL (sub_menu_tools), hint);
    gtk_menu_shell_append (GTK_MENU_SHELL (sub_menu_tools), solve);
    gtk_menu_shell_append (GTK_MENU_SHELL (sub_menu_tools), regenerate);
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (menu_tools), sub_menu_tools);

    gtk_menu_shell_append (GTK_MENU_SHELL (sub_menu_size), size_small);
    gtk_menu_shell_append (GTK_MENU_SHELL (sub_menu_size), size_medium);
    gtk_menu_shell_append (GTK_MENU_SHELL (sub_menu_size), size_large);
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (menu_size), sub_menu_size);

    gtk_menu_shell_append (GTK_MENU_SHELL (sub_menu_difficulty), difficulty_easy);
    gtk_menu_shell_append (GTK_MENU_SHELL (sub_menu_difficulty), difficulty_medium);
    gtk_menu_shell_append (GTK_MENU_SHELL (sub_menu_difficulty), difficulty_hard);
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (menu_difficulty), sub_menu_difficulty);
    
    gtk_menu_shell_append (GTK_MENU_SHELL (menu_bar), menu_tools);
    gtk_menu_shell_append (GTK_MENU_SHELL (menu_bar), menu_size);
    gtk_menu_shell_append (GTK_MENU_SHELL (menu_bar), menu_difficulty);

    vector<vector<int>> sudoku_puzzle = get_puzzle(size, difficulty);
    int empty_squares = 0;
    for (auto& i: sudoku_puzzle){
        for (auto& j: i){
            if (j != 0) empty_squares++;
        }
    }

    GtkCssProvider *cssProvider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(cssProvider, "mystyle.css", NULL);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_USER);

    window = gtk_application_window_new(app);
    gtk_widget_add_events(window, GDK_KEY_PRESS_MASK);
    gtk_window_set_title(GTK_WINDOW(window), "Sudoku");
    sudoku_grid = gtk_grid_new();

    
    // vector<vector<int>> puzzle = get_puzzle(size, difficulty);
    // var_storage_2* vars = new var_storage_2(sudoku_puzzle, *empty_squares);
    generate_grid(size, difficulty, sudoku_puzzle, sudoku_grid);
    // menu_bar = generate_menu();
    gtk_container_add(GTK_CONTAINER(window), sudoku_grid);
    gtk_grid_attach(GTK_GRID(sudoku_grid), menu_bar, 0, 0, subsize, 1);

    var_storage* datar = new var_storage(size, difficulty, sudoku_grid, menu_bar, window, sudoku_puzzle, empty_squares);

    g_signal_connect(G_OBJECT(window), "key_press_event", G_CALLBACK(onKeyPress), datar);

    g_signal_connect(size_small, "activate", G_CALLBACK(update_size), datar);
    g_signal_connect(size_medium, "activate", G_CALLBACK(update_size), datar);
    g_signal_connect(size_large, "activate", G_CALLBACK(update_size), datar);
    g_signal_connect(regenerate, "activate", G_CALLBACK(update_size), datar);


    GtkCssProvider *provider = gtk_css_provider_new ();
    gtk_css_provider_load_from_path (provider, "mystyle.css", NULL);

    // subsize = sqrt(size);

    gtk_widget_show_all(window);
}

static void activate(GtkApplication *app, gpointer user_data) {
    createSudokuGrid(app, user_data);
}

void win(){
    cout << "WIN!\n";
    // add more to this later
}

// void on_popup_clicked (GtkButton* button, GtkWidget* pWindow)
// {
//     GtkWidget *popup_window;
//     popup_window = gtk_window_new (GTK_WINDOW_POPUP);
//     gtk_window_set_title (GTK_WINDOW (popup_window), "Pop Up window");
//     gtk_container_set_border_width (GTK_CONTAINER (popup_window), 10);
//     gtk_window_set_resizable(GTK_WINDOW (popup_window), FALSE);
//     gtk_window_set_decorated(GTK_WINDOW (popup_window), FALSE);
//     gtk_widget_set_size_request (popup_window, 150, 150);
//     gtk_window_set_transient_for(GTK_WINDOW (popup_window),GTK_WINDOW (pWindow));
//     gtk_window_set_position (GTK_WINDOW (popup_window),GTK_WIN_POS_CENTER);
//     g_signal_connect (G_OBJECT (button), "event",
//                         G_CALLBACK (on_popup_window_event),NULL);

//     GdkColor color;
//     gdk_color_parse("#3b3131", &color);
//     gtk_widget_modify_bg(GTK_WIDGET(popup_window), GTK_STATE_NORMAL, &color);


//     gtk_widget_show_all (popup_window);
// }

// gboolean on_popup_window_event(GtkWidget *popup_window, GdkEventExpose *event)
// {
//     if(event->type == GDK_FOCUS_CHANGE)
//         gtk_widget_hide (popup_window);

//     return FALSE;
// }

int main (int argc, char **argv) {
    GtkApplication *app;
    int status;
    
    app = gtk_application_new("org.gtk.example", G_APPLICATION_FLAGS_NONE);
    // vector<vector<int>> sudoku_puzzle = get_puzzle(9, 0);
    int empty_squares = 0;
    // for (auto& i: sudoku_puzzle){
    //     for (auto& j: i){
    //         if (j != 0) empty_squares++;
    //     }
    // }
    // var_storage* info = new var_storage(sudoku_puzzle, empty_squares);
    g_signal_connect(app, "activate", G_CALLBACK(activate), &empty_squares);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    
    g_object_unref(app);

    return status;
}
