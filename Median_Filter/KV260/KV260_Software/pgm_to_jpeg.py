from PIL import Image
import sys
import os

def pgm_to_jpeg(input_file, output_file):
    try:
        # Mở file PGM
        with Image.open(input_file) as img:
            # Chuyển đổi và lưu dưới dạng JPEG
            img.convert("L").save(output_file, "JPEG")
        print(f"Chuyển đổi thành công từ {input_file} sang {output_file}")
    except Exception as e:
        print(f"Lỗi: {e}")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Cách dùng: python script.py <input_file.pgm> <output_file.jpg>")
        sys.exit(1)

    input_file = sys.argv[1]
    output_file = sys.argv[2]

    # Kiểm tra phần mở rộng file (tùy chọn)
    if not input_file.lower().endswith(".pgm"):
        print("File đầu vào phải có đuôi .pgm")
        sys.exit(1)
    if not output_file.lower().endswith(".jpg") and not output_file.lower().endswith(".jpeg"):
        print("File đầu ra phải có đuôi .jpg hoặc .jpeg")
        sys.exit(1)

    pgm_to_jpeg(input_file, output_file)
