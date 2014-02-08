
#include "main.hpp"
#include <fstream>
#include <sstream>
#include <iterator>
#include <iostream>


const std::size_t SIZE = 1024;


int main(int argc, char** argv)
{
    Matrix2D matrix = readMatrix(std::string("multibrot,d=2.dat"));

    std::cout << "Writing pre image... ";
    writeMatrix(matrix, std::string("image_phase1.ppm"), 1);
    std::cout << "done." << std::endl;

    std::cout << "Remove islands... ";
    removeIslands(matrix, SIZE / 2, SIZE / 2);
    std::cout << "done." << std::endl;

    std::cout << "Writing island-less image... ";
    writeMatrix(matrix, std::string("image_phase2.ppm"), 2);
    std::cout << "done." << std::endl;

    std::cout << "Compressing. Looking for boxes of size ";
    std::size_t boxSize = 1;
    while (compress(matrix, boxSize))
    {
        std::cout << boxSize << " ";
        boxSize *= 2;
    }
    std::cout << "done." << std::endl;

    std::cout << "Writing compressed image... ";
    writeMatrix(matrix, std::string("image_phase3.ppm"), (short)boxSize);
    std::cout << "done." << std::endl;

    std::cout << "Writing geometry... ";
    writeGeometry(matrix, std::string("geometry.dat"));
    writeMatrix(matrix, std::string("image_phase4.ppm"), (short)boxSize);
    std::cout << "done." << std::endl;

    std::cout << "Program complete." << std::endl;

    return EXIT_SUCCESS;
}



Matrix2D readMatrix(std::string filename)
{
    std::string line;
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
    Matrix2D matrix;

    for (std::size_t y = 0; y < SIZE; y++)
    {
        bool state = false;
        std::vector<short> row;
        for (std::size_t j = 0; j < allIntegers[y].size(); j++)
        {
            row.insert(row.end(), (std::size_t)allIntegers[y][j], state ? 1 : 0);
            state = !state;
        }

        matrix.push_back(row);
    }

    return matrix;
}



void removeIslands(Matrix2D& matrix, std::size_t startX, std::size_t startY)
{
    matrix[startX][startY] = -1;

    bool expanded;
    do
    {
        expanded = false;

        for (std::size_t x = 0; x < SIZE; x++)
        {
            for (std::size_t y = 0; y < SIZE; y++)
            {
                if (matrix[x][y] < 0) //if already marked
                {
                    if (matrix[x][y - 1] > 0) //if inside set
                    {
                        matrix[x][y - 1] = -1;
                        expanded = true;
                    }

                    if (matrix[x][y + 1] > 0) //if inside set
                    {
                        matrix[x][y + 1] = -1;
                        expanded = true;
                    }

                    if (matrix[x - 1][y] > 0) //if inside set
                    {
                        matrix[x - 1][y] = -1;
                        expanded = true;
                    }

                    if (matrix[x + 1][y] > 0) //if inside set
                    {
                        matrix[x + 1][y] = -1;
                        expanded = true;
                    }
                }
            }
        }

    } while (expanded);

    for (std::size_t x = 0; x < SIZE; x++)
        for (std::size_t y = 0; y < SIZE; y++)
            matrix[x][y] = matrix[x][y] < 0 ? 1 : 0; //all marked are good, otherwise remove
}



bool compress(Matrix2D& matrix, std::size_t lookingFor)
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



void writeGeometry(Matrix2D& matrix, std::string filename)
{
    std::ofstream fout;
    fout.open(filename, std::ofstream::out);

    short boxSize = 1;
    bool found;
    do
    {
        found = false;
        std::cout << boxSize << " ";
        std::cout.flush();

        Point2D startPoint;
        while (true) //eliminate all boxes
        {
            startPoint = getPointOf(matrix, boxSize);
            if (startPoint.first < 0 || startPoint.second < 0)
                break; //box not found

            Bounds2D bounds = eliminateBoxOf(matrix, getPointOf(matrix, boxSize));
            if (boxSize == 1) //dealing with points
            {
                for (int x = bounds.first.first; x <= bounds.second.first; x++)
                    for (int y = bounds.first.second; y <= bounds.second.second; y++)
                            fout << x << " " << y << std::endl;

                //TODO: it'd be really great if GL_LINES could be used efficiently
            }
            else
            {
                fout << bounds.first.first <<
                    " " << bounds.first.second <<
                    " " << bounds.second.first <<
                    " " << bounds.second.second << std::endl;
            }

            found = true;
        }

        boxSize *= 2;

    } while (found);

    fout.close();
}



Point2D getPointOf(Matrix2D& matrix, short boxSize)
{
    for (std::size_t x = 0; x < SIZE; x++)
        for (std::size_t y = 0; y < SIZE; y++)
            if (matrix[x][y] == boxSize)
                return std::make_pair((int)x, (int)y);

    return std::make_pair(-1, -1);
}



Bounds2D eliminateBoxOf(Matrix2D& matrix, Point2D point)
{
    short targetBoxSize = matrix[(std::size_t)point.first][(std::size_t)point.second];
    matrix[(std::size_t)point.first][(std::size_t)point.second] = -1;

    bool expanded;
    do
    {
        expanded = false;

        for (std::size_t x = 0; x < SIZE; x++)
        {
            for (std::size_t y = 0; y < SIZE; y++)
            {
                if (matrix[x][y] < 0) //if already marked
                {
                    if (matrix[x][y - 1] == targetBoxSize)
                    {
                        matrix[x][y - 1] = -1;
                        expanded = true;
                    }

                    if (matrix[x][y + 1] == targetBoxSize)
                    {
                        matrix[x][y + 1] = -1;
                        expanded = true;
                    }

                    if (matrix[x - 1][y] == targetBoxSize)
                    {
                        matrix[x - 1][y] = -1;
                        expanded = true;
                    }

                    if (matrix[x + 1][y] == targetBoxSize)
                    {
                        matrix[x + 1][y] = -1;
                        expanded = true;
                    }
                }
            }
        }

    } while (expanded);

    Point2D min = std::make_pair(SIZE, SIZE), max = std::make_pair(-1, -1);
    for (int x = 0; x < (int)SIZE; x++)
    {
        for (int y = 0; y < (int)SIZE; y++)
        {
            if (matrix[(std::size_t)x][(std::size_t)y] < 0) //if marked
            {
                matrix[(std::size_t)x][(std::size_t)y] = 0; //eliminate

                if (x < min.first)
                    min.first = (int)x;
                if (y < min.second)
                    min.second = (int)y;

                if (x > max.first)
                    max.first = (int)x;
                if (y > max.second)
                    max.second = (int)y;
            }
        }
    }

    return std::make_pair(min, max);
}



void writeMatrix(Matrix2D& matrix, std::string filename, short max)
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
                fout << gray << " " << gray << " " << gray << " ";
        }

        fout << std::endl;
    }
    fout.close();
}
