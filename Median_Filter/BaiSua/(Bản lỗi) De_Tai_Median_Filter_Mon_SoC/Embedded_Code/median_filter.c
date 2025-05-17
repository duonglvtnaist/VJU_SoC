#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>  // Để dùng uint8_t
#include <dirent.h>
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

    // printf("FILTER_SIZE = %d\n", FILTER_SIZE);
    // printf("height = %d\n", height);
    // printf("width = %d\n", width);
    // printf("indexer = %d\n", indexer);
    // printf("FILTER_SIZE * FILTER_SIZE = %d\n", FILTER_SIZE * FILTER_SIZE);
    // printf("FILTER_SIZE * FILTER_SIZE / 2 = %d\n", FILTER_SIZE * FILTER_SIZE / 2);
    // // indexer = 1; // Chỉ dùng cho ảnh 3x3
    // printf("input[1][0] = %02x\n", input[1][0]);
    // printf("input[1][1] = %02x\n", input[1][1]);

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
            // if(i == 0  && j < 20) {
            //     int k = 0;
            //     printf("window at [%d][%d]: ", i, j);
            //     for (k = 0; k < FILTER_SIZE * FILTER_SIZE; ++k) {
            //         printf("%02x ", window[k]);
            //     }
            //     printf("\n");
            // }
            bubble_sort(window, FILTER_SIZE * FILTER_SIZE);

            output[i][j] = window[FILTER_SIZE * FILTER_SIZE / 2];
            // if(i == 553 && j > 420) {
            //     printf("output[%d][%d] = %02x\n", i, j, output[i][j]);
            // }
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

void add_salt_and_pepper_noise(uint8_t **img, int height, int width, float noise_ratio) {
    // noise_ratio: tỷ lệ nhiễu (giá trị từ 0 đến 1)
    int total_pixels = height * width;
    int num_noisy_pixels = (int)(total_pixels * noise_ratio);

    srand((unsigned int)time(NULL)); // Khởi tạo seed cho hàm rand

    for (int i = 0; i < num_noisy_pixels; ++i) {
        int x = rand() % height; // Chọn ngẫu nhiên tọa độ x
        int y = rand() % width;  // Chọn ngẫu nhiên tọa độ y

        // Thêm nhiễu muối (255) hoặc tiêu (0) ngẫu nhiên
        img[x][y] = (rand() % 2 == 0) ? 0 : 255;
    }
}



void process_images_with_noise(const char *input_dir, const char *output_dir_pgm, const char *output_dir_jpg, float noise_ratio) {
    DIR *dir;
    struct dirent *entry;

    // Mở thư mục đầu vào
    if ((dir = opendir(input_dir)) == NULL) {
        perror("Không thể mở thư mục đầu vào");
        return;
    }

    // Duyệt qua từng file trong thư mục
    while ((entry = readdir(dir)) != NULL) {
        // Bỏ qua các mục "." và ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        // Kiểm tra phần mở rộng file (chỉ xử lý file .pgm)
        if (strstr(entry->d_name, ".pgm") == NULL)
            continue;

        // Tạo đường dẫn đầy đủ cho file đầu vào
        char input_path[256];
        snprintf(input_path, sizeof(input_path), "%s/%s", input_dir, entry->d_name);

        // Đọc ảnh từ file
        int height, width;
        uint8_t **input = read_pgm(input_path, &height, &width);
        uint8_t **noisy_image = allocate_image(height, width);

        // Sao chép ảnh gốc vào noisy_image
        for (int i = 0; i < height; ++i)
            for (int j = 0; j < width; ++j)
                noisy_image[i][j] = input[i][j];

        // Thêm nhiễu muối tiêu
        add_salt_and_pepper_noise(noisy_image, height, width, noise_ratio);

        // Tạo đường dẫn đầy đủ cho file đầu ra (PGM)
        char output_pgm_path[256];
        snprintf(output_pgm_path, sizeof(output_pgm_path), "%s/%s", output_dir_pgm, entry->d_name);

        // Ghi ảnh có nhiễu ra file PGM
        write_pgm(output_pgm_path, noisy_image, height, width);

        // Tạo đường dẫn đầy đủ cho file đầu ra (JPG)
        char output_jpg_path[256];
        snprintf(output_jpg_path, sizeof(output_jpg_path), "%s/%s", output_dir_jpg, entry->d_name);
        char *dot = strrchr(output_jpg_path, '.');
        if (dot) strcpy(dot, ".jpg"); // Đổi đuôi file thành .jpg

        // Tạo lệnh gọi script Python để chuyển đổi từ PGM sang JPG
        char command[512];
        snprintf(command, sizeof(command), "python pgm_to_jpeg.py \"%s\" \"%s\"", output_pgm_path, output_jpg_path);

        // Thực thi lệnh
        printf("Đang chuyển đổi: %s -> %s\n", output_pgm_path, output_jpg_path);
        int result = system(command);
        if (result != 0) {
            printf("Lỗi khi chuyển đổi file: %s\n", output_pgm_path);
        }

        // Giải phóng bộ nhớ
        free_image(input, height);
        free_image(noisy_image, height);

        printf("Đã xử lý: %s -> %s và %s\n", input_path, output_pgm_path, output_jpg_path);
    }

    closedir(dir);
}

