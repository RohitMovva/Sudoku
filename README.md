# Sudoku
create_binary_matrix.cpp is the file that contains the actual algorithm. It uses Knuth's algorithm X implemented with dancing links. It isn't the cleanest or most optimized code but it works.

# To run (for me)
Compile the gtk file using:
gcc `pkg-config --cflags gtk+-3.0` -o example-0 test.cpp `pkg-config --libs gtk+-3.0`
Will compile to example-0
