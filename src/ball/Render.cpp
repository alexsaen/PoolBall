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

#include "Render.h"
#include "RenderResource.h"

#include "opengl.h"
#include "FBO.h"
#include "Shader.h"
#include "Texture.h"
#include "utils.h"

static std::vector<RenderResource*>	resources;

RenderResource::RenderResource() {
	resources.push_back(this);
}

RenderResource::~RenderResource() {
	vector_fast_remove(resources, this);
}

Render::Render(): transform(1.0f), curShader(0), ballPos(0,0,1), lightPos(5,5,10,0), rotateAngle(0)
{}

Render::~Render() {
	release();
}

static Render *render = 0;
Render& Render::instance() {
	if(!render)
		render = new Render();
	return *render;
}

void Render::destroy() {
	if(render) {
		delete render;
		render = 0;
	}
}

void Render::release() {
	for(std::vector<RenderResource*>::iterator r = resources.begin(); r != resources.end(); ++r)
		(*r)->release();
}

struct V3N3T2 {
	vec3 v, n;
	vec2 tv;
	V3N3T2() {}
	V3N3T2(const vec3& av, const vec3& an, const vec2 &atv): v(av), n(an), tv(atv) {}
}; 

void Render::buildGSphere(float radius, int nstacks, int nslices) {
	std::vector<V3N3T2>	bverts;
	std::vector<unsigned short>	bidxs;

	float phiStep = PI / nstacks;

	for( int i = 1; i < nstacks; ++i ) {
		float phi = i * phiStep;

		float thetaStep = 2.0f * PI / nslices;
		for( int j = 0; j <= nslices; ++j ) {
			float theta = j * thetaStep;

			vec3 pos(-radius * sinf(phi) * cosf(theta),
				-radius * cosf(phi),
				radius * sinf(phi) * sinf(theta));

			vec3 normal = pos;
			normal.normalize();

			vec2 uv;
			uv.x = theta / (2.0f * PI);
			uv.y = phi / PI;

			bverts.push_back(V3N3T2(pos, normal, uv));
		}
	}

	bverts.push_back( V3N3T2(vec3(0.0f, radius, 0),  vec3(0.0f, 1.0f, 0.0f),  vec2(0.0f, 1.0f)) );
	bverts.push_back( V3N3T2(vec3(0.0f, -radius, 0), vec3(0.0f, -1.0f, 0.0f), vec2(0.0f, 0.0f)) );

	int npidx = (int)bverts.size() - 1;
	int spidx = (int)bverts.size() - 2;

	int nverts = nslices + 1;

	for(int i = 0; i < nstacks - 2; ++i) {
		for(int j = 0; j < nslices; ++j) {
			bidxs.push_back(i * nverts + j);
			bidxs.push_back(i * nverts + j + 1);
			bidxs.push_back((i + 1) * nverts + j);

			bidxs.push_back((i + 1)  *nverts + j);
			bidxs.push_back(i * nverts + j + 1);
			bidxs.push_back((i + 1)  *nverts + j + 1);
		}
	}

	for(int i = 0; i < nslices; ++i) {
		bidxs.push_back(npidx);
		bidxs.push_back(i + 1);
		bidxs.push_back(i);
	}

	int bidx = (nstacks - 2) * nverts;
	for(int i = 0; i < nslices; ++i) {
		bidxs.push_back(spidx);
		bidxs.push_back(bidx + i);
		bidxs.push_back(bidx + i + 1);
	}

	ballVertsVBO.bind();
	ballVertsVBO.setData(bverts.size() * sizeof(V3N3T2), GL_STATIC_DRAW, &bverts[0]);
	ballVertsVBO.unbind();

	ballIndexVBO.bind();
	ballIndexVBO.setData(bidxs.size() * sizeof(unsigned short), GL_STATIC_DRAW, &bidxs[0]);
	ballIndexVBO.unbind();
	ballIdxCount = bidxs.size();

}

