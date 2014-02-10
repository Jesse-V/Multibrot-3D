
#include "main.hpp"
#include <fstream>
#include <sstream>
#include <iterator>
#include <iostream>


const std::size_t SIZE = 1024;
const std::size_t HEIGHT = 1;


int main(int argc, char** argv)
{
    std::vector<std::string> files{"multibrot,d=2.dat"};
    Matrix3D matrix = readMatrix(files);

    //std::cout << "Writing pre image... ";
    //writeMatrix(matrix, std::string("image_phase1.ppm"), 1);
    //std::cout << "done." << std::endl;

    std::cout << "Remove islands... ";
    removeIslands(matrix, SIZE / 2, SIZE / 2);
    std::cout << "done." << std::endl;

    //std::cout << "Writing island-less image... ";
    //writeMatrix(matrix, std::string("image_phase2.ppm"), 1);
    //std::cout << "done." << std::endl;

    std::cout << "Compressing. Looking for boxes of size ";
    std::size_t boxSize = 1;
    while (compress(matrix, boxSize))
    {
        std::cout << boxSize << " ";
        boxSize *= 2;
    }
    std::cout << "done." << std::endl;

    //std::cout << "Writing compressed image... ";
    //writeMatrix(matrix, std::string("image_phase3.ppm"), (short)boxSize / 2);
    //std::cout << "done." << std::endl;

    std::cout << "Calculating geometry, ";
    writeGeometry(matrix, std::string("geometry.dat"));
    //writeMatrix(matrix, std::string("image_phase4.ppm"), (short)boxSize / 2);
    std::cout << "finished." << std::endl;

    std::cout << "Program complete." << std::endl;

    return EXIT_SUCCESS;
}



Matrix3D readMatrix(std::vector<std::string>& filenames)
{
    Matrix3D matrix;

    std::string line;
    for (const auto &filename : filenames)
    {
        std::ifstream file;
        file.open(filename, std::ifstream::in);
        std::vector<std::vector<int>> allIntegers;
        while (getline(file, line))
        {
            std::istringstream is(line);
            allIntegers.push_back(
                std::vector<int>(std::istream_iterator<int>(is),
                std::istream_iterator<int>()
            ));
        }

        file.close();
        Matrix2D plane;

        for (std::size_t y = 0; y < SIZE; y++)
        {
            bool state = false;
            std::vector<short> row;
            for (std::size_t j = 0; j < allIntegers[y].size(); j++)
            {
                row.insert(row.end(), (std::size_t)allIntegers[y][j], state ? 1 : 0);
                state = !state;
            }

            plane.push_back(row);
        }

        matrix.push_back(plane);
    }

    return matrix;
}



void removeIslands(Matrix3D& matrix, std::size_t startX, std::size_t startY)
{
    matrix[matrix.size() / 2][startX][startY] = -1;

    bool expanded;
    do
    {
        expanded = false;

        for (std::size_t d = 0; d < HEIGHT; d++)
        {
            for (std::size_t x = 0; x < SIZE; x++)
            {
                for (std::size_t y = 0; y < SIZE; y++)
                {
                    if (matrix[d][x][y] < 0) //if already marked
                    {
                        //check neighbors are inside set, if so mark them

                        if (d > 0 && matrix[d - 1][x][y] > 0)
                        {
                            matrix[d - 1][x][y] = -1;
                            expanded = true;
                        }

                        if (d + 1 < HEIGHT && matrix[d + 1][x][y] > 0)
                        {
                            matrix[d + 1][x][y] = -1;
                            expanded = true;
                        }

                        if (x > 0 && matrix[d][x - 1][y] > 0)
                        {
                            matrix[d][x - 1][y] = -1;
                            expanded = true;
                        }

                        if (x + 1 < SIZE && matrix[d][x + 1][y] > 0)
                        {
                            matrix[d][x + 1][y] = -1;
                            expanded = true;
                        }

                        if (y > 0 && matrix[d][x][y - 1] > 0)
                        {
                            matrix[d][x][y - 1] = -1;
                            expanded = true;
                        }

                        if (y + 1 < SIZE && matrix[d][x][y + 1] > 0)
                        {
                            matrix[d][x][y + 1] = -1;
                            expanded = true;
                        }
                    }
                }
            }
        }

    } while (expanded);

    for (std::size_t d = 0; d < HEIGHT; d++)
        for (std::size_t x = 0; x < SIZE; x++)
            for (std::size_t y = 0; y < SIZE; y++)
                matrix[d][x][y] = matrix[d][x][y] < 0 ? 1 : 0;
                //all marked are good, otherwise remove
}

/*                                  BELOW NEEDS 3D CONVERSION                 */

