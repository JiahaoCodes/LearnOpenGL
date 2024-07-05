# OpenGL
## 编译要求
　　编译器：MSVC，VS2022_x64  
　　CMake的Generator：Ninja  
　　C++语法标准：>= C++17
## 目录结构
### assets
　　存放资源文件，包括纹理、模型、字体等。
### bin
　　存放最终生成的可执行文件，相关的资源文件也会被拷贝到此目录中，比如：font，texutre，model，shaders等。
### build
　　CMake的Build目录。
### cmake
　　存放一些cmake的脚本：包括下载依赖库的函数定义和创建可执行文件的宏。
### libraries
　　存放使用的第三方库，使用vcpkg以x64-windows版本编译安装并导出使用。目前使用到的第三方库有：`assimp`,`glad`,`glfw3`,`glm`,`imgui`,`spdlog`,`stb`
### source
#### include
　　包含一些公用的类的定义，比如shader、camera等
#### 子目录
　　子目录中包含教程中的每一个章节，每个章节使用一个文件夹，每个章节的目录结构如下：  
```txt
.
|-- chapter
|   `-- create_a_window.cpp
|   `-- hello_window.cpp
|   `-- shaders
|       `-- default.glsl
```
　　其中shader目录用于保存当前章节使用到的着色器代码，这些着色器代码会在编译前输出到bin/shaders目录中，每个章节目录中的每一个.cpp都会被编译成一个独立的可执行文件。
### tools
　　存放一些工具，比如m4宏处理器等。
### CmakeLists.txt
　　CMake配置文件，用于编译生成可执行程序。
## 参考资料