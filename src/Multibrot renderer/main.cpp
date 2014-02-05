
#include "main.hpp"
#include "Chunk.struct"
//#include <mpi.h>
#include <queue>
#include <complex>
#include <fstream>
#include <iostream>


const unsigned int CHUNK_SIZE = 32; //seems to be the fastest factor
const unsigned int IMAGE_SIZE = 1024; //size of the resulting image, N * N
const unsigned int MAX_DEPTH = 1024; //max iterations to follow the point
const float D = 32;


int main(int argc, char** argv)
{
    /*int processors, rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &processors);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);*/

    std::queue<Chunk> chunks;
    for (unsigned int x = 0; x < IMAGE_SIZE; x += CHUNK_SIZE)
        for (unsigned int y = 0; y < IMAGE_SIZE; y += CHUNK_SIZE)
            chunks.push(Chunk(x, y, x + CHUNK_SIZE, y + CHUNK_SIZE));

    Matrix2D matrix;
    initializeMatrix(matrix);

    while (!chunks.empty())
    {
        //std::cout << chunks.size() << std::endl;
        render(chunks.front(), matrix);
        chunks.pop();
    }

    std::stringstream filename;
    filename << "matrix" << D << ".ppm" << std::endl;
    writeMatrix(matrix, filename.str());

    /*
        divide image into chunks. Add all chunks to a queue.
        Chunk object
            minimum XY
            maximum XY
        For each chunk, check boundaries.
            If all black, then chunk is black
            else complete rest of the chunk manually
        Print image
    */

    //std::cout << "Program complete for rank " << rank << std::endl;

    //MPI_Finalize();
    return EXIT_SUCCESS;
    //return EXIT_FAILURE;
}



void initializeMatrix(Matrix2D& matrix)
{
    for (std::size_t j = 0; j < IMAGE_SIZE; j++)
        matrix.push_back(std::vector<bool>(IMAGE_SIZE, 0));
}



void render(Chunk chunk, Matrix2D& matrix)
{
    if (bordersAreInside(chunk))
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
            matrix[chunk.minX_ + x][chunk.minY_ + y] = isInsideFractal(fx, fy);
        }
    }
}



bool bordersAreInside(Chunk chunk)
{
    bool black = true;

    for (unsigned int j = 0; black && j < CHUNK_SIZE; j++)
        if (!isInsideFractal(toFractalSpace(chunk.minX_), toFractalSpace(chunk.minY_ + j)))
            black = false;

    for (unsigned int j = 0; black && j < CHUNK_SIZE; j++)
        if (!isInsideFractal(toFractalSpace(chunk.maxX_), toFractalSpace(chunk.minY_ + j)))
            black = false;

    for (unsigned int j = 0; black && j < CHUNK_SIZE; j++)
        if (!isInsideFractal(toFractalSpace(chunk.minX_ + j), toFractalSpace(chunk.minY_)))
            black = false;

    for (unsigned int j = 0; black && j < CHUNK_SIZE; j++)
        if (!isInsideFractal(toFractalSpace(chunk.minX_ + j), toFractalSpace(chunk.maxY_)))
            black = false;

    return black;
}



float toFractalSpace(unsigned int pixel)
{
    return 4.0f * pixel / IMAGE_SIZE - 2;
}



bool isInsideFractal(float x, float y)
{
    std::complex<float> c(x, y);
    std::complex<float> z(0, 0);

    unsigned int i;
    for (i = 0; norm(z) < 4 && i < MAX_DEPTH; i++)
        z = pow(z, D) + c;

    return i == MAX_DEPTH;
}



void writeMatrix(Matrix2D& matrix, std::string filename)
{
    std::ofstream fout;
    fout.open(filename, std::ofstream::out);
    fout << "P2 " << IMAGE_SIZE << " " << IMAGE_SIZE << " 255" << std::endl;

    for (const auto &row : matrix)
    {
        for (const auto &cell : row)
        {
            int value = cell ? 0 : 100;
            fout << value << " ";
        }
        fout << std::endl;
    }

    fout.close();
}
