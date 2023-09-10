#include <gtk/gtk.h>

#ifndef SUDOKUSQUARE_H
#define SUDOKUSQUARE_H

class SudokuSquare{
    public:
    GtkWidget* entry_box;
    // SudokuGrid* master;
    bool selected;
    int start_num, x, y;
    SudokuSquare(GtkWidget *new_entry_box, int num, int x, int y);
    GtkWidget* getButtonWidget();
    // void onButtonPress();
    void select();
    void unselect();
    bool isSelected();
    bool isHint();
    int getX();
    int getY();

};

#endif