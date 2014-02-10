
#ifndef MAIN
#define MAIN

struct Point3D
{
    Point3D(int x, int y, int z):
        x_(x), y_(y), z_(z)
    {}

    int x_, y_, z_;
};

#include <vector>
#include <string>

typedef std::vector<std::vector<short>> Matrix2D;
typedef std::vector<Matrix2D> Matrix3D;
typedef std::pair<Point3D, Point3D> Bounds2D;

Matrix3D readMatrix(std::vector<std::string>& filenames);
void removeIslands(Matrix3D& matrix, std::size_t startX, std::size_t startY);
bool compress(Matrix3D& matrix, std::size_t level);
void writeGeometry(Matrix3D& matrix, std::string filename);
Point3D getPointOf(Matrix3D& matrix, short boxSize);
Bounds2D eliminateBoxOf(Matrix3D& matrix, Point3D point);
void writeMatrix(Matrix3D& matrix, std::string filename, short max);

#endif
