#ifndef TEXTURE_CLASS_H
#define TEXTURE_CLASS_H

#include "Core.h"
#include "Shader.h"

class Texture {
	bool disposed = false;
public:

	static Ref<Texture> defaultTex;

	static std::map <std::string, Ref<Texture>> library;

	GLuint handle;
	GLuint unit;
	Texture(const char* file, GLuint slot);
	~Texture();
	static Ref<Texture> Create();
	static Ref<Texture> Create(std::string name, std::string file, GLuint slot = 0);
	void TexUnit(Ref<Shader>& shader, const char* uniform, GLuint unit);
	void Bind();
	void Unbind();
	void Delete();
};

#endif
