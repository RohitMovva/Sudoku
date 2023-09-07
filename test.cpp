#include <gtk/gtk.h>
#include <vector>
#include "SudokuSquare.h"
#include <iostream>
#include <algorithm>
using namespace std;

SudokuSquare* selected_square = nullptr;

void setSelectedButton(GtkWidget* tile, gpointer* data, SudokuSquare* clicked_button){
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

gboolean onKeyPress (GtkWidget *widget, GdkEventKey *event, gpointer data) {
    if (event->keyval < 49 || event->keyval > 57){
        return FALSE;
    }
    if (selected_square == nullptr){
        return FALSE;
    }
    char buffer[1];
    sprintf(buffer, "%d", event->keyval-48);
    gtk_button_set_label (GTK_BUTTON(selected_square->getButtonWidget()), buffer);
    return TRUE;
    // gtk_button_set_label(GTK_BUTTON(selected_square), buffer);
}

void createSudokuGrid(GtkApplication *app, gpointer user_data){
    GtkWidget *window;
    GtkWidget *button;
    GtkWidget *button_box;
    GtkWidget *entry_box;
    GtkWidget *sudoku_grid;
    int size = 9;
    window = gtk_application_window_new(app);
    gtk_widget_add_events(window, GDK_KEY_PRESS_MASK);
    gtk_window_set_title(GTK_WINDOW(window), "Sudoku");
    sudoku_grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), sudoku_grid);
    vector<vector<SudokuSquare*>> board;
    g_signal_connect (G_OBJECT (window), "key_press_event", G_CALLBACK(onKeyPress), NULL);
    for (int i = 0; i < size; i++){
        board.push_back({});
        for (int j = 0; j < size; j++){
            entry_box = gtk_button_new();
            SudokuSquare* new_button = new SudokuSquare(entry_box);
            g_signal_connect(entry_box, "clicked", G_CALLBACK(setSelectedButton), new_button);
            board[i].push_back(new_button);
            gtk_grid_attach(GTK_GRID(sudoku_grid), entry_box, i, j, 1, 1);
            
        }
    }
    gtk_widget_show_all (window);
}

static void activate(GtkApplication *app, gpointer user_data) {
    createSudokuGrid(app, user_data); // SudokuGrid* grid = 
}

int main (int argc, char **argv) {
    GtkApplication *app;
    int status;

    app = gtk_application_new("org.gtk.example", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
