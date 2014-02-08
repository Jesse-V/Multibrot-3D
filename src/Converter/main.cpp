
#include "main.hpp"
#include <fstream>
#include <sstream>
#include <iterator>
#include <iostream>

const std::size_t SIZE = 1024;
const std::size_t MAX_BOX_SIZE = 128;

int main(int argc, char** argv)
{
    Matrix2D matrix = readMatrix(std::string("multibrot,d=2.dat"));

    std::cout << "Writing... ";
    writeMatrix(matrix, std::string("image_pre.ppm"));
    std::cout << "done." << std::endl;

    std::cout << "Compressing at level ";
    for (std::size_t level = 2; level <= MAX_BOX_SIZE; level *= 2)
    {
        std::cout << level << " ";
        compress(matrix, level);
    }
    std::cout << " done." << std::endl;

    std::cout << "Writing... ";
    writeMatrix(matrix, std::string("image_post.ppm"));
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



void compress(Matrix2D& matrix, std::size_t level)
{
    for (std::size_t x = 0; x < SIZE - level; x += level)
    {
        for (std::size_t y = 0; y < SIZE - level; y += level)
        {
            if (matrix[x][y] == matrix[x + level][y] &&
                matrix[x][y] == matrix[x + level][y + level] &&
                matrix[x][y] == matrix[x][y + level]
            )
                for (std::size_t j = 0; j < level; j++)
                    for (std::size_t k = 0; k < level; k++)
                        matrix[x + j][y + k] = matrix[x][y] * 2;
        }
    }
}



void writeMatrix(Matrix2D& matrix, std::string filename)
{
    std::ofstream fout;
    fout.open(filename, std::ofstream::out);

    fout << "P2 " << SIZE << " " << SIZE << " 255" << std::endl;
    for (const auto &row : matrix)
    {
        for (const auto &cell : row)
        {
            if (cell == 0)
                fout << "255 ";
            else
                fout << (int)(cell / (float)MAX_BOX_SIZE * 255) << " ";
        }

        fout << std::endl;
    }
    fout.close();
}
