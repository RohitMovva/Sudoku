#include <gtk/gtk.h>
#include "SudokuSquare.h"


SudokuSquare::SudokuSquare(GtkWidget *new_entry_box){
    entry_box = new_entry_box;
    // master = grid;
    gtk_widget_set_size_request(entry_box, 64, 64);
}

GtkWidget* SudokuSquare::getButtonWidget(){
    return entry_box;
}

void SudokuSquare::select(){
    selected = true;
}

void SudokuSquare::unselect(){
    selected = false;
}

bool SudokuSquare::isSelected(){
    return selected;
}
