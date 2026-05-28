/**
 * @file texture.hpp
 * @author Dante Harper
 * @date 24 May 26
 */

#pragma once

#include "open_gl.hpp"
namespace cs300 {

class Texture {
public:
	Texture();

	void bind(unsigned int slot = 0) const;

private:
	gl::Texture texture;
};

}
