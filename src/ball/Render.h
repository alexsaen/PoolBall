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

#ifndef RENDER_H
#define RENDER_H

#include "math3d.h"
#include "VBO.h"
#include "FBO.h"
#include "Shader.h"
#include "Texture.h"
#include "Font.h"
#include "trackball.h"

#include <vector>

class RenderResource;

static const unsigned int ATTRIB_POSITION = 0;
static const unsigned int ATTRIB_TEXCOORD = 1;
static const unsigned int ATTRIB_LEVEL	  = 6;
static const unsigned int ATTRIB_NORMAL	  = 7;

struct V2TV2Vert {
	vec2	v, tv;
	V2TV2Vert()													{}
	V2TV2Vert(const vec2& av, const vec2& atv): v(av), tv(atv)	{}
};

class ChapterCombat;

class Render {
	friend class RenderResource;
	int		width, height;
	float	aspect;
	rect	bounds;

	VertexShader	fontVertShader, ballVertShader, tableVertShader, shadowVertShader;
	FragmentShader	fontFragShader, ballFragShader, tableFragShader, shadowFragShader;
	ShaderProgram	fontShaderProgram, ballShaderProgram, tableShaderProgram, shadowShaderProgram;
	
	mat4			transform, modelview;
	mat3			normalMat;
	ShaderProgram	*curShader;
	Font			font;

	Texture			ballTexture;
	Cubemap			envTexture;		
	float			rotateAngle;

	VBOVertex		ballVertsVBO;
	VBOIndex		ballIndexVBO;
	int				ballIdxCount;

					Render();
	virtual			~Render();

	Trackball		trackball;
	vec3			ballPos, eyePos;
	vec4			lightPos;

	void	drawBall();
	void	drawTable();
	void	drawShadow();
	void	buildGSphere(float radius, int nstacks, int nslices);

public:

	static	Render	&instance();
	static	void	destroy();

			void	reshape(int w, int h);
	static	void	release();

	void	setStdShader(const mat4 &transform);
	void	beginFont(const mat4 &transform);
	Font&	getFont()														{	return font;	}
	float	getAspect()														{	return aspect;		}

	void	setShader(ShaderProgram *sp);
	void	setColor(const vec4 &c);
			
	const	rect &getBounds()												{	return bounds;	}	

	void	draw();

	void	moveBall(const vec3 &delta);
	void	moveLight(const vec3 &delta);

	void	touchBegan(int id, int x, int y);
	void	touchMove(int id, int x, int y);
	void	touchEnded(int id);
	void	keyDown(int kid);
};

#endif 
