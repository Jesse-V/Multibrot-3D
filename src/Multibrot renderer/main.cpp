
#include "main.hpp"
#include "Chunk.struct"
#include <queue>
#include <complex>
#include <fstream>
#include <iostream>


const unsigned int CHUNK_SIZE = 32; //seems to be the fastest factor
const unsigned int IMAGE_SIZE = 1024; //size of the resulting image, N * N
const unsigned int MAX_DEPTH = 1024; //max iterations to follow the point
const float MAX_HEIGHT = 1024;


int main(int argc, char** argv)
{
    for (int height = 0; height < MAX_HEIGHT; height++)
    {
        float d = (height / (float)MAX_HEIGHT) * 32 + 1;

        std::cout << "Processing " << height << " / " << MAX_HEIGHT << " (" << d << ") ... ";

        std::queue<Chunk> chunks;
        for (unsigned int x = 0; x < IMAGE_SIZE; x += CHUNK_SIZE)
            for (unsigned int y = 0; y < IMAGE_SIZE; y += CHUNK_SIZE)
                chunks.push(Chunk(x, y, x + CHUNK_SIZE, y + CHUNK_SIZE));

        Matrix2D matrix;
        initializeMatrix(matrix);

        while (!chunks.empty())
        {
            //std::cout << chunks.size() << std::endl;
            render(chunks.front(), matrix, d);
            chunks.pop();
        }

        std::cout << "done" << std::endl;

        std::stringstream filename;
        filename << "multibrot,d=" << d << ".dat";
        writeMatrix(matrix, filename.str());
    }

    std::cout << "Program complete." << std::endl;

    return EXIT_SUCCESS;
}



void initializeMatrix(Matrix2D& matrix)
{
    for (std::size_t j = 0; j < IMAGE_SIZE; j++)
        matrix.push_back(std::vector<bool>(IMAGE_SIZE, 0));
}



/*
    For each chunk, check boundaries.
        If all black, then chunk is black
        else complete rest of the chunk manually
*/
void render(Chunk chunk, Matrix2D& matrix, float d)
{
    if (bordersAreInside(chunk, d))
    {
        for (unsigned int x = 0; x < CHUNK_SIZE; x++)
            for (unsigned int y = 0; y < CHUNK_SIZE; y++)
                matrix[chunk.minX_ + x][chunk.minY_ + y] = true;
        return;
    }

    for (unsigned int x = 0; x < CHUNK_SIZE; x++)
    {
        for (unsigned int y = 0; y < CHUNK_SIZE; y++)
        {
            float fx = toFractalSpace(chunk.minX_ + x);
            float fy = toFractalSpace(chunk.minY_ + y);
            matrix[chunk.minX_ + x][chunk.minY_ + y] = isInsideFractal(fx, fy, d);
        }
    }
}



bool bordersAreInside(Chunk chunk, float d)
{
    bool black = true;

    for (unsigned int j = 0; black && j < CHUNK_SIZE; j++)
        if (!isInsideFractal(toFractalSpace(chunk.minX_), toFractalSpace(chunk.minY_ + j), d))
            black = false;

    for (unsigned int j = 0; black && j < CHUNK_SIZE; j++)
        if (!isInsideFractal(toFractalSpace(chunk.maxX_), toFractalSpace(chunk.minY_ + j), d))
            black = false;

    for (unsigned int j = 0; black && j < CHUNK_SIZE; j++)
        if (!isInsideFractal(toFractalSpace(chunk.minX_ + j), toFractalSpace(chunk.minY_), d))
            black = false;

    for (unsigned int j = 0; black && j < CHUNK_SIZE; j++)
        if (!isInsideFractal(toFractalSpace(chunk.minX_ + j), toFractalSpace(chunk.maxY_), d))
            black = false;

    return black;
}



float toFractalSpace(unsigned int pixel)
{
    return 4.0f * pixel / IMAGE_SIZE - 2;
}



bool isInsideFractal(float x, float y, float d)
{
    std::complex<float> c(x, y);
    std::complex<float> z(0, 0);

    unsigned int i;
    for (i = 0; norm(z) < 4 && i < MAX_DEPTH; i++)
        z = pow(z, d) + c;

    return i == MAX_DEPTH;
}



void writeMatrix(Matrix2D& matrix, std::string filename)
{
    std::ofstream fout;
    fout.open(filename, std::ofstream::out);

    for (const auto &row : matrix)
    {
        bool state = false;
        int count = 0;

        for (int index = 0; index < row.size(); index++)
        {
            if (row[index] == state)
                count++;
            else
            {
                fout << count << " ";
                count = 0;
                state = !state;
            }
        }

        fout << count << " ";
        fout << std::endl;
    }

    fout.close();
}
