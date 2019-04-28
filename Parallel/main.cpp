/*
#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;
int main(int argc, char** argv)
{
    // Read the image file
    Mat image = imread("/Users/nicolasviveros/Desktop/test.jpg");
    
    // Check for failure
    if (image.empty())
    {
        cout << "Could not open or find the image" << endl;
        cin.get(); //wait for any key press
        return -1;
    }
    
    //Estas dos lineas son el código para crear una ventana
//    String windowName = "B&W"; //Name of the window
//    namedWindow(windowName); // Create a window
    
    int rows = image.rows;
    int cols = image.cols;
    
    uint8_t* pixelPtr = (uint8_t*)image.data;
    int cn = image.channels();
    
    for(int i=0;i<rows;i++){
        for(int j=0;j<cols;j++){
            
//            if(i>rows/2){
//                pixelPtr[i*cols*cn + j*cn + 0] = 0; // B
//                pixelPtr[i*cols*cn + j*cn + 1] = 0; // G
//                pixelPtr[i*cols*cn + j*cn + 2] = 0; // R
//            }
            
            int b = pixelPtr[i*cols*cn + j*cn + 0]; // B
            int g = pixelPtr[i*cols*cn + j*cn + 1]; // G
            int r = pixelPtr[i*cols*cn + j*cn + 2]; // R
            int avr = (r+b+g)/3;
            pixelPtr[i*cols*cn + j*cn + 0] = avr; // B
            pixelPtr[i*cols*cn + j*cn + 1] = avr; // G
            pixelPtr[i*cols*cn + j*cn + 2] = avr; // R
//            printf("The RGB is: %d %d %d\n", r,g,b);
        }
    }
    imwrite("/Users/nicolasviveros/Desktop/testbw.jpg",image);
    
//Estas dos lineas son el código para mostrar la imagen en la ventana y cerrar la ventana al presionar culquier tecla
//    imshow(windowName, image); // Show our image inside the created window.
//    waitKey(0); // Wait for any keystroke in the window
//    destroyWindow(windowName); //destroy the created window
    
    return 0;
}



#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

using namespace cv;

/// Global variables
Mat src, dst, tmp;
char const* window_name = "Pyramids Demo";



int main( int argc, char** argv )
{


  /// General instructions
  printf( "\n Zoom In-Out demo  \n " );
  printf( "------------------ \n" );
  printf( " * [u] -> Zoom in  \n" );
  printf( " * [d] -> Zoom out \n" );
  printf( " * [ESC] -> Close program \n \n" );



  /// Test image - Make sure it s divisible by 2^{n}
  src = imread( "/home/brehynnermraz/Documentos/PARALELA/Parallel/Parallel/input.jpeg" );
  if( !src.data )
    { printf(" No data! -- Exiting the program \n");
      return -1; }

  tmp = src;
  dst = tmp;

  /// Create window
  namedWindow( window_name, CV_WINDOW_AUTOSIZE );
  imshow( window_name, dst );

  pyrDown( tmp, dst, Size( tmp.cols/2, tmp.rows/2 ) );
  pyrUp( tmp, dst, Size( tmp.cols*2, tmp.rows*2 ));

  
  imshow( window_name, dst );
  tmp = dst;


  /// Loop
  while( true )
  {
    int c;
    c = waitKey(10);

    if( (char)c == 27 )
      { break; }
    if( (char)c == 'u' )
      { pyrUp( tmp, dst, Size( tmp.cols*2, tmp.rows*2 ) );
        printf( "** Zoom In: Image x 2 \n" );
      }
    else if( (char)c == 'd' )
     { pyrDown( tmp, dst, Size( tmp.cols/2, tmp.rows/2 ) );
       printf( "** Zoom Out: Image / 2 \n" );
     }

    imshow( window_name, dst );
    tmp = dst;
  }



  return 0;
}
*/

#include <iostream>
#include <vector>
#include <assert.h>
#include <cmath>
#include <png++/png.hpp>
#include <pthread.h>

#define NUM_THREADS 4

using namespace std;

typedef vector<double> Array;
typedef vector<Array> Matrix;
typedef vector<Matrix> Image;

Matrix filter;
Image image;
Image newImage;

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
    
    int from = (newImageHeight / NUM_THREADS)*thread_id;
    int to = thread_id != NUM_THREADS-1 ? from + (newImageHeight / NUM_THREADS) : newImageHeight;
    
//    cout << "Hello world! from thread " << thread_id << endl;
//    cout << image[0].size() << " x " << image[0][0].size() << endl;
//    cout << "I go from " << from << " to " << to << endl;
    
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

int main()
{
    filter = getGaussian(30, 30, 10.0);
    int numthreads;

    cout << "Loading image..." << endl;
    image = loadImage("/Users/nicolasviveros/Documents/Parallel/Parallel/input.jpeg");
    if(image[0].size() < NUM_THREADS || image[0][0].size() < NUM_THREADS){
        cout << "The size of the image is less than the number of threads you are trying to use\nOnly 1 thread will be used";
        numthreads = 1;
    }
    else{
        numthreads = NUM_THREADS;
    }
    
    newImage = Image(3, Matrix(image[0].size() - filter.size() + 1, Array(image[0][0].size() - filter[0].size() + 1)));
    
    cout << "Creathing threads..." << endl;
    cout << "Applying filter..." << endl;
    int args[numthreads];
    pthread_t hilo[numthreads];
    int i,r,*rv;
    
    //thread creation
    for (i = 0; i < numthreads; i++)
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
    for (i = 0; i < numthreads; i++)
    {
        r = pthread_join(hilo[i], (void **)&rv);
        if (r != 0)
        {
            perror("can't join thread");
            exit(-1);
        }
    }
    
    cout << "Saving image..." << endl;
    saveImage(newImage, "newImage.png");
    cout << "Done!" << endl;
}
