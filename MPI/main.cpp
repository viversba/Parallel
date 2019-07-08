#include <iostream>
#include <fstream>
#include <vector>
#include <assert.h>
#include <cmath>
#include <png++/png.hpp>
#include <pthread.h>
#include <mpi.h>

#define MSG_LENGTH 15

using namespace std;

typedef vector<double> Array;
typedef vector<Array> Matrix;
typedef vector<Matrix> Image;

Matrix filter;
Image image;
Image newImage;
int num_threads;

//calcular el número gaussiano para multiplicar por la matriz de pixeles "kernel"

Matrix getGaussian(int height, int width, double sigma)
{
    Matrix kernel(height, Array(width));
    double sum=0.0;
    int i,j;

    for (i=0 ; i<height ; i++) {
        for (j=0 ; j<width ; j++) {
            kernel[i][j] = exp(-(i*i+j*j)/(2*sigma*sigma))/(2*M_PI*sigma*sigma);
            sum += kernel[i][j];
        }
    }

    for (i=0 ; i<height ; i++) {
        for (j=0 ; j<width ; j++) {
            kernel[i][j] /= sum;
        }
    }

    return kernel;
}

//cargar imagen

Image loadImage(const char *filename)
{
    png::image<png::rgb_pixel> image(filename);
    Image imageMatrix(3, Matrix(image.get_height(), Array(image.get_width())));

    int h,w;
    for (h=0 ; h<image.get_height() ; h++) {
        for (w=0 ; w<image.get_width() ; w++) {
            imageMatrix[0][h][w] = image[h][w].red;
            imageMatrix[1][h][w] = image[h][w].green;
            imageMatrix[2][h][w] = image[h][w].blue;
        }
    }

    return imageMatrix;
}

//guardar imagen 
void saveImage(Image &image, const char *filename)
{
    assert(image.size()==3);

    int height = image[0].size();
    int width = image[0][0].size();
    int x,y;

    png::image<png::rgb_pixel> imageFile(width, height);

    for (y=0 ; y<height ; y++) {
        for (x=0 ; x<width ; x++) {
            imageFile[y][x].red = image[0][y][x];
            imageFile[y][x].green = image[1][y][x];
            imageFile[y][x].blue = image[2][y][x];
        }
    }
    imageFile.write(filename);
}

//función aplicar filtro blur
Image applyFilter(Image &image, Matrix &filter){
    
    assert(image.size()==3 && filter.size()!=0);

    int height = image[0].size();
    int width = image[0][0].size();
    int filterHeight = filter.size();
    int filterWidth = filter[0].size();
    int newImageHeight = height-filterHeight+1;
    int newImageWidth = width-filterWidth+1;
    int d,i,j,h,w;

    Image newImage(3, Matrix(newImageHeight, Array(newImageWidth)));

    for (d=0 ; d<3 ; d++) {
        for (i=0 ; i<newImageHeight ; i++) {
            for (j=0 ; j<newImageWidth ; j++) {
                for (h=i ; h<i+filterHeight ; h++) {
                    for (w=j ; w<j+filterWidth ; w++) {
                        newImage[d][i][j] += filter[h-i][w-j]*image[d][h][w];
                    }
                }
            }
        }
    }

    return newImage;
}

//aplicar filtro y generar imagen nueva

Image applyFilter(Image &image, Matrix &filter, int times)
{
    Image newImage = image;
    for(int i=0 ; i<times ; i++) {
        newImage = applyFilter(newImage, filter);
    }
    return newImage;
}

void *thread_function(void *ap){
    
    assert(image.size()==3 && filter.size()!=0);
    long thread_id = (long)ap;
    
    int height = image[0].size();
    int width = image[0][0].size();
    int filterHeight = filter.size();
    int filterWidth = filter[0].size();
    int newImageHeight = height-filterHeight+1;
    int newImageWidth = width-filterWidth+1;
    int d,i,j,h,w;
    
    int from = (newImageHeight / num_threads)*thread_id;
    int to = thread_id != num_threads-1 ? from + (newImageHeight / num_threads) : newImageHeight;
    
    for (d=0 ; d<3 ; d++) {
        for (i=from ; i<to ; i++) {
            for (j=0 ; j<newImageWidth ; j++) {
                for (h=i ; h<i+filterHeight ; h++) {
                    for (w=j ; w<j+filterWidth ; w++) {
                        newImage[d][i][j] += filter[h-i][w-j]*image[d][h][w];

    long thread_id = (long)ap;
    
    int height = image[0].size();
    int width = image[0][0].size();
    int filterHeight = filter.size();
    int filterWidth = filter[0].size();
    int newImageHeight = height-filterHeight+1;
    int newImageWidth = width-filterWidth+1;
    int d,i,j,h,w;
    
    int from = (newImageHeight / num_threads)*thread_id;
    int to = thread_id != num_threads-1 ? from + (newImageHeight / num_threads) : newImageHeight;
    
    for (d=0 ; d<3 ; d++) {
        for (i=from ; i<to ; i++) {
            for (j=0 ; j<newImageWidth ; j++) {
                for (h=i ; h<i+filterHeight ; h++) {
                    for (w=j ; w<j+filterWidth ; w++) {
                        newImage[d][i][j] += filter[h-i][w-j]*image[d][h][w];
                    }
                }
            }
        }
    }

    pthread_exit(NULL);
}

int main(int argc, char** argv)
{
    assert(argc == 5 && "You must specify the input name, output name, number of threads and kernel size as parameters");
    
    int kernel;
    num_threads = atoi(argv[3]);
    kernel = atoi(argv[4]);
    
    filter = getGaussian(kernel, kernel, 10.0);
    cout << "Loading image..." << endl;
    image = loadImage(argv[1]);
    
    if(image[0].size() < num_threads || image[0][0].size() < num_threads){
        cout << "The size of the image is less than the number of threads you are trying to use\nOnly 1 thread will be used";
        num_threads = 1;
    }
    
    int i, tag=1, tasks, iam;
    char message[MSG_LENGTH];
    MPI_Status status;
 
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &tasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &iam);
    if (iam == 0) {
 
        strcpy(message, "Hello, world!");
        for (i=1; i<tasks; i++)
            MPI_Send(message, MSG_LENGTH, MPI_CHAR, i, tag, MPI_COMM_WORLD);
    }
    else {
 
        MPI_Recv(message, MSG_LENGTH, MPI_CHAR, 0, tag,
        MPI_COMM_WORLD, &status);
    }
    printf("node %d: %s\n", iam, message);
 
    MPI_Finalize();
    
    newImage = Image(3, Matrix(image[0].size() - filter.size() + 1, Array(image[0][0].size() - filter[0].size() + 1)));
    
    cout << "Creathing threads..." << endl;
    cout << "Applying filter..." << endl;
    int args[num_threads];
    pthread_t hilo[num_threads];
    int r,*rv;

    //thread creation
    for (i = 0; i < num_threads; i++)
    {
        args[i] = i;
        r = pthread_create(&hilo[i], NULL, thread_function, (void *)i);
        if (r != 0)
        {
            perror("can't create thread");
            exit(-1);
        }
    }
    
    //thread opening
    for (i = 0; i < num_threads; i++)
    {
        r = pthread_join(hilo[i], (void **)&rv);
        if (r != 0)
        {
            perror("can't join thread");
            exit(-1);
        }
    }
    
    cout << "Saving image..." << endl;
    saveImage(newImage, argv[2]);
    cout << "Done!" << endl;
}