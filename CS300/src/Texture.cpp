#include "Texture.hpp"

Texture::Texture() {
	const unsigned char data[] = {0,   0,   255, 0,   255, 255, 0,   255, 0,   255, 255, 0,   255, 0,   0,   255, 0,   255,
	                              0,   255, 255, 0,   255, 0,   255, 255, 0,   255, 0,   0,   255, 0,   255, 0,   0,   255,
	                              0,   255, 0,   255, 255, 0,   255, 0,   0,   255, 0,   255, 0,   0,   255, 0,   255, 255,
	                              255, 255, 0,   255, 0,   0,   255, 0,   255, 0,   0,   255, 0,   255, 255, 0,   255, 0,
	                              255, 0,   0,   255, 0,   255, 0,   0,   255, 0,   255, 255, 0,   255, 0,   255, 255, 0,
	                              255, 0,   255, 0,   0,   255, 0,   255, 255, 0,   255, 0,   255, 255, 0,   255, 0,   0};

	glGenTextures(1, &m.id);
	glBindTexture(GL_TEXTURE_2D, m.id);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 6, 6, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::~Texture() {
	if (m.id != 0) {
		glDeleteTextures(1, &m.id);
	}
}

Texture::Texture(Texture&& other) noexcept {
	m.id = other.m.id;
	other.m.id = 0;
}

auto Texture::operator=(Texture&& other) noexcept -> Texture& {
	if (this != &other) {
		if (m.id != 0) {
			glDeleteTextures(1, &m.id);
		}
		m.id = other.m.id;
		other.m.id = 0;
	}
	return *this;
}

void Texture::bind(unsigned int unit) const {
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, m.id);
}
