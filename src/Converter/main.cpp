
#include "main.hpp"
#include <fstream>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <iostream>


const std::size_t SIZE = 1024;
const std::size_t HEIGHT = 1024;


int main(int argc, char** argv)
{
    std::cout << "Reading files... ";
    std::vector<std::string> files;
    for (std::size_t j = 1; j <= HEIGHT; j++)
    {
        std::stringstream ss("");
        //ss << "test" << j << ".txt";
        ss << "multibrot,d=" << (j / (float)32 + 1) << ".dat";
        files.push_back(ss.str());
    }
    std::cout << "done." << std::endl;

    Matrix3D matrix = readMatrix(files);

    std::cout << "Remove islands... ";
    std::cout.flush();
    removeIslands(matrix, HEIGHT / 2, SIZE / 2, SIZE / 2);
    std::cout << "done." << std::endl;
    std::cout.flush();

    std::cout << "Compressing. Looking for boxes of size ";
    std::size_t boxSize = 1;
    while (compress(matrix, boxSize))
        boxSize *= 2;
    std::cout << "done." << std::endl;
/*
    for (std::size_t d = 0; d < HEIGHT; d++)
    {
        for (std::size_t x = 0; x < SIZE; x++)
        {
            for (std::size_t y = 0; y < SIZE; y++)
                std::cout << matrix[d][x][y] << " ";
            std::cout << std::endl;
        }
        std::cout << std::endl << std::endl;
    }*/

    std::cout << "Calculating geometry, ";
    writeGeometry(matrix, std::string("geometry.dat"));
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
        if (file.fail())
            std::cout << "Unable to open \"" << filename << "\"!" << std::endl;

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



void removeIslands(Matrix3D& matrix, std::size_t startD, std::size_t startX, std::size_t startY)
{
    if (matrix[startD][startX][startY] == 0)
        std::cout << "WARNING: start position is not in set!" << std::endl;
    matrix[startD][startX][startY] = -1;

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



bool compress(Matrix3D& matrix, std::size_t lookingFor)
{
    std::cout << lookingFor << " ";
    auto newSize = lookingFor * 2;
    bool found = false;

    for (std::size_t d = 0; d <= HEIGHT - newSize; d += newSize)
    {
        for (std::size_t x = 0; x <= SIZE - newSize; x += newSize)
        {
            for (std::size_t y = 0; y <= SIZE - newSize; y += newSize)
            {
                if (
                (short)lookingFor == matrix[d][x][y] && //current cell
                (short)lookingFor == matrix[d][x + lookingFor][y] && //current layer
                (short)lookingFor == matrix[d][x + lookingFor][y + lookingFor] &&
                (short)lookingFor == matrix[d][x][y + lookingFor] &&

                (short)lookingFor == matrix[d + lookingFor][x][y] && //next layer
                (short)lookingFor == matrix[d + lookingFor][x + lookingFor][y] &&
                (short)lookingFor == matrix[d + lookingFor][x + lookingFor][y + lookingFor] &&
                (short)lookingFor == matrix[d + lookingFor][x][y + lookingFor]
                )
                {
                    found = true;
                    for (std::size_t q = 0; q < newSize; q++)
                        for (std::size_t r = 0; r < newSize; r++)
                            for (std::size_t s = 0; s < newSize; s++)
                                matrix[d + q][x + r][y + s] = (short)newSize;
                }
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

    unsigned int pointCount = 0, lineCount = 0, planeCount = 0, cubeCount = 0;
    for (short boxSize = 4; boxSize <= (short)std::min(SIZE, HEIGHT); boxSize *= 2)
    {
        std::cout << boxSize << " ";
        std::cout.flush();

        Point3D startPoint;
        while (true) //eliminate all boxes
        {
            startPoint = getPointOf(matrix, boxSize);
            //std::cout << "(" << startPoint.d_ << "," << startPoint.x_ << "," << startPoint.y_ << ")" << std::endl;
            if (startPoint.d_ < 0 || startPoint.x_ < 0 || startPoint.y_ < 0)
                break; //box not found

            int flag = 0;

            Bounds2D bounds = eliminateBoxOf(matrix, startPoint);
/*
            for (std::size_t d = 0; d < HEIGHT; d++)
            {
                for (std::size_t x = 0; x < SIZE; x++)
                {
                    for (std::size_t y = 0; y < SIZE; y++)
                        std::cout << matrix[d][x][y] << " ";
                    std::cout << std::endl;
                }
                std::cout << std::endl << std::endl;
            }
*/
            if (boxSize == 1) //dealing with points
            {
                auto diffD = bounds.second.d_ - bounds.first.d_;
                auto diffX = bounds.second.x_ - bounds.first.x_;
                auto diffY = bounds.second.y_ - bounds.first.y_;

                std::vector<int> sizes{diffD, diffX, diffY};
                std::sort(sizes.begin(), sizes.end());

                //smallest, middle, largest

                if (sizes[2] == 1)
                { //know that sizes[0] == sizes[1] == 1, so clearly a point
                    pointCount++;
                    flag = 1;
                }
                else if (sizes[1] == 1)
                { //know that sizes[2] is the only one > 1
                    lineCount++;
                    flag = 2;
                }
                else if (sizes[1] == 1)
                { //we know that sizes[1] > 1 && sizes[2] > 1, so a plane
                    planeCount++;
                    flag = 3;
                }
                else
                { //know all three are > 0, so clearly a cube
                    cubeCount++;
                    flag = 4;
                    std::cout << "Actually executed!" << std::endl;
                    std::cout << flag << " " << bounds.first.d_ <<
                        " " << bounds.first.x_ <<
                        " " << bounds.first.y_ <<
                        " " << bounds.second.d_ <<
                        " " << bounds.second.x_ <<
                        " " << bounds.second.y_ << std::endl;
                }
            }
            else //dealing with squares
            {
                cubeCount++;
                flag = 4;
            }

            fout << flag << " " << bounds.first.d_ <<
                " " << bounds.first.x_ <<
                " " << bounds.first.y_ <<
                " " << bounds.second.d_ <<
                " " << bounds.second.x_ <<
                " " << bounds.second.y_ << std::endl;
        }
    }

    std::cout << "complete, (" << cubeCount << "," << planeCount << "," <<
        lineCount << "," << pointCount << ") ";

    fout.close();
}



Point3D getPointOf(Matrix3D& matrix, short boxSize)
{
    for (std::size_t d = 0; d < HEIGHT; d++)
        for (std::size_t x = 0; x < SIZE; x++)
            for (std::size_t y = 0; y < SIZE; y++)
                if (matrix[d][x][y] == boxSize)
                    return Point3D((int)d, (int)x, (int)y);

    return Point3D(-1, -1, -1);
}



//the given point must be the upper left corner (minimum) of the box
Bounds2D eliminateBoxOf(Matrix3D& matrix, Point3D point)
{
    std::size_t sD = (std::size_t)point.d_, sX = (std::size_t)point.x_, sY = (std::size_t)point.y_;
    short boxSize = matrix[sD][sX][sY];
    matrix[sD][sX][sY] = -1;

    //look d+ and x+ and y+ direction due to algorithm in getPointOf

    std::size_t dLength = 0; //number of boxes in the x direction
    while (sD + (dLength + 1) * boxSize < HEIGHT && matrix[sD + (dLength + 1) * boxSize][sX][sY] == boxSize)
        dLength++;
    if (sD + (dLength + 1) * boxSize == HEIGHT && matrix[sD + (dLength + 1) * boxSize - 1][sX][sY] == boxSize)
        dLength++;

    std::size_t xLength = 0; //number of boxes in the x direction
    while (sX + (xLength + 1) * boxSize < SIZE && matrix[sD][sX + (xLength + 1) * boxSize][sY] == boxSize)
        xLength++;
    if (sX + (xLength + 1) * boxSize == SIZE && matrix[sD][sX + (xLength + 1) * boxSize - 1][sY] == boxSize)
        dLength++;

    std::size_t yLength = 0; //number of boxes in the x direction
    while (sY + (yLength + 1) * boxSize < SIZE && matrix[sD][sX][sY + (yLength + 1) * boxSize] == boxSize)
        yLength++;
    if (sY + (yLength + 1) * boxSize == SIZE && matrix[sD][sX][sY + (yLength + 1) * boxSize - 1] == boxSize)
        dLength++;

    //keep track of largest element
    std::size_t maxLength = 0;
    std::size_t* maxDimension = &dLength;
    if (dLength > maxLength)
    {
        maxLength = dLength;
        maxDimension = &dLength;
    }

    if (xLength > maxLength)
    {
        maxLength = xLength;
        maxDimension = &xLength;
    }

    if (yLength > maxLength)
    {
        maxLength = yLength;
        maxDimension = &yLength;
    }

    //if not a point, zero out all others
    if (&dLength != maxDimension)
        dLength = 0;
    if (&xLength != maxDimension)
        xLength = 0;
    if (&yLength != maxDimension)
        yLength = 0;

    //max specifies the maximum bounds in only one dimension
    Point3D max = Point3D((int)(sD + std::max((int)dLength, 1) * boxSize),
                          (int)(sX + std::max((int)xLength, 1) * boxSize),
                          (int)(sY + std::max((int)yLength, 1) * boxSize));

    //eliminate area inside bounds
    for (int d = point.d_; d < max.d_; d++)
        for (int x = point.x_; x < max.x_; x++)
            for (int y = point.y_; y < max.y_; y++)
                matrix[(std::size_t)d][(std::size_t)x][(std::size_t)y] = 0;

    return std::make_pair(point, max);
}
