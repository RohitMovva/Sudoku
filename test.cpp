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
    vector<vector<int>> board;
    int empty_squares;
    var_storage(vector<vector<int>> puzzle, int num_empty): board(puzzle), empty_squares(num_empty) {}
};

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

gboolean onKeyPress(GtkWidget* tile, GdkEventKey *event, gpointer data) {
    // vector<vector<int>> *board = data;
    vector<vector<int>>* board = &(((var_storage*)data)->board);
    int empty_squares = ((var_storage*)data)->empty_squares;
    if (selected_square == nullptr || selected_square->isHint()){
        return FALSE;
    }
    if (event->keyval == 65288){
        gtk_button_set_label(GTK_BUTTON(selected_square->getButtonWidget()), "");
        (*board)[selected_square->getX()][selected_square->getY()] = 0;
        empty_squares--;
    }
    if (event->keyval < 49 || event->keyval > 57){
        return FALSE;
    }
    if (!isValidMove(*board, 9, selected_square->getX(), selected_square->getY(), event->keyval-48)){
        return FALSE;
    }
    char buffer[2];
    sprintf(buffer, "%d", event->keyval-48);
    gtk_button_set_label (GTK_BUTTON(selected_square->getButtonWidget()), buffer);
    (*board)[selected_square->getX()][selected_square->getY()] = event->keyval-48;
    // cout << selected_square->getX() << " " << selected_square->getY() << "\n";
    empty_squares++;
    return TRUE;
    // gtk_button_set_label(GTK_BUTTON(selected_square), buffer);
}

void createSudokuGrid(GtkApplication *app, gpointer user_data){
    vector<vector<int>> sudoku_puzzle = ((var_storage*)user_data)->board;
    GtkWidget *window;
    GtkWidget *button;
    GtkWidget *button_box;
    GtkWidget *entry_box;
    GtkWidget *sudoku_grid;
    int size = 9;

    GtkCssProvider *cssProvider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(cssProvider, "mystyle.css", NULL);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
                                GTK_STYLE_PROVIDER(cssProvider),
                                GTK_STYLE_PROVIDER_PRIORITY_USER);

    window = gtk_application_window_new(app);
    gtk_widget_add_events(window, GDK_KEY_PRESS_MASK);
    gtk_window_set_title(GTK_WINDOW(window), "Sudoku");
    sudoku_grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), sudoku_grid);
    vector<vector<SudokuSquare*>> board;
    g_signal_connect(G_OBJECT(window), "key_press_event", G_CALLBACK(onKeyPress), user_data);


    GtkCssProvider *provider = gtk_css_provider_new ();
    gtk_css_provider_load_from_path (provider, "mystyle.css", NULL);

    for (int i = 0; i < size; i++){
        board.push_back({});
        for (int j = 0; j < size; j++){
            entry_box = gtk_button_new_with_label("");
            g_object_set(entry_box, "margin", 1, NULL);
            SudokuSquare* new_button = new SudokuSquare(entry_box, sudoku_puzzle[i][j], i, j);
            gtk_widget_set_name(entry_box, "sudoku_tile");
            if (sudoku_puzzle[i][j] > 0){
                char buffer[2];
                sprintf(buffer, "%d", sudoku_puzzle[i][j]);
                gtk_button_set_label (GTK_BUTTON(entry_box), buffer);
            }
            g_signal_connect(entry_box, "clicked", G_CALLBACK(setSelectedButton), new_button);
            board[i].push_back(new_button);
            gtk_grid_attach(GTK_GRID(sudoku_grid), entry_box, j, i, 1, 1);
            
        }
    }
    gtk_widget_show_all(window);
}

static void activate(GtkApplication *app, gpointer user_data) {
    createSudokuGrid(app, user_data); // SudokuGrid* grid = 
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
    vector<vector<int>> sudoku_puzzle = get_puzzle();
    int empty_squares = 0;
    for (auto& i: sudoku_puzzle){
        for (auto& j: i){
            if (j != 0) empty_squares++;
        }
    }
    var_storage* info = new var_storage(sudoku_puzzle, empty_squares);
    g_signal_connect(app, "activate", G_CALLBACK(activate), info);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    
    g_object_unref(app);

    return status;
}
