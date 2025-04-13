from PIL import Image

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
    # Nhập tên file từ người dùng (không bao gồm đuôi)
    input_name = input("Nhập tên file PGM đầu vào (không bao gồm đuôi): ")
    
    # Thêm đuôi file
    input_file = f"{input_name}.pgm"
    output_file = f"{input_name}.jpg"
    
    pgm_to_jpeg(input_file, output_file)