bool compress(Matrix3D& matrix, std::size_t lookingFor)
{
    auto newSize = lookingFor * 2;
    bool found = false;

    for (std::size_t x = 0; x <= SIZE - newSize; x += newSize)
    {
        for (std::size_t y = 0; y <= SIZE - newSize; y += newSize)
        {
            if (matrix[x][y] == (short)lookingFor)
            {
                if (lookingFor == matrix[x + lookingFor][y] &&
                    lookingFor == matrix[x + lookingFor][y + lookingFor] &&
                    lookingFor == matrix[x][y + lookingFor]
                )
                    for (std::size_t j = 0; j < newSize; j++)
                        for (std::size_t k = 0; k < newSize; k++)
                            matrix[x + j][y + k] = (short)newSize;

                found = true;
            }
        }
    }

    return found;
}



void writeGeometry(Matrix3D& matrix, std::string filename)
{
    std::ofstream fout;
    fout.open(filename, std::ofstream::out);

    std::cout << "eliminating boxes of size ";

    unsigned int pointCount = 0, lineCount = 0, squareCount = 0;

    short boxSize = 1; //initialize to minimum box size
    bool found;
    do
    {
        found = false;
        std::cout << boxSize << " ";
        std::cout.flush();

        Point3D startPoint;
        while (true) //eliminate all boxes
        {
            startPoint = getPointOf(matrix, boxSize);
            if (startPoint.first < 0 || startPoint.second < 0)
                break; //box not found

            Bounds2D bounds = eliminateBoxOf(matrix, startPoint);
            if (boxSize == 1) //dealing with points
            {
                auto diffX = bounds.second.first - bounds.first.first;
                auto diffY = bounds.second.second - bounds.first.second;

                if (diffX == 1 && diffY == 1) //if point
                {
                    pointCount++;
                    fout << "1 " << bounds.first.first << " " << bounds.first.second << std::endl;
                }
                else //if line
                {
                    lineCount++;
                    fout << "2 " << bounds.first.first <<
                        " " << bounds.first.second <<
                        " " << bounds.second.first <<
                        " " << bounds.second.second << std::endl;
                }
            }
            else //dealing with squares
            {
                squareCount++;
                fout << "4 " << bounds.first.first <<
                    " " << bounds.first.second <<
                    " " << bounds.second.first <<
                    " " << bounds.second.second << std::endl;
            }

            found = true;
        }

        boxSize *= 2;

    } while (found);

    std::cout << "complete, (" << squareCount << "," <<
        lineCount << "," << pointCount << ") ";

    fout.close();
}



Point3D getPointOf(Matrix3D& matrix, short boxSize)
{
    for (std::size_t x = 0; x < SIZE; x++)
        for (std::size_t y = 0; y < SIZE; y++)
            if (matrix[x][y] == boxSize)
                return std::make_pair((int)x, (int)y);

    return std::make_pair(-1, -1);
}



//the given point must be the upper left corner (minimum) of the box
Bounds3D eliminateBoxOf(Matrix3D& matrix, Point2D point)
{
    std::size_t startX = (std::size_t)point.first, startY = (std::size_t)point.second;
    short boxSize = matrix[startX][startY];
    matrix[startX][startY] = -1;

    //look x+ and y+ direction due to algorithm in getPointOf
    std::size_t xLength = 1; //number of boxes in the x direction
    while (matrix[startX + xLength * boxSize][startY] == boxSize)
        xLength++;

    std::size_t yLength = 1; //number of boxes in the x direction
    while (matrix[startX][startY + yLength * boxSize] == boxSize)
        yLength++;

    //it's guaranteed at this point that x != 2 && y != 2

    Point2D max;
    if (xLength > yLength)
        max = std::make_pair(point.first + xLength * boxSize, point.second + boxSize);
    else
        max = std::make_pair(point.first + boxSize, point.second + yLength * boxSize);

    for (int x = point.first; x < max.first; x++)
        for (int y = point.second; y < max.second; y++)
            matrix[(std::size_t)x][(std::size_t)y] = 0; //eliminate

    return std::make_pair(point, max);
}



/*
void writeMatrix(Matrix3D& matrix, std::string filename, short max)
{
    std::ofstream fout;
    fout.open(filename, std::ofstream::out);

    fout << "P3 " << SIZE << " " << SIZE << " 255" << std::endl;
    for (const auto &row : matrix)
    {
        for (const auto &cell : row)
        {
            int gray = (int)(cell / (float)max * 255);
            if (cell == 0)
                fout << "0 0 255 ";
            else if (cell == 1)
                fout << "0 255 0 ";
            else
                fout << (gray / 2) << " " << gray << " " << gray << " ";
        }

        fout << std::endl;
    }
    fout.close();
}
*/
