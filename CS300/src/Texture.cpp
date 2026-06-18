#include "Texture.hpp"

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <cs300/stb_image.h>

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

Texture::Texture(const std::string& filepath) {
	glGenTextures(1, &m.id);
	glBindTexture(GL_TEXTURE_2D, m.id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nr_channels;
	unsigned char* data = stbi_load(filepath.c_str(), &width, &height, &nr_channels, 0);
	if (data) {
		GLenum format = GL_RGB;
		if (nr_channels == 1) {
			format = GL_RED;
		} else if (nr_channels == 3) {
			format = GL_RGB;
		} else if (nr_channels == 4) {
			format = GL_RGBA;
		}
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	} else {
		std::cout << "Failed to load texture " << filepath << '\n';
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(data);
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
