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