void convert_jpg_to_pgm(const char *input_dir, const char *output_dir) {
    DIR *dir;
    struct dirent *entry;

    // Mở thư mục đầu vào
    if ((dir = opendir(input_dir)) == NULL) {
        perror("Không thể mở thư mục đầu vào");
        return;
    }

    // Duyệt qua từng file trong thư mục
    while ((entry = readdir(dir)) != NULL) {
        // Bỏ qua các mục "." và ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        // Kiểm tra phần mở rộng file (chỉ xử lý file .jpg hoặc .jpeg)
        if (strstr(entry->d_name, ".jpg") == NULL && strstr(entry->d_name, ".jpeg") == NULL)
            continue;

        // Tạo đường dẫn đầy đủ cho file đầu vào
        char input_path[256];
        snprintf(input_path, sizeof(input_path), "%s/%s", input_dir, entry->d_name);

        // Tạo tên file đầu ra với đuôi .pgm
        char output_file[256];
        snprintf(output_file, sizeof(output_file), "%s/%s", output_dir, entry->d_name);
        char *dot = strrchr(output_file, '.');
        if (dot) strcpy(dot, ".pgm"); // Đổi đuôi file thành .pgm

        // Tạo lệnh gọi script Python
        char command[512];
        snprintf(command, sizeof(command), "python jpeg_to_pgm.py \"%s\" \"%s\"", input_path, output_file);
        // Thực thi lệnh
        printf("Đang chuyển đổi: %s -> %s\n", input_path, output_file);
        int result = system(command);
        if (result != 0) {
            printf("Lỗi khi chuyển đổi file: %s\n", input_path);
        }
    }

    closedir(dir);
}

void process_and_convert_images(const char *input_dir, const char *output_dir_pgm, const char *output_dir_jpg) {
    DIR *dir;
    struct dirent *entry;

    // Mở thư mục đầu vào
    if ((dir = opendir(input_dir)) == NULL) {
        perror("Không thể mở thư mục đầu vào");
        return;
    }

    // Duyệt qua từng file trong thư mục
    while ((entry = readdir(dir)) != NULL) {
        // Bỏ qua các mục "." và ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        // Kiểm tra phần mở rộng file (chỉ xử lý file .pgm)
        if (strstr(entry->d_name, ".pgm") == NULL)
            continue;

        // Tạo đường dẫn đầy đủ cho file đầu vào
        char input_path[256];
        snprintf(input_path, sizeof(input_path), "%s/%s", input_dir, entry->d_name);

        // Đọc ảnh từ file
        int height, width;
        uint8_t **input = read_pgm(input_path, &height, &width);

        // Kiểm tra kích thước ảnh
        if ((long long)width * height > (1 << 18)) { // 2^18 = 262144
            printf("Ảnh quá lớn: %s (kích thước: %d x %d = %lld pixel)\n", input_path, width, height, (long long)width * height);
            free_image(input, height);
            continue;
        } else {
            printf("Ảnh đủ kích thước: %s (kích thước: %d x %d = %lld pixel)\n", input_path, width, height, (long long)width * height);
        }

        uint8_t **output = allocate_image(height, width);

        // Áp dụng bộ lọc trung vị
        median_filter(input, output, height, width);

        // Tạo đường dẫn đầy đủ cho file đầu ra (PGM)
        char output_pgm_path[256];
        snprintf(output_pgm_path, sizeof(output_pgm_path), "%s/%s", output_dir_pgm, entry->d_name);

        // Ghi ảnh đã khử nhiễu ra file PGM
        write_pgm(output_pgm_path, output, height, width);

        // Tạo đường dẫn đầy đủ cho file đầu ra (JPG)
        char output_jpg_path[256];
        snprintf(output_jpg_path, sizeof(output_jpg_path), "%s/%s", output_dir_jpg, entry->d_name);
        char *dot = strrchr(output_jpg_path, '.');
        if (dot) strcpy(dot, ".jpg"); // Đổi đuôi file thành .jpg

        // Tạo lệnh gọi script Python để chuyển đổi từ PGM sang JPG
        char command[512];
        snprintf(command, sizeof(command), "python pgm_to_jpeg.py \"%s\" \"%s\"", output_pgm_path, output_jpg_path);
        printf("Lệnh gọi: %s\n", command);

        // Thực thi lệnh
        printf("Đang chuyển đổi: %s -> %s\n", output_pgm_path, output_jpg_path);
        int result = system(command);
        if (result != 0) {
            printf("Lỗi khi chuyển đổi file: %s\n", output_pgm_path);
        }

        // Giải phóng bộ nhớ
        free_image(input, height);
        free_image(output, height);

        printf("Đã xử lý: %s -> %s và %s\n", input_path, output_pgm_path, output_jpg_path);
    }

    closedir(dir);
}

