#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <vector>
#include <cmath>
#include <time.h>
#include <helper_cuda.h>
#define PNG_DEBUG 3
#include <png.h>

using std::vector;

typedef vector<double> Array;
typedef vector<Array> Matrix;
typedef vector<Matrix> Image;

Matrix filter;
Image image;
Image newImage;
int num_threads;

void abort_(const char * s, ...)
{
        va_list args;
        va_start(args, s);
        vfprintf(stderr, s, args);
        fprintf(stderr, "\n");
        va_end(args);
        abort();
}

int GetDeviceCount(){
    int deviceCount = 0;
    cudaError_t error_id = cudaGetDeviceCount(&deviceCount);
    return deviceCount;
}

int x, y;

int width, height;
png_byte color_type;
png_byte bit_depth;

png_structp png_ptr;
png_infop info_ptr;
int number_of_passes;
png_bytep * row_pointers;

void read_png_file(char* file_name)
{
        char header[8];    // 8 is the maximum size that can be checked

        /* open file and test for it being a png */
        FILE *fp = fopen(file_name, "rb");
        if (!fp)
                abort_("[read_png_file] File %s could not be opened for reading", file_name);
        fread(header, 1, 8, fp);
        //if (png_sig_cmp(header, 0, 8))
        //        abort_("[read_png_file] File %s is not recognized as a PNG file", file_name);


        /* initialize stuff */
        png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

        if (!png_ptr)
                abort_("[read_png_file] png_create_read_struct failed");

        info_ptr = png_create_info_struct(png_ptr);
        if (!info_ptr)
                abort_("[read_png_file] png_create_info_struct failed");

        if (setjmp(png_jmpbuf(png_ptr)))
                abort_("[read_png_file] Error during init_io");

        png_init_io(png_ptr, fp);
        png_set_sig_bytes(png_ptr, 8);

        png_read_info(png_ptr, info_ptr);

        width = png_get_image_width(png_ptr, info_ptr);
        height = png_get_image_height(png_ptr, info_ptr);
        color_type = png_get_color_type(png_ptr, info_ptr);
        bit_depth = png_get_bit_depth(png_ptr, info_ptr);

        number_of_passes = png_set_interlace_handling(png_ptr);
        png_read_update_info(png_ptr, info_ptr);


        /* read file */
        if (setjmp(png_jmpbuf(png_ptr)))
                abort_("[read_png_file] Error during read_image");

        row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * height);
        for (y=0; y<height; y++)
                row_pointers[y] = (png_byte*) malloc(png_get_rowbytes(png_ptr,info_ptr));

        png_read_image(png_ptr, row_pointers);

        fclose(fp);
}


