#include <gtk/gtk.h>

#ifndef SUDOKUSQUARE_H
#define SUDOKUSQUARE_H

class SudokuSquare{
    public:
    GtkWidget* entry_box;
    // SudokuGrid* master;
    bool selected;

    SudokuSquare(GtkWidget *new_entry_box);
    GtkWidget* getButtonWidget();
    // void onButtonPress();
    void select();
    void unselect();
    bool isSelected();

};

#endif