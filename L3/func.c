#define _CRT_SECURE_NO_WARNINGS
#include "func.h"


void negative(RGBTRIPLE* pixel) {
    pixel->rgbtBlue = ~pixel->rgbtBlue;
    pixel->rgbtGreen = ~pixel->rgbtGreen;
    pixel->rgbtRed = ~pixel->rgbtRed;
}
void black_and_white(RGBTRIPLE* pixel) {
    float gray = round((pixel->rgbtBlue + pixel->rgbtGreen + pixel->rgbtRed) / 3.0f);
    pixel->rgbtBlue = (unsigned char)gray;
    pixel->rgbtGreen = (unsigned char)gray;
    pixel->rgbtRed = (unsigned char)gray;
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
void save_image(const char* file_name, BITMAPFILEHEADER file_header, BITMAPINFOHEADER info_header, const RGBTRIPLE* pixels, int row_size) {
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
            pixel->rgbtBlue = (unsigned char)(round(255 * pow((pixel->rgbtBlue / 255.0), gamma)));
            pixel->rgbtGreen = (unsigned char)(round(255 * pow((pixel->rgbtGreen / 255.0), gamma)));
            pixel->rgbtRed = (unsigned char)(round(255 * pow((pixel->rgbtRed / 255.0), gamma)));
        }
    }
}
int compare(const void* a, const void* b) {
    return (*(const unsigned char*)a - *(const unsigned char*)b);
}
void apply_median_filter(RGBTRIPLE* pixels, int width, int height, int row_size, int filter_size) {
    MedianFilterData data;
    data.temp_pixels = malloc(row_size * height);
    memcpy(data.temp_pixels, pixels, row_size * height);
    data.pixels = pixels;
    data.width = width;
    data.height = height;
    data.row_size = row_size;
    data.filter_size = filter_size;
    data.half_filter_size = filter_size / 2;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            apply_median_filter_to_pixel(&data, i, j);
        }
    }
    free(data.temp_pixels);
}
void apply_median_filter_to_pixel(MedianFilterData* data, int i, int j) {
    unsigned char* window_red = malloc(data->filter_size * data->filter_size * sizeof(unsigned char));
    unsigned char* window_green = malloc(data->filter_size * data->filter_size * sizeof(unsigned char));
    unsigned char* window_blue = malloc(data->filter_size * data->filter_size * sizeof(unsigned char));
    int window_index = 0;
    for (int k = -data->half_filter_size; k <= data->half_filter_size; k++) {
        for (int l = -data->half_filter_size; l <= data->half_filter_size; l++) {
            add_pixel_to_window(data, i, j, k, l, &window_index, &window_red, &window_green, &window_blue);
        }
    }
    qsort(window_red, window_index, sizeof(unsigned char), compare);
    qsort(window_green, window_index, sizeof(unsigned char), compare);
    qsort(window_blue, window_index, sizeof(unsigned char), compare);
    RGBTRIPLE* pixel = data->pixels + data->row_size / sizeof(RGBTRIPLE) * i + j;
    pixel->rgbtRed = window_red[window_index / 2];
    pixel->rgbtGreen = window_green[window_index / 2];
    pixel->rgbtBlue = window_blue[window_index / 2];
    free(window_red);
    free(window_green);
    free(window_blue);
}
void add_pixel_to_window(MedianFilterWindowData* data, int i, int j, int k, int l) {
    int x = j + l;
    int y = i + k;
    if (x >= 0 && x < data->width && y >= 0 && y < data->height) {
        RGBTRIPLE* pixel = data->temp_pixels + data->row_size / sizeof(RGBTRIPLE) * y + x;
        (*data->window_red)[*data->window_index] = pixel->rgbtRed;
        (*data->window_green)[*data->window_index] = pixel->rgbtGreen;
        (*data->window_blue)[*data->window_index] = pixel->rgbtBlue;
        (*data->window_index)++;
    }
}
int read_image(const char* input_file_name, BITMAPFILEHEADER* file_header, BITMAPINFOHEADER* info_header, RGBTRIPLE** pixels, int* row_size) {
    FILE* input_file = fopen(input_file_name, "rb");
    if (input_file == NULL) {
        printf("\033[0;33m Error: could not open file %s\n \033[0m", input_file_name);
        return -1;
    }
    fread(file_header, sizeof(BITMAPFILEHEADER), 1, input_file);
    if (file_header->bfType != 0x4D42) {
        printf("\033[0;33m Error: file %s is not a BMP file\n \033[0m", input_file_name);
        fclose(input_file);
        return -1;
    }
    fread(info_header, sizeof(BITMAPINFOHEADER), 1, input_file);
    if (info_header->biBitCount != 24) {
        printf("\033[0;33m Error: only support for processing of 24-bit images\n \033[0m");
        fclose(input_file);
        return -1;
    }
    *row_size = (info_header->biWidth * 3 + 3) & ~3;
    *pixels = malloc(*row_size * info_header->biHeight);
    fseek(input_file, file_header->bfOffBits, SEEK_SET);
    for (int i = 0; i < info_header->biHeight; i++) {
        fread(*pixels + *row_size / sizeof(RGBTRIPLE) * i, *row_size, 1, input_file);
    }
    fclose(input_file);
    return 0;
}

int present_menu() {
    int choice;
    printf("\033[0;37m Select an operation:\n");
    printf("1. Negative\n");
    printf("2. Black and White\n");
    printf("3. Apply multiple effects\n");
    printf("4. Gamma Correction\n");
    printf("5. Median Filter\n");
    printf("6. Exit\n \033[0m");
    printf("\033[0;32m Enter your choice: \033[0m");
    scanf("%d", &choice);
    return choice;
}

void process_choice(int choice, BITMAPFILEHEADER file_header, BITMAPINFOHEADER info_header, RGBTRIPLE* pixels, RGBTRIPLE* original_pixels, int row_size) {
    char output_file_name[256];
    printf("\033[0;32m Enter the name of the output file: \033[0m");
    scanf("%s", output_file_name);
    switch (choice) {
    case 1:
        apply_negative(pixels, info_header.biWidth, info_header.biHeight, row_size);
        save_image(output_file_name, file_header, info_header, pixels, row_size);
        memcpy(pixels, original_pixels, row_size * info_header.biHeight);
        break;
    case 2:
        apply_black_and_white(pixels, info_header.biWidth, info_header.biHeight, row_size);
        save_image(output_file_name, file_header, info_header, pixels, row_size);
        memcpy(pixels, original_pixels, row_size * info_header.biHeight);
        break;
    case 3:
        printf("\033[0;32m Enter the value for gamma correction: \033[0m");
        float gamma;
        scanf("%f", &gamma);
        while (gamma <= 0) {
            printf("\033[0;33m Error: gamma value must be greater than 0\n \033[0m");
            printf("\033[0;32m Enter the value for gamma correction: \033[0m");
            scanf("%f", &gamma);
        }
        apply_gamma_correction(pixels, info_header.biWidth, info_header.biHeight, row_size, gamma);
        break;
    case 4:
        printf("\033[0;32m Enter the size of the median filter: \033[0m");
        int filter_size;
        scanf("%d", &filter_size);
        while (filter_size < 3 || filter_size > 19) {
            printf("\033[0;33m Error: filter size must be between 3 and 19\n \033[0m");
            printf("\033[0;32m Enter the size of the median filter: \033[0m");
            scanf("%d", &filter_size);
        }
        apply_median_filter(pixels, info_header.biWidth, info_header.biHeight, row_size, filter_size);
        break;
    }            
}