void write_png_file(char* file_name)
{
        /* create file */
        FILE *fp = fopen(file_name, "wb");
        if (!fp)
                abort_("[write_png_file] File %s could not be opened for writing", file_name);


        /* initialize stuff */
        png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

        if (!png_ptr)
                abort_("[write_png_file] png_create_write_struct failed");

        info_ptr = png_create_info_struct(png_ptr);
        if (!info_ptr)
                abort_("[write_png_file] png_create_info_struct failed");

        if (setjmp(png_jmpbuf(png_ptr)))
                abort_("[write_png_file] Error during init_io");

        png_init_io(png_ptr, fp);


        /* write header */
        if (setjmp(png_jmpbuf(png_ptr)))
                abort_("[write_png_file] Error during writing header");

        png_set_IHDR(png_ptr, info_ptr, width, height,
                     bit_depth, color_type, PNG_INTERLACE_NONE,
                     PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

        png_write_info(png_ptr, info_ptr);


        /* write bytes */
        if (setjmp(png_jmpbuf(png_ptr)))
                abort_("[write_png_file] Error during writing bytes");

        png_write_image(png_ptr, row_pointers);


        /* end write */
        if (setjmp(png_jmpbuf(png_ptr)))
                abort_("[write_png_file] Error during end of write");

        png_write_end(png_ptr, NULL);

        /* cleanup heap allocation */
        for (y=0; y<height; y++)
                free(row_pointers[y]);
        free(row_pointers);

        fclose(fp);
}

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

__global__ void vectorAdd(double *image_d1, double *kernel, double *output, int width, int height, int kernelSize, int totalThreads)
{
    int id = blockDim.x * blockIdx.x + threadIdx.x;
    //printf("blockDim is: %d blockId is: %d threadId is: %d id is: %d\n", blockDim.x, blockIdx.x, threadIdx.x, id);
    //printf("width: %d height: %d ", width, height);

    int filterHeight = kernelSize;
    int filterWidth = kernelSize;
    int newImageHeight = height-kernelSize+1;
    //int newImageHeight = height;
    int newImageWidth = width-kernelSize+1;
    int d,i,j,h,w;
    
    //int fromY = (newImageHeight / totalThreads)*id;
    //int toY = id != totalThreads-1 ? fromY + (newImageHeight / totalThreads) : newImageHeight;
    
    //int fromX = (newImageWidth / totalThreads)*id;
    //int toX = id != totalThreads-1 ? fromX + (newImageWidth / totalThreads) : newImageWidth;
    
    int fromY = ((newImageHeight + totalThreads - 1) / totalThreads)*id;
    int toY = id != totalThreads-1 ? fromY + ((newImageHeight + totalThreads - 1) / totalThreads) : newImageHeight;

    int fromX = ((newImageWidth + totalThreads - 1) / totalThreads)*id;
    int toX = id != totalThreads-1 ? fromX + ((newImageWidth + totalThreads - 1) / totalThreads) : newImageWidth;
    
    //printf("ID: %d #Blocks %d From %d To %d\n",id,(toX-fromX), fromX, toX);
    
    if(toX > newImageWidth) return;
    
    int numElements = sizeof(double) * 4 * newImageHeight * newImageWidth;
        
    for (i=0; i<newImageHeight ; i++) {
        for (j=fromX ; j<toX ; j++) {
            for(int k=0; k<4; k++){
                output[i*newImageWidth*4 + j*4 + k] = 0;
                for (h=i ; h<i+filterHeight ; h++) {
                    for (w=j ; w<j+filterWidth ; w++) {
                        output[i*newImageWidth*4 + j*4 + k] += kernel[(h-i) * kernelSize + (w-j)]*image_d1[h*width*4 + w*4 + k];
                    }
                }
            }
        }
    }
    
    printf("");
}


void process_file(int kernel,int blocks,int threads)
{
    if (png_get_color_type(png_ptr, info_ptr) == PNG_COLOR_TYPE_RGB)
      abort_("[process_file] input file is PNG_COLOR_TYPE_RGB but must be PNG_COLOR_TYPE_RGBA "
        "(lacks the alpha channel)");

    if (png_get_color_type(png_ptr, info_ptr) != PNG_COLOR_TYPE_RGBA)
      abort_("[process_file] color_type of input file must be PNG_COLOR_TYPE_RGBA (%d) (is %d)",
        PNG_COLOR_TYPE_RGBA, png_get_color_type(png_ptr, info_ptr));
    
    int device;
    cudaGetDeviceCount(&device);
    cudaSetDevice(device);
    cudaDeviceProp deviceProp;
    cudaGetDeviceProperties(&deviceProp, device);
    
    int err;
    
    int multiprocessorNum =  deviceProp.multiProcessorCount;
    int cores_mp = _ConvertSMVer2Cores(deviceProp.major, deviceProp.minor);
    int cores = _ConvertSMVer2Cores(deviceProp.major, deviceProp.minor) *deviceProp.multiProcessorCount;
    int newWidth = (width-kernel + 1);
    int newHeight = (height-kernel + 1);
    
    int size = sizeof(double) * 4 * width * height;
    int kernelSize = sizeof(double) * kernel * kernel;
    int outputSize = sizeof(double) * 4 * newHeight * newWidth;
    
    double *image_h1 = (double *)malloc(size);
    double *image_d1 = NULL;
    err = cudaMalloc((void **)&image_d1, size);
    double *kernel_h1 = (double *)malloc(kernelSize);
    double *kernel_d1 = NULL;
    err = cudaMalloc(&kernel_d1, kernelSize);

    double *output_h = (double *)malloc(outputSize);
    double *output_d = NULL;
    err = cudaMalloc((void **)&output_d, outputSize);
   
    // Initialize the HOST image
    for (y=0; y<height; y++) {
        png_byte* row = row_pointers[y];
        for (x=0; x<width; x++) {   
            png_byte* ptr = &(row[x*4]);
            for(int i=0; i<4; i++){
                image_h1[y*width*4 + x*4 + i] = (double)ptr[i];
            }
        }
    }
    
    // Copy the HOST image to DEVICE image
    err = cudaMemcpy(image_d1, image_h1, size, cudaMemcpyHostToDevice);
    
    Matrix filter = getGaussian(kernel, kernel, 10.0);
    kernel_h1 = new double[kernel];
    for(int i=0; i<kernel; i++){
        for(int j=0; j<kernel; j++){ 
            kernel_h1[i*kernel + j] = filter[i][j];
        }
    }

    err = cudaMemcpy(kernel_d1, kernel_h1, kernelSize, cudaMemcpyHostToDevice);
    
    // vectorAdd<<<multiprocessorNum, cores_mp>>>(image_d1, kernel_d1, output_d, width, height, kernel);
    vectorAdd<<<blocks, threads>>>(image_d1, kernel_d1, output_d, width, height, kernel, blocks*threads);
    
    err = cudaMemcpy(image_h1, image_d1, size, cudaMemcpyDeviceToHost);
    err = cudaMemcpy(output_h, output_d, outputSize, cudaMemcpyDeviceToHost);
    
    for (y=0; y<newHeight; y++) {
        png_byte* row = row_pointers[y];
        for (x=0; x<newWidth; x++) {   
            png_byte* ptr = &(row[x*4]);
            for(int i=0; i<4; i++){
                 ptr[i] = output_h[y*newWidth*4 + x*4 + i];
            }
        }
    }
}

int main(int argc, char **argv)
{
    if (argc != 6)
      abort_("Usage: program_name <file_in> <file_out> <kernel size> <blocks number> <threads number>");
    
    int kernel = atoi(argv[3]);
    int blocks = atoi(argv[4]);
    int threads = atoi(argv[5]);
    
  	read_png_file(argv[1]);
    process_file(kernel, blocks, threads);
    write_png_file(argv[2]);

}