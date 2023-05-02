#define _CRT_SECURE_NO_WARNINGS
#include "func.h"


int main() {
    char input_file_name[256];
    printf("\033[0;32m Enter the name of the file to process: \033[0m");
    fgets(input_file_name, sizeof(input_file_name), stdin);
    input_file_name[strcspn(input_file_name, "\n")] = '\0';
    BITMAPFILEHEADER file_header;
    BITMAPINFOHEADER info_header;
    RGBTRIPLE* pixels;
    int row_size;
    if (read_image(input_file_name, &file_header, &info_header, &pixels, &row_size) == -1) {
        return -1;
    }
    RGBTRIPLE* original_pixels = malloc(row_size * info_header.biHeight);
    memcpy(original_pixels, pixels, row_size * info_header.biHeight);
    int choice;
    while ((choice = present_menu()) != 6) {
        process_choice(choice, file_header, info_header, pixels, original_pixels, row_size);
    }
    free(pixels);
    free(original_pixels);
    return 0;
}