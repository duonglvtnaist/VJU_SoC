from PIL import Image
import sys

def convert_jpeg_to_pgm(input_path, output_path):
    """
    Hàm chuyển đổi file JPEG sang PGM.
    """
    try:
        # Mở file JPEG
        with Image.open(input_path) as img:
            # Chuyển đổi và lưu dưới dạng PGM
            img.convert("L").save(output_path, "PPM")
        print(f"Đã chuyển đổi thành công: {input_path} -> {output_path}")
    except Exception as error:
        print(f"Lỗi khi chuyển đổi: {error}")

if __name__ == "__main__":
    # Kiểm tra số lượng tham số dòng lệnh
    if len(sys.argv) != 3:
        print("Cách sử dụng: python jpeg_to_pgm.py <input_file.jpg> <output_file.pgm>")
        sys.exit(1)

    input_file = sys.argv[1]
    output_file = sys.argv[2]

    # Kiểm tra phần mở rộng file
    if not input_file.lower().endswith(".jpg") and not input_file.lower().endswith(".jpeg"):
        print("Lỗi: File đầu vào phải có đuôi .jpg hoặc .jpeg")
        sys.exit(1)

    if not output_file.lower().endswith(".pgm"):
        print("Lỗi: File đầu ra phải có đuôi .pgm")
        sys.exit(1)

    # Gọi hàm chuyển đổi
    convert_jpeg_to_pgm(input_file, output_file)