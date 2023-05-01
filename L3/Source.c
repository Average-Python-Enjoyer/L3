#define _CRT_SECURE_NO_WARNINGS
#include "func.h"


int main() {
    char input_file_name[256];
    printf("\033[0;32m Enter the name of the file to process: \033[0m");
    fgets(input_file_name, sizeof(input_file_name), stdin);
    input_file_name[strcspn(input_file_name, "\n")] = '\0';
    FILE* input_file = fopen(input_file_name, "rb");
    if (input_file == NULL) {
        printf("\033[0;33m Error: could not open file %s\n \033[0m", input_file_name);
        return -1;
    }
    BITMAPFILEHEADER file_header;
    fread(&file_header, sizeof(BITMAPFILEHEADER), 1, input_file);
    if (file_header.bfType != 0x4D42) {
        printf("\033[0;33m Error: file %s is not a BMP file\n \033[0m", input_file_name);
        fclose(input_file);
        return -1;
    }
    BITMAPINFOHEADER info_header;
    fread(&info_header, sizeof(BITMAPINFOHEADER), 1, input_file);
    if (info_header.biBitCount != 24) {
        printf("\033[0;33m Error: only support for processing of 24-bit images\n \033[0m");
        fclose(input_file);
        return -1;
    }
    int row_size = (info_header.biWidth * 3 + 3) & ~3;
    RGBTRIPLE* pixels = malloc(row_size * info_header.biHeight);
    RGBTRIPLE* original_pixels = malloc(row_size * info_header.biHeight);
    fseek(input_file, file_header.bfOffBits, SEEK_SET);
    for (int i = 0; i < info_header.biHeight; i++) {
        fread(pixels + row_size / sizeof(RGBTRIPLE) * i, row_size, 1, input_file);
        memcpy(original_pixels + row_size / sizeof(RGBTRIPLE) * i, pixels + row_size / sizeof(RGBTRIPLE) * i, row_size);
    }
    fclose(input_file);

    int choice;
    while (1) {
        printf("\033[0;37m Select an operation:\n");
        printf("1. Negative\n");
        printf("2. Black and White\n");
        printf("3. Apply multiple effects\n");
        printf("4. Gamma Correction\n");
        printf("5. Median Filter\n");
        printf("6. Exit\n \033[0m");
        printf("\033[0;32m Enter your choice: \033[0m");
        scanf("%d", &choice);
        if (choice == 6) {
            break;
        }
        char output_file_name[256];
        printf("\033[0;32m Enter the name of the output file: \033[0m");
        scanf("%s", output_file_name);
        //output_file_name[strcspn(output_file_name, "\n")] = '\0';
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
            printf("\033[0;32m Enter the number of effects to apply: \033[0m");
            int num_effects;
            scanf("%d", &num_effects);
            for (int i = 0; i < num_effects; i++) {
                printf("\033[0;37m Select an effect to apply:\n");
                printf("1. Negative\n");
                printf("2. Black and White\n");
                printf("3. Gamma Correction\n");
                printf("4. Median Filter\n \033[0m");
                printf("\033[0;32m Enter your choice: \033[0m");
                int effect_choice;
                scanf("%d", &effect_choice);
                switch (effect_choice) {
                case 1:
                    apply_negative(pixels, info_header.biWidth, info_header.biHeight, row_size);
                    break;
                case 2:
                    apply_black_and_white(pixels, info_header.biWidth, info_header.biHeight, row_size);
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
                default:
                    printf("\033[0;33m Invalid choice\n \033[0m");
                    break;
                }
            }
            save_image(output_file_name, file_header, info_header, pixels, row_size);
            memcpy(pixels, original_pixels, row_size * info_header.biHeight);
            break;
        case 4:
            printf("\033[0;32m Enter the value for gamma correction: \033[0m");
            float gamma;
            scanf("%f", &gamma);
            while (gamma <= 0) {
                printf("\033[0;33m Error: gamma value must be greater than 0\n \033[0m");
                printf("\033[0;32m Enter the value for gamma correction: \033[0m");
                scanf("%f", &gamma);
            }
            apply_gamma_correction(pixels, info_header.biWidth, info_header.biHeight, row_size, gamma);
            save_image(output_file_name, file_header, info_header, pixels, row_size);
            memcpy(pixels, original_pixels, row_size * info_header.biHeight);
            break;
        case 5:
            printf("\033[0;32m Enter the size of the median filter: \033[0m");
            int filter_size;
            scanf("%d", &filter_size);
            while (filter_size < 3 || filter_size > 19) {
                printf("\033[0;33m Error: filter size must be between 3 and 19\n \033[0m");
                printf("\033[0;32m Enter the size of the median filter: \033[0m");
                scanf("%d", &filter_size);
            }
            apply_median_filter(pixels, info_header.biWidth, info_header.biHeight, row_size, filter_size);
            save_image(output_file_name, file_header, info_header, pixels, row_size);
            memcpy(pixels, original_pixels, row_size * info_header.biHeight);
            break;
        }

    }
    free(pixels);
    free(original_pixels);
    return 0;
}