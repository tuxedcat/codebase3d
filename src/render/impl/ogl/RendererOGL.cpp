#include <assert.h>
#include <iostream>
#include "render/impl/ogl/RendererOGL.h"
#include "GL/gl.h"
using namespace std;

GLenum PrimitiveType2GL(PrimitiveType prim_type){
	switch(prim_type){
		case PrimitiveType::points:
			return GL_POINTS;
		case PrimitiveType::lines:
			return GL_LINES;
		case PrimitiveType::triangles:
			return GL_TRIANGLES;
		default:
			throw "Unimplemented";
	}
	assert(false);
}
		#include <iostream>
		using namespace std;

void RendererOGL::render(const std::vector<RenderRequest>& render_q){
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	// GLfloat light_pos[4]={camX,camY,camZ,0.0};//Light pos = camera pos
	GLfloat light_pos[4]={1,1,1,0};//Light pos = absolute
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
	
	glEnable(GL_TEXTURE_2D);

	for(const auto&req:render_q){
		glLoadMatrixf((const float*)req.to_world.transposed().a);
		glBindTexture(GL_TEXTURE_2D, req.textureID);
		glBegin(PrimitiveType2GL(req.primitive_type));
		for(auto face:req.faces){
			for(auto i:face){
				if(req.textureID!=-1)
					glTexCoord2f(req.texcoord[i].x,req.texcoord[i].y);
				glNormal3f(req.normals[i].x,req.normals[i].y,req.normals[i].z);
				glVertex3f(req.vertices[i].x,req.vertices[i].y,req.vertices[i].z);
			}
		}
		glEnd();
	}

	glDisable(GL_TEXTURE_2D);
}