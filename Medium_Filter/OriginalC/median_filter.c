#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>  // Để dùng uint8_t
// #include <windows.h>

#define FILTER_SIZE 3

// Hoán đổi giá trị
void swap(uint8_t *a, uint8_t *b) {
    uint8_t temp = *a;
    *a = *b;
    *b = temp;
}

// Sắp xếp mảng bằng bubble sort
void bubble_sort(uint8_t arr[], int n) {
    for (int i = 0; i < n - 1; ++i)
        for (int j = 0; j < n - i - 1; ++j){
            if (arr[j] > arr[j + 1])
                swap(&arr[j], &arr[j + 1]);
            // printf("i = %d, j = %d, \t %d, %d, %d, %d, %d, %d, %d, %d\n", i, j, arr[0], arr[1], arr[2], arr[3], arr[4], arr[5], arr[6], arr[7], arr[8]);
        }
}

// Lọc median
void median_filter(uint8_t **input, uint8_t **output, int height, int width) {
    int indexer = FILTER_SIZE / 2;
    uint8_t window[FILTER_SIZE * FILTER_SIZE];

    printf("FILTER_SIZE = %d\n", FILTER_SIZE);
    printf("height = %d\n", height);
    printf("width = %d\n", width);
    printf("indexer = %d\n", indexer);
    printf("FILTER_SIZE * FILTER_SIZE = %d\n", FILTER_SIZE * FILTER_SIZE);
    printf("FILTER_SIZE * FILTER_SIZE / 2 = %d\n", FILTER_SIZE * FILTER_SIZE / 2);
    // indexer = 1; // Chỉ dùng cho ảnh 3x3
    printf("input[1][0] = %02x\n", input[1][0]);
    printf("input[1][1] = %02x\n", input[1][1]);

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            int count = 0;
            for (int fi = -indexer; fi <= indexer; ++fi) {  // fi from -1 to 1
                for (int fj = -indexer; fj <= indexer; ++fj) { // fj from -1 to 1
                    int ni = i + fi;    // ni = i + fi
                    int nj = j + fj;    // nj = j + fj

                    // if(i == 0 && j == 0) {
                    //     // printf("input[%d][%d] = %02x\n", ni, nj, input[ni][nj]);
                    //     printf("ni = %d, nj = %d\n", ni*width, nj);
                    // }
                    if (ni >= 0 && ni < height && nj >= 0 && nj < width) {
                        window[count++] = input[ni][nj];
                    } else {
                        window[count++] = 0; // padding bằng 0
                    }
                }
            }
            if(i == 0  && j < 20) {
                int k = 0;
                printf("window at [%d][%d]: ", i, j);
                for (k = 0; k < FILTER_SIZE * FILTER_SIZE; ++k) {
                    printf("%02x ", window[k]);
                }
                printf("\n");
            }
            bubble_sort(window, FILTER_SIZE * FILTER_SIZE);

            output[i][j] = window[FILTER_SIZE * FILTER_SIZE / 2];
            if(i == 553 && j > 420) {
                printf("output[%d][%d] = %02x\n", i, j, output[i][j]);
            }
        }
    }
}

// Cấp phát bộ nhớ động cho ảnh 2D
uint8_t **allocate_image(int height, int width) {
    uint8_t **img = (uint8_t **)malloc(height * sizeof(uint8_t *));
    for (int i = 0; i < height; ++i)
        img[i] = (uint8_t *)malloc(width);
    return img;
}

// Giải phóng bộ nhớ ảnh
void free_image(uint8_t **img, int height) {
    for (int i = 0; i < height; ++i)
        free(img[i]);
    free(img);
}

// Đọc ảnh PGM định dạng P2 hoặc P5
uint8_t **read_pgm(const char *filename, int *height, int *width) {
    FILE *f = fopen(filename, "rb");
    if (!f) {
        printf("Không thể mở file %s\n", filename);
        exit(1);
    }

    char format[3];
    fscanf(f, "%2s", format);
    if (strcmp(format, "P5") != 0 && strcmp(format, "P2") != 0) {
        printf("File không phải định dạng P2 hoặc P5\n");
        fclose(f);
        exit(1);
    }

    // Bỏ qua comment nếu có
    int c = fgetc(f);
    while (c == '#') {
        while (fgetc(f) != '\n');
        c = fgetc(f);
    }
    ungetc(c, f);

    int maxval;
    fscanf(f, "%d %d\n%d\n", width, height, &maxval);

    uint8_t **img = allocate_image(*height, *width);

    if (strcmp(format, "P5") == 0) {
        // Đọc dữ liệu nhị phân
        for (int i = 0; i < *height; ++i)
            fread(img[i], sizeof(uint8_t), *width, f);
    } else {
        // Đọc dữ liệu văn bản (P2)
        for (int i = 0; i < *height; ++i)
            for (int j = 0; j < *width; ++j) {
                int pixel;
                fscanf(f, "%d", &pixel);
                img[i][j] = (uint8_t)pixel;
            }
    }

    fclose(f);
    return img;
}

// Ghi ảnh PGM ở định dạng P5
void write_pgm(const char *filename, uint8_t **img, int height, int width) {
    FILE *f = fopen(filename, "wb");
    if (!f) {
        printf("Không thể ghi file %s\n", filename);
        exit(1);
    }

    fprintf(f, "P5\n%d %d\n255\n", width, height);
    for (int i = 0; i < height; ++i)
        fwrite(img[i], sizeof(uint8_t), width, f);

    fclose(f);
}

void write_uint8_matrix_to_hex_txt(const char* filename, uint8_t** data, int width, int height) {
    FILE* fp = fopen(filename, "w");
    if (fp == NULL) {
        perror("Error opening file");
        return;
    }

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            fprintf(fp, "%02X\n", data[i][j]);
        }
    }

    fclose(fp);
}

int main() {
    // SetConsoleOutputCP(65001); // Đặt mã hóa UTF-8 cho CMD
    int height, width;
    uint8_t **input = read_pgm("noisyimg.pgm", &height, &width);
    uint8_t **output = allocate_image(height, width);

    write_uint8_matrix_to_hex_txt("noisyimg.txt", input, width, height);

    printf("Đọc ảnh thành công: %d x %d\n", height, width);

    median_filter(input, output, height, width);
    write_pgm("removed_noise.pgm", output, height, width);

    write_uint8_matrix_to_hex_txt("removed_noise.txt", output, width, height);

    free_image(input, height);
    free_image(output, height);

    printf("Hoàn thành lọc median và lưu ảnh ra removed_noise.pgm\n");

    // ------------------------------------------------------------------
    // uint8_t test_array[9] = {9, 3, 7, 1, 4, 6, 8, 2, 5};
    // int n = 9;

    // printf("Mảng trước khi sắp xếp:\n");
    // for (int i = 0; i < n; ++i) {
    //     printf("%d ", test_array[i]);
    // }
    // printf("\n");

    // // Gọi hàm bubble_sort
    // bubble_sort(test_array, n);

    // printf("Mảng sau khi sắp xếp:\n");
    // for (int i = 0; i < n; ++i) {
    //     printf("%d ", test_array[i]);
    // }
    // printf("\n");
    return 0;
}
