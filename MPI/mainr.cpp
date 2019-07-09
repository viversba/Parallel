#include <iostream>
#include <fstream>
#include <vector>
#include <assert.h>
#include <cmath>
#include <png++/png.hpp>
#include <mpi.h>

#define MSG_LENGTH 15

using namespace std;

typedef vector<double> Array;
typedef vector<Array> Matrix;
typedef vector<Matrix> Image;

Matrix filter;
Image image;
Image newImage;
int num_threads, id, from, to, newImageHeight, newImageWidth;

//calcular el número gaussiano para multiplicar por la matriz de pixeles "kernel"

Matrix getGaussian(int height, int width, double sigma)
{
    Matrix kernel(height, Array(width));
    double sum = 0.0;
    int i, j;

    for (i = 0; i < height; i++)
    {
        for (j = 0; j < width; j++)
        {
            kernel[i][j] = exp(-(i * i + j * j) / (2 * sigma * sigma)) / (2 * M_PI * sigma * sigma);
            sum += kernel[i][j];
        }
    }

    for (i = 0; i < height; i++)
    {
        for (j = 0; j < width; j++)
        {
            kernel[i][j] /= sum;
        }
    }

    return kernel;
}

//cargar imagen

Image loadImage(const char *filename)
{

    Image imageMatrix;
    try
    {
        png::image<png::rgb_pixel> image(filename);
        imageMatrix = Image(3, Matrix(image.get_height(), Array(image.get_width())));

        int h, w;
        for (h = 0; h < image.get_height(); h++)
        {
            for (w = 0; w < image.get_width(); w++)
            {
                imageMatrix[0][h][w] = image[h][w].red;
                imageMatrix[1][h][w] = image[h][w].green;
                imageMatrix[2][h][w] = image[h][w].blue;
            }
        }
    }
    catch (...)
    {
        if (newImageHeight == 0 || newImageWidth == 0)
            exit(-1);

        imageMatrix = Image(3, Matrix(newImageHeight, Array(newImageWidth)));
        int h, w;
        for (h = 0; h < newImageHeight; h++)
        {
            for (w = 0; w < newImageWidth; w++)
            {
                imageMatrix[0][h][w] = 0;
                imageMatrix[1][h][w] = 0;
                imageMatrix[2][h][w] = 0;
            }
        }
    }

    return imageMatrix;
}

//guardar imagen
void saveImage(Image &image, const char *filename)
{
    assert(image.size() == 3);

    Image tempImage = loadImage(filename);

    int height = image[0].size();
    int width = image[0][0].size();
    int x, y;

    png::image<png::rgb_pixel> imageFile(width, height);

    for (y = from; y < to; y++)
    {
        for (x = 0; x < width; x++)
        {
            tempImage[0][y][x] = image[0][y][x];
            tempImage[1][y][x] = image[1][y][x];
            tempImage[2][y][x] = image[2][y][x];
        }
    }

    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            imageFile[y][x].red = tempImage[0][y][x];
            imageFile[y][x].green = tempImage[1][y][x];
            imageFile[y][x].blue = tempImage[2][y][x];
        }
    }
    imageFile.write(filename);
}

//función aplicar filtro blur
Image applyFilter(int thread_id)
{
    assert(image.size() == 3 && filter.size() != 0);

    int height = image[0].size();
    int width = image[0][0].size();
    int filterHeight = filter.size();
    int filterWidth = filter[0].size();
    newImageHeight = height - filterHeight + 1;
    newImageWidth = width - filterWidth + 1;
    int d, i, j, h, w;

    from = (newImageHeight / num_threads) * thread_id;
    to = thread_id != num_threads - 1 ? from + (newImageHeight / num_threads) : newImageHeight;

    printf("Process number %d of %d processes. From %d to %d\n", thread_id, num_threads, from, to);

    for (d = 0; d < 3; d++)
    {
        for (i = from; i < to; i++)
        {
            for (j = 0; j < newImageWidth; j++)
            {
                for (h = i; h < i + filterHeight; h++)
                {
                    for (w = j; w < j + filterWidth; w++)
                    {
                        newImage[d][i][j] += filter[h - i][w - j] * image[d][h][w];
                    }
                }
            }
        }
    }

    return newImage;
}

int main(int argc, char **argv)
{

    assert(argc == 5 && "You must specify the input name, output name, number of threads and kernel size as parameters");

    int kernel;
    num_threads = atoi(argv[3]);
    kernel = atoi(argv[4]);

    newImageHeight = 0;
    newImageWidth = 0;

    filter = getGaussian(kernel, kernel, 10.0);
    cout << "Loading image..." << endl;
    image = loadImage(argv[1]);

    if (image[0].size() < num_threads || image[0][0].size() < num_threads)
    {
        cout << "The size of the image is less than the number of threads you are trying to use\nOnly 1 thread will be used";
        num_threads = 1;
    }

    int i, tag = 1, tasks, iam;
    char message[MSG_LENGTH];
    MPI_Status status;

    double arr[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    Array test(arr, arr + sizeof(arr) / sizeof(arr[0]));
    int size = test.size();

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &tasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &iam);

    num_threads = tasks;

    if (iam == 0)
    {
        test.clear();
        test.push_back(9);
        test.push_back(8);
        test.push_back(7);
        test.push_back(6);
        test.push_back(5);
        test.push_back(4);
        test.push_back(3);
        test.push_back(2);
        test.push_back(1);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Bcast(&test[0], size, MPI_FLOAT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Finalize();

    printf("node %d: of %d tasks\n", iam, tasks);

    newImage = Image(3, Matrix(image[0].size() - filter.size() + 1, Array(image[0][0].size() - filter[0].size() + 1)));

    cout << "Creathing threads..." << endl;
    cout << "Applying filter..." << endl;
    int args[num_threads];
    int r, *rv;

    applyFilter(iam);

    cout << "Saving image..." << endl;
    saveImage(newImage, argv[2]);
    cout << "Done!" << endl;
}