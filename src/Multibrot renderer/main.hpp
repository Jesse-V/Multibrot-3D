
#ifndef MAIN
#define MAIN

#include "Chunk.struct"
#include <vector>
#include <string>

typedef std::vector<std::vector<bool>> Matrix2D;

void initializeMatrix(Matrix2D& matrix);
void render(Chunk chunk, Matrix2D& matrix);
bool bordersAreInside(Chunk chunk);
float toFractalSpace(unsigned int pixel);
bool isInsideFractal(float x, float y);
void writeMatrix(Matrix2D& matrix, std::string filename);

#endif
