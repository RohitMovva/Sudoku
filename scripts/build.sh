# build.sh
#
# This script is used to configure and compile lamscript.

ROOT_DIR="$(git rev-parse --show-toplevel)"
pushd "$ROOT_DIR" > /dev/null

# ----------------------------- LAMBDA-SH & ARGS ------------------------------

source scripts/lambda.sh

LAMBDA_PARSE_ARG build Release "The type of build to produce."
LAMBDA_PARSE_ARG cpp_compiler g++ "The compiler to use for C++ code."
LAMBDA_PARSE_ARG cores 6 "The number of cores used for compilation."
LAMBDA_PARSE_ARG os linux "The operating system being built on."

LAMBDA_COMPILE_ARGS $@

export CXX="$LAMBDA_cpp_compiler"

BUILD_GENERATOR="Unix Makefiles"
BUILD_COMMAND="make -j $LAMBDA_cores"

if [ "$LAMBDA_os" = "Windows" ]; then
    BUILD_GENERATOR="Visual Studio 16 2019"
    BUILD_COMMAND="MSBuild.exe lamscript.sln //t:Rebuild //p:Configuration=$LAMBDA_build"
fi

LAMBDA_INFO "Attempting to Compile a $LAMBDA_build for OS $LAMBDA_os using compiler $LAMBDA_cpp_compiler or $LAMBDA_os"

# ----------------------------------- CMAKE ------------------------------------
case "$LAMBDA_os" in
    linux)
        LAMBDA_INFO "Attempting to Compile a $LAMBDA_build for Linux -. "
        $LAMBDA_cpp_compiler `pkg-config --cflags gtk+-3.0` -o build SudokuSquare.cpp createSudokuPuzzle.cpp test.cpp `pkg-config --libs gtk+-3.0`
        ;;
    macos)
        LAMBDA_INFO "Attempting to Compile a $LAMBDA_build for MacOS -."
        $LAMBDA_cpp_compiler `pkg-config --cflags gtk+-3.0` -o build SudokuSquare.cpp createSudokuPuzzle.cpp test.cpp `pkg-config --libs gtk+-3.0`
        ;;
    windows)
        LAMBDA_INFO "Attempting to Compile a $LAMBDA_build for Windows -."
        # var=`bash -c "pkg-config --cflags gtk+-3.0"`
        # LAMBDA_INFO $var
        # bash -c "pkg-config --libs gtk+-3.0"
        # bash -c "$LAMBDA_cpp_compiler `pkg-config --cflags gtk+-3.0` -o build SudokuSquare.cpp createSudokuPuzzle.cpp test.cpp `pkg-config --libs gtk+-3.0`"
        echo `pkg-config --libs gtk+-3.0`
        echo `pkg-config --cflags gtk+-3.0`
        echo `ls`
        echo `pwd`
        # echo `find /c -name 'libsimavr.a'`
        # CPPFLAGS=-I/mingw64/include LDFLAGS=-L/mingw64/lib
        $LAMBDA_cpp_compiler -L/a/_temp/msys64/mingw64/lib `pkg-config --cflags gtk+-3.0` -o build SudokuSquare.cpp createSudokuPuzzle.cpp test.cpp `pkg-config --libs gtk+-3.0`
        ;;
    *)
        echo "$USAGE"
        exit 1
        ;;
esac
LAMBDA_INFO "Build is complete for to Compile a $LAMBDA_build for OS $os"

# mkdir -p build
# pushd build > /dev/null

# if [ "$LAMBDA_build" = "Release" ] || [ "$LAMBDA_build" = "Debug" ]; then
#     cmake .. \
#         -G "$BUILD_GENERATOR" \
#         -DCMAKE_BUILD_TYPE="$LAMBDA_build"
# elif [ "$LAMBDA_build" = "Dist" ]; then
#     cmake .. \
#         -G "$BUILD_GENERATOR" \
#         -DCMAKE_BUILD_TYPE=Release
# else
#     LAMBDA_FATAL \
#         "You need to pass a valid build type in order to compile lamscript."
# fi

# LAMBDA_ASSERT_LAST_COMMAND_OK \
#     "Couldn't generate the cmake files necessary for compiling lamscript."

# ----------------------------------- BUILD ------------------------------------

# $BUILD_COMMAND

# LAMBDA_ASSERT_LAST_COMMAND_OK "Couldn't successfully compile lamscript."
# LAMBDA_INFO "Successfully compiled lamscript."

# popd > /dev/null  # build
# popd > /dev/null  # ROOT_DIR
