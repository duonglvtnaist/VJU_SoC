@echo off
REM filepath: g:\My Drive\VJU Giao Trinh\CE433 - Thiết kế Hệ thống SoC\DeAnCuoiKy\Code_C\compile_and_run.bat

REM Chuyển mã hóa CMD sang UTF-8
chcp 65001 > nul

REM Đặt đường dẫn tới file mã nguồn
set SOURCE_FILE=median_filter.c
set OUTPUT_FILE=median_filter.exe

REM Biên dịch file C
gcc %SOURCE_FILE% -o %OUTPUT_FILE%

REM Kiểm tra xem quá trình biên dịch có thành công không
if %errorlevel% neq 0 (
    echo Biên dịch thất bại!
    pause
    exit /b %errorlevel%
)

REM Thực thi chương trình
echo Đang chạy chương trình...
%OUTPUT_FILE%

pause