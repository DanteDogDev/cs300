#include "texture.hpp"

namespace cs300 {

Texture::Texture() {
	texture.make("./data/textures/default.bmp");
}


void Texture::bind(unsigned int slot) const {
	texture.bind(slot);
}
}