void Render::reshape(int w, int h) {
	width = w;
	height = h;
	aspect = (float) width / (float) height;

	trackball.reshape(w, h);

	ballVertShader.load("ball_vert.glsl");
	ballFragShader.load("ball_frag.glsl");
	ballShaderProgram.attach(&ballVertShader, &ballFragShader); 
	ballShaderProgram.bindAttrib(ATTRIB_POSITION, "position");
	ballShaderProgram.bindAttrib(ATTRIB_TEXCOORD, "texcoord");
	ballShaderProgram.bindAttrib(ATTRIB_NORMAL, "normal");
	ballShaderProgram.link();

	tableVertShader.load("table_vert.glsl");
	tableFragShader.load("table_frag.glsl");
	tableShaderProgram.attach(&tableVertShader, &tableFragShader); 
	tableShaderProgram.bindAttrib(ATTRIB_POSITION, "position");
	tableShaderProgram.bindAttrib(ATTRIB_NORMAL, "normal");
	tableShaderProgram.link();

	shadowVertShader.load("shadow_vert.glsl");
	shadowFragShader.load("shadow_frag.glsl");
	shadowShaderProgram.attach(&shadowVertShader, &shadowFragShader); 
	shadowShaderProgram.bindAttrib(ATTRIB_POSITION, "position");
	shadowShaderProgram.bindAttrib(ATTRIB_TEXCOORD, "texcoord");
	shadowShaderProgram.link();

	fontVertShader.load("font_vert.glsl");
	fontFragShader.load("font_frag.glsl");
	fontShaderProgram.attach(&fontVertShader, &fontFragShader); 
	fontShaderProgram.bindAttrib(ATTRIB_POSITION, "position");
	fontShaderProgram.bindAttrib(ATTRIB_TEXCOORD, "texcoord");
	fontShaderProgram.link();

	font.init("assets/fonts/Cuprum-Regular.ttf", 30);

	ballTexture.loadFromPNG("Pool_Ball.png");

	const char * cmap[6] = {
		"cm_left.png",
		"cm_right.png",
		"cm_top.png",
		"cm_bottom.png",
		"cm_back.png",
		"cm_front.png",
	};

	envTexture.loadFromPNG(cmap);

	buildGSphere(1.0f, 100, 100);
}


void Render::setShader(ShaderProgram *sp) {
	if(curShader == sp)
		return;
	curShader = sp;
	if(curShader) 
		curShader->use();
	else
		ShaderProgram::unuse();
}

void Render::setColor(const vec4 &c) {
	if(curShader)
		curShader->uniform(SU_COLOR, c);
}

void Render::beginFont(const mat4 &transform) {
	setShader(&fontShaderProgram);
	fontShaderProgram.uniform(SU_TRANSFORM, transform);
	fontShaderProgram.uniform(SU_TEX0, 0);
}

void Render::draw() {

	glClearColor(0.0f,0.0f,0.0f,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	rotateAngle += 0.5f;

	modelview = mat4::get_translate(ballPos) * mat4::get_rotate_z(rotateAngle) * mat4::get_rotate_y(rotateAngle);

	mat4 eyeMat = mat4::get_translate(0, 0, -5) * trackball.transform();
	eyePos = eyeMat.inverse() * vec3(0.0f);

	transform = mat4::get_perspective(60, aspect, 0.1f, 1000.0f) * eyeMat;
	normalMat = mat3(modelview).inverse().transpose();

	drawTable();
	if(eyePos.z > 0) {
		drawShadow();
		drawBall();
	}

	glDisable(GL_DEPTH_TEST);
	mat4 view2D = mat4::get_ortho(-aspect, aspect, -1, 1);
	beginFont(view2D);
	const float fontSize = 0.09f;
	setColor(vec4(1, 0, 0, 1));
	font.draw(-aspect, -0.95f, fontSize, " Use mouse to rotate scene. Use Arrows Up, Down to move the ball. Use Arrows Left, Right to move the light.");
}

void Render::drawTable() {
	static const float tsize = 2000.0f;
	static const vec3 verts[4] =   { vec3(-tsize, -tsize, 0), vec3(-tsize, tsize, 0), vec3(tsize, tsize, 0), vec3(tsize, -tsize, 0)  };
	static const vec3 normals[4] = { vec3(0,0,1), vec3(0,0,1), vec3(0,0,1), vec3(0,0,1) };

	setShader(&tableShaderProgram);
	tableShaderProgram.uniform(SU_TRANSFORM, transform);
	tableShaderProgram.uniform(SU_LIGHT_POS, lightPos);
	tableShaderProgram.uniform(SU_EYE_POS, eyePos);

	glEnableVertexAttribArray(ATTRIB_POSITION);
	glVertexAttribPointer(ATTRIB_POSITION, 3, GL_FLOAT, false, sizeof(vec3), verts);
	glEnableVertexAttribArray(ATTRIB_NORMAL);
	glVertexAttribPointer(ATTRIB_NORMAL, 3, GL_FLOAT, false, sizeof(vec3), normals);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDisableVertexAttribArray(ATTRIB_POSITION);
	glDisableVertexAttribArray(ATTRIB_NORMAL);
}

void Render::drawShadow() {
	vec3 ray = ballPos - lightPos;
	ray.normalize();
	vec3 dx = cross(ray, vec3(0,0,1));
	dx.normalize();
	vec3 dy = cross(ray, dx);
	dy.normalize();
	vec3 v[4] = { ballPos-dx-dy, ballPos-dx+dy, ballPos+dx+dy, ballPos+dx-dy };
	vec3 n(0, 0, 1);

	vec3 verts[4];
	vec3 texCoords[4] = { vec3(-1,-1,1), vec3(-1,1,1), vec3(1,1,1), vec3(1,-1,1) };

	for(int i=0; i<4; ++i) {
		vec3 r = v[i]-lightPos;
		r.normalize();
		float d =  -lightPos.z / (r*n);
		verts[i] = lightPos + r*d;
		texCoords[i].z = d * v[i].z/lightPos.z; 
	}

	glDisable(GL_DEPTH_TEST);
	setShader(&shadowShaderProgram);
	shadowShaderProgram.uniform(SU_TRANSFORM, transform);
	shadowShaderProgram.uniform(SU_LIGHT_POS, lightPos);
	shadowShaderProgram.uniform(SU_EYE_POS, eyePos);

	glEnable(GL_BLEND);
	glBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);

	glEnableVertexAttribArray(ATTRIB_POSITION);
	glVertexAttribPointer(ATTRIB_POSITION, 3, GL_FLOAT, false, sizeof(vec3), verts);
	glEnableVertexAttribArray(ATTRIB_TEXCOORD);
	glVertexAttribPointer(ATTRIB_TEXCOORD, 3, GL_FLOAT, false, sizeof(vec3), texCoords);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDisableVertexAttribArray(ATTRIB_POSITION);
	glDisableVertexAttribArray(ATTRIB_TEXCOORD);
	glDisable(GL_BLEND);

	glEnable(GL_DEPTH_TEST);
}


