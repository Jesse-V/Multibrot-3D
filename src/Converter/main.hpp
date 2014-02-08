
#ifndef MAIN
#define MAIN

#include <vector>
#include <string>

typedef std::vector<std::vector<short>> Matrix2D;

Matrix2D readMatrix(std::string filename);
bool compress(Matrix2D& matrix, std::size_t level);
void writeMatrix(Matrix2D& matrix, std::string filename, short max);
void writeGeometry(Matrix2D& matrix, std::string filename);

#endif
