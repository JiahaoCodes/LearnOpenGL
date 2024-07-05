#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H
 
#include <glad/glad.h>
#include <glm/glm.hpp>
 
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>
 
class ShaderProgram{
public:
    // 用于构建program的shader类型
    enum class ShaderType{
        VERTEX = GL_VERTEX_SHADER,
        TESSCONTROL = GL_TESS_CONTROL_SHADER,
        TESSEVALUATION = GL_TESS_EVALUATION_SHADER,
        GEOMETRY = GL_GEOMETRY_SHADER,
        FRAGMENT = GL_FRAGMENT_SHADER,
    };
 
    using ShaderSourcePair = std::pair<ShaderType, std::string>;
 
    ShaderProgram(std::initializer_list<ShaderSourcePair> shaderSources){
        ID = glCreateProgram();
        for(const auto& source : shaderSources){
            compileAndAttachShader(source);
        }
        linkProgram();
    }
 
    ~ShaderProgram()
    {
        glDeleteProgram(ID);
    }
    // activate the shader
    // ------------------------------------------------------------------------
    void use() const
    { 
        glUseProgram(ID); 
    }
    // utility uniform functions
    // ------------------------------------------------------------------------
    void setUniform(const std::string &name, bool value) const
    {
        setUniform(name, static_cast<int>(value));   
    }
    // ------------------------------------------------------------------------
    void setUniform(const std::string &name, int value) const
    { 
        glUniform1i(getUniformLocation(name), value); 
    }
    // ------------------------------------------------------------------------
    void setUniform(const std::string &name, float value) const
    { 
        glUniform1f(getUniformLocation(name), value); 
    }
    // ------------------------------------------------------------------------
    void setUniform(const std::string &name, const glm::vec2 &value) const
    { 
        glUniform2fv(getUniformLocation(name), 1, &value[0]); 
    }
    void setUniform(const std::string &name, float x, float y) const
    { 
        glUniform2f(getUniformLocation(name), x, y); 
    }
    // ------------------------------------------------------------------------
    void setUniform(const std::string &name, const glm::vec3 &value) const
    { 
        glUniform3fv(getUniformLocation(name), 1, &value[0]); 
    }
    void setUniform(const std::string &name, float x, float y, float z) const
    { 
        glUniform3f(getUniformLocation(name), x, y, z); 
    }
    // ------------------------------------------------------------------------
    void setUniform(const std::string &name, const glm::vec4 &value) const
    { 
        glUniform4fv(getUniformLocation(name), 1, &value[0]); 
    }
    void setUniform(const std::string &name, float x, float y, float z, float w) const
    { 
        glUniform4f(getUniformLocation(name), x, y, z, w); 
    }
    // ------------------------------------------------------------------------
    void setUniform(const std::string &name, const glm::mat2 &mat) const
    {
        glUniformMatrix2fv(getUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setUniform(const std::string &name, const glm::mat3 &mat) const
    {
        glUniformMatrix3fv(getUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setUniform(const std::string &name, const glm::mat4 &mat) const
    {
        glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
    }
 
private:

    GLint getUniformLocation(const std::string& name) const{
        GLint location = glGetUniformLocation(ID, name.c_str());
        if(location == -1){
            std::cout << "Warning: uniform " << name << " does not exist in program " << ID << std::endl;
        }
        return location;
    }
 
    void compileAndAttachShader(const ShaderSourcePair& shaderSourcePair){
        std::string shaderCode;
        try{
            shaderCode = readFile(shaderSourcePair.second);
        }catch(const std::runtime_error& e){
            std::cout << e.what() << std::endl;
            return;
        }
        const char* shaderCodeCStr = shaderCode.c_str();
        GLuint shader = glCreateShader(static_cast<GLenum>(shaderSourcePair.first));
        glShaderSource(shader, 1, &shaderCodeCStr, NULL);
        glCompileShader(shader);
        checkShaderCompileErrors(shader, shaderSourcePair.first);
        glAttachShader(ID, shader);
        glDeleteShader(shader);
    }

    std::string readFile(const std::string& filename){
        std::ifstream shaderFile(filename);
        if(!shaderFile){
            throw std::runtime_error("ShaderProgram: Failed to open file: " + filename);
        }
        std::stringstream shaderStream;
        shaderStream << shaderFile.rdbuf();
        return shaderStream.str();
    }
 
    void linkProgram() {
        glLinkProgram(ID);
        checkProgramLinkErrors();
    }
 
    void checkShaderCompileErrors(GLuint shader, ShaderType type)
    {
        GLint success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            constexpr int MAX_LOG_SIZE = 512;
            std::vector<GLchar> infoLog(MAX_LOG_SIZE);
            glGetShaderInfoLog(shader, static_cast<GLsizei>(infoLog.size()), nullptr, infoLog.data());
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << shaderTypeMap[type] << "\n" << infoLog.data()
                      << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
 
    void checkProgramLinkErrors(void){
        GLint success;
        glGetProgramiv(ID, GL_LINK_STATUS, &success);
        if (!success)
        {
            constexpr int MAX_LOG_SIZE = 512;
            std::vector<GLchar> infoLog(MAX_LOG_SIZE);
            glGetProgramInfoLog(ID, static_cast<GLsizei>(infoLog.size()), nullptr, infoLog.data());
            std::cout << "ERROR::PROGRAM_LINKING_ERROR: \n" << infoLog.data() 
                      << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
 
private:
    unsigned int ID;
 
    static std::unordered_map<ShaderType, const char*> shaderTypeMap;
};
 
std::unordered_map<ShaderProgram::ShaderType, const char*> ShaderProgram::shaderTypeMap = {
    {ShaderProgram::ShaderType::VERTEX, "VERTEX"},
    {ShaderProgram::ShaderType::TESSCONTROL, "TESS_CONTROL"},
    {ShaderProgram::ShaderType::TESSEVALUATION, "TESS_EVALUATION"},
    {ShaderProgram::ShaderType::GEOMETRY, "GEOMETRY"},
    {ShaderProgram::ShaderType::FRAGMENT, "FRAGMENT"}
};
 
#endif
