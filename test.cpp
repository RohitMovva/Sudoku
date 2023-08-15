#include <gtk/gtk.h>

// GtkWidget selected_button = NULL;

// static void select_button(GtkWidget *widget, gpointer data) {
//     // g_print ("Hello World\n");
//     selected_button = widget;
// }

static void activate (GtkApplication *app, gpointer user_data) {
    GtkWidget *window;
    GtkWidget *button;
    GtkWidget *button_box;
    GtkWidget *entry_box;
    GtkWidget *sudoku_grid;
    int size = 9;
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Sudoku");
    gtk_window_set_default_size(GTK_WINDOW(window), 9*16, 9*64);
    // key_press_event(true);
    // button_box = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
    sudoku_grid = gtk_grid_new();
    // gtk_flow_box_set_max_children_per_line(sudoku_grid, 9);
    gtk_container_add(GTK_CONTAINER(window), sudoku_grid);
    gtk_grid_set_row_homogeneous(GTK_GRID(sudoku_grid), TRUE);
    gtk_grid_set_column_homogeneous(GTK_GRID(sudoku_grid), TRUE);
    // gtk_container_add(GTK_CONTAINER(window), button_box);

    for (int i = 0; i < size; i++){
        for (int j = 0; j < size; j++){
            entry_box = gtk_button_new();
            // gtk_widget_set_size_request(entry_box, 0, 0);
            gtk_widget_set_size_request(entry_box, 64, 64);
            // key_press_event()
            gtk_grid_attach(GTK_GRID(sudoku_grid), entry_box, i, j, 1, 1);
        }
    }

    // button = gtk_button_new_with_label ("Hello World");
    // g_signal_connect(button, "clicked", G_CALLBACK(print_hello), NULL);
    // g_signal_connect_swapped (button, "clicked", G_CALLBACK(gtk_widget_destroy), window);
    // gtk_container_add(GTK_CONTAINER(button_box), button);

    gtk_widget_show_all (window);
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
