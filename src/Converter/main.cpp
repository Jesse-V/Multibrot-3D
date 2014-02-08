
#include "main.hpp"
#include <fstream>
#include <sstream>
#include <iterator>
#include <iostream>

const std::size_t SIZE = 1024;
const std::size_t MAX_BOX_SIZE = 256;

int main(int argc, char** argv)
{
    Matrix2D matrix = readMatrix(std::string("multibrot,d=2.dat"));

    std::cout << "Writing pre image... ";
    writeMatrix(matrix, std::string("image_pre.ppm"), 1);
    std::cout << "done." << std::endl;

    std::cout << "Compressing. Looking for boxes of size ";
    short max = 0;
    for (std::size_t lookFor = 1; lookFor < MAX_BOX_SIZE; lookFor *= 2)
    {
        std::cout << lookFor << " ";
        auto val = compress(matrix, lookFor);
        if (val > max)
            max = val;
    }
    std::cout << " done." << std::endl;

    //for (std::size_t x = 0; x <= SIZE - 32; x += 32)
    //    for (std::size_t y = 0; y < SIZE; y++)
    //        matrix[x][y] = 0;

    std::cout << "Writing post image... ";
    writeMatrix(matrix, std::string("image_post.ppm"), max);
    std::cout << "done." << std::endl;

    std::cout << "Writing geometry... ";
    writeGeometry(matrix, std::string("geometry.dat"), max);
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



short compress(Matrix2D& matrix, std::size_t lookingFor)
{
    size_t newSize = lookingFor * 2;
    bool found = false;

    for (std::size_t x = 0; x <= SIZE - newSize; x += newSize)
    {
        for (std::size_t y = 0; y <= SIZE - newSize; y += newSize)
        {
            if (matrix[x][y] == lookingFor)
            {
                if (lookingFor == matrix[x + lookingFor][y] &&
                    lookingFor == matrix[x + lookingFor][y + lookingFor] &&
                    lookingFor == matrix[x][y + lookingFor]
                )
                    for (std::size_t j = 0; j < newSize; j++)
                        for (std::size_t k = 0; k < newSize; k++)
                            matrix[x + j][y + k] = newSize;

                found = true;
            }
        }
    }

    return found ? lookingFor : 0;
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
            else
                fout << gray << " " << gray << " " << gray << " ";
        }

        fout << std::endl;
    }
    fout.close();
}
