#pragma once

#include <GL/glew.h>
#include <string>

class Texture {
	friend auto main(int argc, char* args[]) -> int;

public:
	Texture();

	Texture(int width, int height, int format = GL_RGB);

	Texture(const std::string& filepath);
	~Texture();

	Texture(const Texture&) = delete;
	auto operator=(const Texture&) -> Texture& = delete;

	Texture(Texture&& other) noexcept;
	auto operator=(Texture&& other) noexcept -> Texture&;

	void bind(unsigned int unit = 0) const;

	void setDepthBuffer() const { glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m.id, 0); }

private:
	struct {
		GLuint id = 0;
	} m;
};