void Render::drawBall() {
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	ballTexture.bind();
	glActiveTexture(GL_TEXTURE1);
	glEnable(GL_TEXTURE_2D);
	envTexture.bind();

	setShader(&ballShaderProgram);
	ballShaderProgram.uniform(SU_TRANSFORM, transform * modelview);
	ballShaderProgram.uniform(SU_MODELVIEW, modelview);
	ballShaderProgram.uniform(SU_NORMAL_MAT, normalMat);
	ballShaderProgram.uniform(SU_LIGHT_POS, lightPos);
	ballShaderProgram.uniform(SU_EYE_POS, eyePos);
	ballShaderProgram.uniform(SU_TEX0, 0);
	ballShaderProgram.uniform(SU_ENV_MAP, 1);

	glEnable(GL_TEXTURE_2D);
	ballVertsVBO.bind();
	ballIndexVBO.bind();

	glEnableVertexAttribArray(ATTRIB_POSITION);
	glVertexAttribPointer(ATTRIB_POSITION, 3, GL_FLOAT, false, sizeof(V3N3T2), &((V3N3T2*)0)->v);
	glEnableVertexAttribArray(ATTRIB_NORMAL);
	glVertexAttribPointer(ATTRIB_NORMAL, 3, GL_FLOAT, false, sizeof(V3N3T2), &((V3N3T2*)0)->n);
	glEnableVertexAttribArray(ATTRIB_TEXCOORD);
	glVertexAttribPointer(ATTRIB_TEXCOORD, 2, GL_FLOAT, false, sizeof(V3N3T2), &((V3N3T2*)0)->tv);
	glDrawElements(GL_TRIANGLES, ballIdxCount, GL_UNSIGNED_SHORT, 0);
	glDisableVertexAttribArray(ATTRIB_POSITION);
	glDisableVertexAttribArray(ATTRIB_NORMAL);
	glDisableVertexAttribArray(ATTRIB_TEXCOORD);

	ballVertsVBO.unbind();
	ballIndexVBO.unbind();
	glActiveTexture(GL_TEXTURE1);
	glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	glDisable(GL_TEXTURE_2D);
}

void Render::moveBall(const vec3 &delta) {
	ballPos += delta;
	ballPos.z = std::min(std::max(ballPos.z, 1.0f), 5.0f);
}

void Render::moveLight(const vec3 &delta) {
	lightPos += delta;
	lightPos.x = std::min(std::max(lightPos.x, -10.0f), 10.0f);
	lightPos.y = std::min(std::max(lightPos.y, -10.0f), 10.0f);
}

void Render::touchBegan(int id, int x, int y) {
	trackball.mouseDown(x, y);
}

void Render::touchMove(int id, int x, int y) {
	trackball.mouseMove(x, y);
}

void Render::touchEnded(int id) {
	trackball.mouseUp();
}

void Render::keyDown(int kid) {
	switch(kid) {
	case 273:
		moveBall(vec3(0,0,0.025f));
		break;
	case 274:
		moveBall(vec3(0,0,-0.025f));
		break;
	case 275:
		moveLight(vec3(0.1f,0.1f,0));
		break;
	case 276:
		moveLight(vec3(-0.1f,-0.1f,0));
		break;
	default:
		break;
	}
}
