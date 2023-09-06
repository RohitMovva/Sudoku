# Sudoku
create_binary_matrix.cpp is the file that contains the actual algorithm. It uses Knuth's algorithm X implemented with dancing links. It isn't the cleanest or most optimized code but it works.

# To run (for me)
Compile the gtk file using:
g++ `pkg-config --cflags gtk+-3.0` -o example-0 test.cpp SudokuSquare.cpp `pkg-config --libs gtk+-3.0`
Will compile to example-0

Thing on how to do keypresses: https://stackoverflow.com/questions/44098084/how-do-i-handle-keyboard-events-in-gtk3
