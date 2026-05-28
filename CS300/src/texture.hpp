/**
 * @file texture.hpp
 * @author Dante Harper
 * @date 24 May 26
 */

#pragma once

#include <GL/glew.h>

namespace cs300 {

class Texture {
public:
	Texture();
	~Texture();

	void bind(unsigned int unit = 0) const;

private:
	struct {
		GLuint id = 0;
	} m;
};

}
