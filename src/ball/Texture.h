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

#ifndef TEXTURE_H_
#define TEXTURE_H_

#include "RenderResource.h"

class TextureBase: public RenderResource {
public:
					TextureBase(unsigned int t);

	virtual	void	release();
			void	bind();
			void	unbind();

protected:
	unsigned int type;
	unsigned int tid;
			bool	loadFromPNG(unsigned int target, const char *filename);
			void	init(unsigned int target, unsigned char *buf, int width, int height, int depth);
};

class Texture: public TextureBase {
public:
					Texture();
			bool	loadFromPNG(const char *filename);
};

class Cubemap: public TextureBase {
public:
					Cubemap();
			bool	loadFromPNG(const char *filename[]);
};

#endif 