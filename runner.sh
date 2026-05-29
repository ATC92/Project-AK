#! /bin/bash

echo "Compilando Project_AK ..."
cmake --build build --config Debug --target Project_AK -- -j 8
echo "Compilacion terminada! "
echo "Ejecucion en proceso :)"
build/Debug/Project_AK
