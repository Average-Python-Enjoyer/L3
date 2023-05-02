#ifndef INC_5_1_FUNCTIONS_H
#define INC_5_1_FUNCTIONS_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#pragma pack(push, 1)

typedef struct {
    unsigned short bfType;
    unsigned int bfSize;
    unsigned short bfReserved1;
    unsigned short bfReserved2;
    unsigned int bfOffBits;
} BITMAPFILEHEADER;
typedef struct {
    unsigned int biSize;
    int biWidth;
    int biHeight;
    unsigned short biPlanes;
    unsigned short biBitCount;
    unsigned int biCompression;
    unsigned int biSizeImage;
    int biXPelsPerMeter;
    int biYPelsPerMeter;
    unsigned int biClrUsed;
    unsigned int biClrImportant;
} BITMAPINFOHEADER;
typedef struct {
    unsigned char rgbtBlue;
    unsigned char rgbtGreen;
    unsigned char rgbtRed;
} RGBTRIPLE;
typedef struct {
    RGBTRIPLE* temp_pixels;
    RGBTRIPLE* pixels;
    int width;
    int height;
    int row_size;
    int filter_size;
    int half_filter_size;
} MedianFilterData;
typedef struct {
    RGBTRIPLE* temp_pixels;
    int width;
    int height;
    int row_size;
    int* window_index;
    unsigned char** window_red;
    unsigned char** window_green;
    unsigned char** window_blue;
} MedianFilterWindowData;
#pragma pack(pop)
void negative(RGBTRIPLE* pixel);
void black_and_white(RGBTRIPLE* pixel);
void apply_negative(RGBTRIPLE* pixels, int width, int height, int row_size);
void apply_black_and_white(RGBTRIPLE* pixels, int width, int height, int row_size);
void save_image(const char* file_name, BITMAPFILEHEADER file_header, BITMAPINFOHEADER info_header, RGBTRIPLE* pixels, int row_size);
void apply_gamma_correction(RGBTRIPLE* pixels, int width, int height, int row_size, float gamma);
int compare(const void* a, const void* b);
void apply_median_filter(RGBTRIPLE* pixels, int width, int height, int row_size, int filter_size);
void apply_median_filter_to_pixel(MedianFilterData* data, int i, int j);
int read_image(const char* input_file_name, BITMAPFILEHEADER* file_header, BITMAPINFOHEADER* info_header, RGBTRIPLE** pixels, int* row_size);
int present_menu();
void add_pixel_to_window(MedianFilterWindowData* data, int i, int j, int k, int l);
void process_choice(int choice, BITMAPFILEHEADER file_header, BITMAPINFOHEADER info_header, RGBTRIPLE* pixels, const RGBTRIPLE* original_pixels, int row_size);
#endif //INC_5_1_FUNCTIONS_H