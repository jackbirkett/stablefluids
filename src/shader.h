#pragma once

#include <iostream>

class Shader {

public:

	unsigned int id;

	Shader(const std::string& vertexpath, const std::string& fragmentpath);
	~Shader();
	void bind() const;
	void unbind() const;
	void setUniform1i(const std::string& name, int value) const;

private:

	void checkCompileErrors(unsigned int shader, std::string type) const;
	
};