#include <assert.h>
#include "render/impl/ogl/RendererOGL.h"
#include "GL/gl.h"

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
	
	//temporal camera transform code
	glTranslatef(0,0,-10);
	glRotatef(45,1,0,0);
	glRotatef(45,0,1,0);
	
	for(const auto&req:render_q){
		glMultMatrixf((const float*)req.to_world.transposed().a);
		glBegin(PrimitiveType2GL(req.primitive_type));
		for(auto face:req.faces){
			for(auto i:face){
				glVertex3f(req.vertices[i].x,req.vertices[i].y,req.vertices[i].z);
			}
		}
		glEnd();
	}
}