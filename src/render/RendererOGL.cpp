#include <assert.h>
#include <iostream>
#include "render/RendererOGL.h"
#include "common/Mat44.h"
#include "GL/gl.h"
using namespace std;

RendererOGL::RendererOGL(){
	glClearColor(0.6, 0.85, 1.0, 0.0);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
	glShadeModel (GL_SMOOTH);
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_shininess[] = { 50.0 };
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	
	glEnable(GL_TEXTURE_2D);
}

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
	GLfloat ambientLight[] = { 0.8f, 0.8f, 0.8f, 1.0f }; 
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight); 

	for(const auto&req:render_q){
		glLoadMatrixf((const float*)req.to_world.transposed().a);
		if(req.textureID!=-1){
			glBindTexture(GL_TEXTURE_2D, req.textureID);
		}
		glBegin(PrimitiveType2GL(req.primitive_type));
		if(req.primitive_type==PrimitiveType::lines){
			glColor3bv((GLbyte*)&req.textureID);
			for(auto vtx:req.vertices)
				glVertex3f(vtx.x,vtx.y,vtx.z);
		}else{
			for(auto face:req.faces){
				for(auto i:face){
					if(req.textureID!=-1)
						glTexCoord2f(req.texcoord[i].x,req.texcoord[i].y);
					glNormal3f(req.normals[i].x,req.normals[i].y,req.normals[i].z);
					glVertex3f(req.vertices[i].x,req.vertices[i].y,req.vertices[i].z);
				}
			}
		}
		glEnd();
	}
}

//https://www.khronos.org/opengl/wiki/GluPerspective_code
void RendererOGL::setProjection(float fovyInDegrees, float aspectRatio, float znear, float zfar){
	auto glhPerspectivef2=[](float* matrix, float fovyInDegrees, float aspectRatio, float znear, float zfar){
		float ymax, xmax;
		ymax = znear * tanf(fovyInDegrees * M_PI / 360.0);
		xmax = ymax * aspectRatio;
		auto glhFrustumf2=[](float* mtx, float left, float right, float bottom, float top, float znear, float zfar){
			float temp1, temp2, temp3, temp4;
			temp1 = 2.0 * znear;
			temp2 = right - left;
			temp3 = top - bottom;
			temp4 = zfar - znear;
			mtx[0] = temp1 / temp2;
			mtx[1] = mtx[2] = mtx[3] = mtx[4] = 0.0;
			mtx[5] = temp1 / temp3;
			mtx[6] = mtx[7] = 0.0;
			mtx[8] = (right + left) / temp2;
			mtx[9] = (top + bottom) / temp3;
			mtx[10] = (-zfar - znear) / temp4;
			mtx[11] = -1.0;
			mtx[12] = mtx[13] = 0.0;
			mtx[14] = (-temp1 * zfar) / temp4;
			mtx[15] = 0.0;
		};
		glhFrustumf2(matrix, -xmax, xmax, -ymax, ymax, znear, zfar);
	};
	Mat44 projection;
	glhPerspectivef2((float*)projection.a, fovyInDegrees, aspectRatio, znear, zfar);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf((float*)projection.a);
}
void RendererOGL::setViewport(int width, int height){
	glViewport(0, 0, width, height);
}