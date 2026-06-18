#pragma once

#include <GL/glew.h>
#include <string>

class Texture {
public:
	Texture();
	Texture(const std::string& filepath);
	~Texture();

	Texture(const Texture&) = delete;
	auto operator=(const Texture&) -> Texture& = delete;

	Texture(Texture&& other) noexcept;
	auto operator=(Texture&& other) noexcept -> Texture&;

	void bind(unsigned int unit = 0) const;

private:
	struct {
		GLuint id = 0;
	} m;
};
