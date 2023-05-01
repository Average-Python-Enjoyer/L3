#define _CRT_SECURE_NO_WARNINGS
#include "func.h"


void negative(RGBTRIPLE* pixel) {
    pixel->rgbtBlue = ~pixel->rgbtBlue;
    pixel->rgbtGreen = ~pixel->rgbtGreen;
    pixel->rgbtRed = ~pixel->rgbtRed;
}
void black_and_white(RGBTRIPLE* pixel) {
    float gray = (pixel->rgbtBlue + pixel->rgbtGreen + pixel->rgbtRed) / 3.0f;
    pixel->rgbtBlue = gray;
    pixel->rgbtGreen = gray;
    pixel->rgbtRed = gray;
}
void apply_negative(RGBTRIPLE* pixels, int width, int height, int row_size) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            negative(pixels + row_size / sizeof(RGBTRIPLE) * i + j);
        }
    }
}
void apply_black_and_white(RGBTRIPLE* pixels, int width, int height, int row_size) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            black_and_white(pixels + row_size / sizeof(RGBTRIPLE) * i + j);
        }
    }
}
void save_image(const char* file_name, BITMAPFILEHEADER file_header, BITMAPINFOHEADER info_header, RGBTRIPLE* pixels, int row_size) {
    FILE* output_file = fopen(file_name, "wb");
    fwrite(&file_header, sizeof(BITMAPFILEHEADER), 1, output_file);
    fwrite(&info_header, sizeof(BITMAPINFOHEADER), 1, output_file);
    fseek(output_file, file_header.bfOffBits, SEEK_SET);
    for (int i = 0; i < info_header.biHeight; i++) {
        fwrite(pixels + row_size / sizeof(RGBTRIPLE) * i, row_size, 1, output_file);
    }
    fclose(output_file);
}
void apply_gamma_correction(RGBTRIPLE* pixels, int width, int height, int row_size, float gamma) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            RGBTRIPLE* pixel = pixels + row_size / sizeof(RGBTRIPLE) * i + j;
            pixel->rgbtBlue = 255 * pow((pixel->rgbtBlue / 255.0), gamma);
            pixel->rgbtGreen = 255 * pow((pixel->rgbtGreen / 255.0), gamma);
            pixel->rgbtRed = 255 * pow((pixel->rgbtRed / 255.0), gamma);
        }
    }
}
int compare(const void* a, const void* b) {
    return (*(unsigned char*)a - *(unsigned char*)b);
}
void apply_median_filter(RGBTRIPLE* pixels, int width, int height, int row_size, int filter_size) {
    RGBTRIPLE* temp_pixels = malloc(row_size * height);
    memcpy(temp_pixels, pixels, row_size * height);

    int half_filter_size = filter_size / 2;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            unsigned char* window_red = malloc(filter_size * filter_size * sizeof(unsigned char));
            unsigned char* window_green = malloc(filter_size * filter_size * sizeof(unsigned char));
            unsigned char* window_blue = malloc(filter_size * filter_size * sizeof(unsigned char));
            int window_index = 0;
            for (int k = -half_filter_size; k <= half_filter_size; k++) {
                for (int l = -half_filter_size; l <= half_filter_size; l++) {
                    int x = j + l;
                    int y = i + k;
                    if (x >= 0 && x < width && y >= 0 && y < height) {
                        RGBTRIPLE* pixel = temp_pixels + row_size / sizeof(RGBTRIPLE) * y + x;
                        window_red[window_index] = pixel->rgbtRed;
                        window_green[window_index] = pixel->rgbtGreen;
                        window_blue[window_index] = pixel->rgbtBlue;
                        window_index++;
                    }
                }
            }
            qsort(window_red, window_index, sizeof(unsigned char), compare);
            qsort(window_green, window_index, sizeof(unsigned char), compare);
            qsort(window_blue, window_index, sizeof(unsigned char), compare);
            RGBTRIPLE* pixel = pixels + row_size / sizeof(RGBTRIPLE) * i + j;
            pixel->rgbtRed = window_red[window_index / 2];
            pixel->rgbtGreen = window_green[window_index / 2];
            pixel->rgbtBlue = window_blue[window_index / 2];
            free(window_red);
            free(window_green);
            free(window_blue);
        }
    }
    free(temp_pixels);
}