int main() {
    // SetConsoleOutputCP(65001); // Đặt mã hóa UTF-8 cho CMD
    int height, width;

    // Chuyển đổi ảnh JPG sang PGM ------------------------------------------------
    printf("\n================ BƯỚC 1: CHUYỂN ĐỔI JPG SANG PGM ================\n");
    const char *jpg_input_dir = "./ImageTest";       // Thư mục chứa ảnh JPG
    const char *pgm_output_dir = "./PGMImageTest";   // Thư mục lưu ảnh PGM

    // Gọi hàm chuyển đổi JPG sang PGM
    convert_jpg_to_pgm(jpg_input_dir, pgm_output_dir);
    printf("Hoàn thành chuyển đổi tất cả file JPG sang PGM.\n");

    // Tạo nhiễu muối tiêu cho ảnh ------------------------------------------------
    printf("\n================ BƯỚC 2: TẠO NHIỄU MUỐI TIÊU ================\n");
    const char *pgm_noise_input_dir = "./PGMImageTest";  // Thư mục chứa ảnh PGM gốc
    const char *pgm_noise_output_dir = "./NoiseImageTest"; // Thư mục lưu ảnh PGM có nhiễu
    const char *jpg_noise_output_dir = "./NoiseImageJPG";  // Thư mục lưu ảnh JPG có nhiễu

    // Tỷ lệ nhiễu muối tiêu (ví dụ: 5%)
    float noise_ratio = 0.05;

    // Gọi hàm thêm nhiễu muối tiêu và chuyển đổi sang JPG
    process_images_with_noise(pgm_noise_input_dir, pgm_noise_output_dir, jpg_noise_output_dir, noise_ratio);
    printf("Hoàn thành thêm nhiễu muối tiêu và chuyển đổi tất cả file PGM sang JPG.\n");

    // Khử nhiễu và chuyển đổi sang JPG -------------------------------------------
    printf("\n================ BƯỚC 3: KHỬ NHIỄU VÀ CHUYỂN ĐỔI SANG JPG ================\n");
    const char *pgm_denoise_input_dir = "./NoiseImageTest";       // Thư mục chứa ảnh PGM có nhiễu
    const char *pgm_denoise_output_dir = "./RemovedNoiseImageTest"; // Thư mục lưu ảnh PGM đã khử nhiễu
    const char *jpg_denoise_output_dir = "./RemovedNoiseImageJPG"; // Thư mục lưu ảnh JPG đã khử nhiễu

    // Gọi hàm khử nhiễu và chuyển đổi sang JPG
    process_and_convert_images(pgm_denoise_input_dir, pgm_denoise_output_dir, jpg_denoise_output_dir);
    printf("Hoàn thành khử nhiễu và chuyển đổi tất cả file PGM sang JPG.\n");

    printf("\n================ HOÀN THÀNH TOÀN BỘ QUY TRÌNH ================\n");
    return 0;
}