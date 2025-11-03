#include <glad/glad.h>
#include "shader.h"
#include <string>
#include <fstream>
#include <sstream>

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath)
{
    std::string vertexSource, fragmentSource;
    std::ifstream vertexFile, fragmentFile;
    std::stringstream vertexStream, fragmentStream;

    vertexFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fragmentFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        vertexFile.open(vertexPath);
        fragmentFile.open(fragmentPath);
        
        vertexStream << vertexFile.rdbuf();
        fragmentStream << fragmentFile.rdbuf();
        
        vertexFile.close();
        fragmentFile.close();
        
        vertexSource = vertexStream.str();
        fragmentSource = fragmentStream.str();
    }
    catch (std::ifstream::failure& e)
    {
        std::cout << "Error: shader file failed to be read: " << e.what() << std::endl;
    }

    const char *cVertexSource = vertexSource.c_str();
    const char *cFragmentSource = fragmentSource.c_str();
    
    unsigned int vertex, fragment;
    
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &cVertexSource, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "vertex");
    
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &cFragmentSource, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "fragment");
    
    id = glCreateProgram();
    glAttachShader(id, vertex);
    glAttachShader(id, fragment);
    glLinkProgram(id);
    checkCompileErrors(id, "program");
    
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

Shader::~Shader()
{
    glDeleteProgram(id);
}
    
void Shader::bind() const
{
    glUseProgram(id);
}

void Shader::unbind() const
{
    glUseProgram(0);
}

void Shader::setUniform1i(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::checkCompileErrors(unsigned int shader, std::string type) const
{
    int success;
    char infoLog[1024];
    if (type != "program")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "Shader compilation error of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "Program linking error of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}
