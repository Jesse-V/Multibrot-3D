
#ifndef MAIN
#define MAIN

#include <vector>
#include <string>

typedef std::vector<std::vector<short>> Matrix2D;

Matrix2D readMatrix(std::string filename);
void removeIslands(Matrix2D& matrix, std::size_t startX, std::size_t startY);
bool compress(Matrix2D& matrix, std::size_t level);
void writeGeometry(Matrix2D& matrix, std::string filename);
void writeMatrix(Matrix2D& matrix, std::string filename, short max);

#endif
