#include <gtk/gtk.h>
#include "SudokuSquare.h"


SudokuSquare::SudokuSquare(GtkWidget *new_entry_box, int num, int col, int row){
    entry_box = new_entry_box;
    start_num = num;
    x = col;
    y = row;
    // master = grid;
    gtk_widget_set_size_request(entry_box, 64, 64);
}

GtkWidget* SudokuSquare::getButtonWidget(){
    return entry_box;
}

void SudokuSquare::select(){
    selected = true;
    gtk_widget_set_name(this->entry_box, "selected_sudoku_tile");
}

void SudokuSquare::unselect(){
    selected = false;
    gtk_widget_set_name(this->entry_box, "sudoku_tile");

}

bool SudokuSquare::isSelected(){
    return selected;
}

bool SudokuSquare::isHint(){
    return (start_num > 0);
}

int SudokuSquare::getX(){
    return x;
}

int SudokuSquare::getY(){
    return y;
}
