/*  
	Copyright (c) 2012, Alexey Saenko
	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at

		http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
*/ 

#include "Texture.h"
#include "opengl.h"
#include "ResourceManager.h"
#include <string>

TextureBase::TextureBase(unsigned int t): type(t), tid(0) {}

void TextureBase::release() {
	if(tid) {
		glDeleteTextures(1, &tid);
		tid = 0;
	}
}

void TextureBase::bind() {
	if(tid)
		glBindTexture(type, tid);
}

void TextureBase::unbind() {
	glBindTexture(type, 0);
}

bool TextureBase::loadFromPNG(unsigned int target, const char *filename) {
	const char *texPath = "assets/textures/";
	std::string path(texPath);
	path += filename;

	int width, height;
	unsigned char* png = ResourceManager::instance()->readPNG(path.c_str(), width, height);
	if(!png)
		return false;

	init(target, png, width, height, 3);
	delete png;
	return true;
}

void TextureBase::init(unsigned int target, unsigned char *buf, int width, int height, int depth) {
	GLenum format = depth == 3 ? GL_RGB : GL_RGBA;
	glTexImage2D(target, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, buf);
}


Texture::Texture(): TextureBase(GL_TEXTURE_2D) {}

bool Texture::loadFromPNG(const char *filename) {
	if(!tid)
		glGenTextures(1, &tid);

	glBindTexture(GL_TEXTURE_2D, tid);

	if(!TextureBase::loadFromPNG(GL_TEXTURE_2D, filename))
		return false;

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	return true;
}

Cubemap::Cubemap(): TextureBase(GL_TEXTURE_CUBE_MAP) {}

bool Cubemap::loadFromPNG(const char *filename[]) {
	if(!tid)
		glGenTextures(1, &tid);

	glBindTexture(GL_TEXTURE_CUBE_MAP, tid);

	for(int i = 0; i<6; ++i) {
		if(!TextureBase::loadFromPNG(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, filename[i]) )
			return false;
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	return true;
}
