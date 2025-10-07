@echo off
echo Intel CPU 硬件随机数生成器 - 编译和运行脚本
echo ================================================

echo.
echo 检查编译器...
where g++ >nul 2>&1
if %errorlevel% neq 0 (
    echo 错误: 未找到g++编译器
    echo 请安装MinGW-w64或其他支持的C++编译器
    pause
    exit /b 1
)

echo 找到g++编译器，开始编译...
echo.

g++ -std=c++11 -O2 -Wall -Wextra -mrdrnd -mrdseed -o random_generator.exe random_generator_en-US.cpp

if %errorlevel% neq 0 (
    echo 编译失败！
    echo 可能的原因：
    echo 1. 编译器不支持 -mrdrnd 或 -mrdseed 选项
    echo 2. 源代码有语法错误
    echo 3. 缺少必要的头文件
    pause
    exit /b 1
)

echo 编译成功！
echo.

echo 运行程序...
echo ================================================
random_generator.exe

echo.
echo 程序运行完毕。
pause