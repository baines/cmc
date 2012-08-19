#include "shader.h"
#include <stdio.h>
#ifndef GL_GLEXT_PROTOTYPES
	#define GL_GLEXT_PROTOTYPES 1
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>
#include <stdlib.h>

static void doload(const char* name, GLuint* id, GLenum type){
	glGenProgramsARB(1, id);
	glBindProgramARB(type, *id);
	
	FILE* f = fopen(name, "rb");
	if(f == NULL){
		fprintf(stderr, "Couldn't load %s\n", name);
		exit(1);
	}
	fseek(f, 0L, SEEK_END);
	int sz = ftell(f);
	char* prog = malloc(sz+1);
	fseek(f, 0L, SEEK_SET);
	sz = fread(prog, 1, sz, f);
	prog[sz] = '\0';
	fclose(f);
	
	glProgramStringARB(type, GL_PROGRAM_FORMAT_ASCII_ARB, sz, prog);
	if(glGetError() == GL_INVALID_OPERATION){
		const GLubyte *errString = glGetString(GL_PROGRAM_ERROR_STRING_ARB);
		fprintf(stderr, "Error(s) compiling shader %s:\n %s\n", name, errString);
		exit(1);
	}
	
	free(prog);
}

void shader_load(struct shader* s, const char* basename){
	char vs_name[256], fs_name[256];
	snprintf(vs_name, 256, "./data/txt/%s.vs", basename);
	snprintf(fs_name, 256, "./data/txt/%s.fs", basename);
	
	doload(vs_name, &(s->v_id), GL_VERTEX_PROGRAM_ARB);
	doload(fs_name, &(s->f_id), GL_FRAGMENT_PROGRAM_ARB);
}
