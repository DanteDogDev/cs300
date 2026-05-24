/**
 * @file texture.hpp
 * @author Dante Harper
 * @date 24 May 26
 */

#pragma once

#include <GL/glew.h>

class Texture {
public:
	Texture();
	~Texture();

	void Bind(unsigned int unit = 0) const;

private:
	struct {
		GLuint id = 0;
	} m;
};

