/*

CG Homework2 - Phong Shading, Dissolving effects and Ramp effects

Objective - learning GLSL, glm model datatype(for .obj file) 

Overview:

	1. Render the model with texture using VBO and/or VAO

	2. Implement Phong shading

	3. Implement Dissolving effects via shader

	4. Implement Ramp effects via shader (Basically a dicrete shading)

Good luck!

!!!IMPORTANT!!! 

1. Make sure to change the window name to your student ID!
2. Make sure to remove glmDraw() at line 211.
   It's meant for a quick preview of the ball object and shouldn't exist in your final code.

*/
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h> /*for function: offsetof */
#include <math.h>
#include <string.h>
#include "../GL/glew.h"
#include "../GL/glut.h""
#include "../shader_lib/shader.h"
#include "glm/glm.h"
extern "C"
{
	#include "glm_helper.h"
}

/*you may need to do something here
you may use the following struct type to perform your single VBO method,
or you can define/declare multiple VBOs for VAO method.
Please feel free to modify it*/
struct Vertex
{
	GLfloat position[3];
	GLfloat normal[3];
	GLfloat texcoord[2];
};
typedef struct Vertex Vertex;

//no need to modify the following function declarations and gloabal variables
void calcNormalMatrix(float* ans,float *mv);
void calcMVP(float* mvp, float *mv, float *p);
void init(void);
void display(void);
void reshape(int width, int height);
void keyboard(unsigned char key, int x, int y);
void keyboardup(unsigned char key, int x, int y);
void motion(int x, int y);
void mouse(int button, int state, int x, int y);
void idle(void);
void draw_light_bulb(void);
void camera_light_ball_move();
void timetick(int);
inline float float_rand();
GLuint loadTexture(char* name, GLfloat width, GLfloat height);

namespace
{
	char *obj_file_dir = "../Resources/bunny.obj";
	char *main_tex_dir = "../Resources/Stone.ppm";
	char *noise_tex_dir = "../Resources/Noise.ppm";
	char *ramp_tex_dir = "../Resources/Ramp.ppm";
	
	GLfloat light_rad = 0.05;//radius of the light bulb
	float eyet = 0.0;//theta in degree
	float eyep = 90.0;//phi in degree
	bool mleft = false;
	bool mright = false;
	bool mmiddle = false;
	bool forward = false;
	bool backward = false;
	bool left = false;
	bool right = false;
	bool up = false;
	bool down = false;
	bool lforward = false;
	bool lbackward = false;
	bool lleft = false;
	bool lright = false;
	bool lup = false;
	bool ldown = false;
	bool llforward = false;
	bool llbackward = false;
	bool llleft = false;
	bool llright = false;
	bool llup = false;
	bool lldown = false;
	bool bforward = false;
	bool bbackward = false;
	bool bleft = false;
	bool bright = false;
	bool bup = false;
	bool bdown = false;
	bool bx = false;
	bool by = false;
	bool bz = false;
	bool brx = false;
	bool bry = false;
	bool brz = false;
	int mousex = 0;
	int mousey = 0;
}

// You can modify the moving/rotating speed if it's too fast/slow for you
const float speed = 0.03; // camera / light / ball moving speed
const float rotation_speed = 0.3; // ball rotating speed

//you may need to use some of the following variables in your program 

// No need for model texture, 'cause glmModel() has already loaded it for you.
// To use the texture, check   .
GLuint mainTextureID; // TA has already loaded this texture for you
GLuint noiseTextureID; // TA has already loaded this texture for you
GLuint rampTextureID; // TA has already loaded this texture for you

GLMmodel *model; //TA has already loaded the model for you(!but you still need to convert it to VBO(s)!)

float eyex = 0.0;
float eyey = 0.0;
float eyez = 5.6;

GLint light_count = 1;
GLfloat light_pos[] = { 1.1, 1.0, 1.3 };
GLfloat light0_ambient[] = { 0.2, 0.2, 0.2 };
GLfloat light0_disffuse[] = { 0.8, 0.8, 0.8 };
GLfloat light0_spec[] = { 0.5, 0.5, 0.5 };
GLfloat light1_ambient[] = { 0.8, 0.8, 0.8 };
GLfloat light1_disffuse[] = { 1, 1,1 };
GLfloat light1_spec[] = { 0.9, 0.9, 0.9 };
GLfloat material_ra[] = { 0.3, 0.3, 0.3 };
GLfloat material_rd[] = { 0.8, 0.8, 0.8 };
GLfloat material_rs[] = { 0.4, 0.4, 0.4 };
GLfloat light1_pos[] = { -1.1, 1.0, 1.3 };
GLfloat ball_pos[] = { 0.0, 0.0, 0.0 };
GLfloat ball_rot[] = { 0.0, 0.0, 0.0 };
GLfloat material_shininess = 100.0F;

GLfloat dissolving_color[] = { 1.0F , 1.0F , 0.0 };
GLfloat dissolving_threshold = 0.0F;
bool enable_dissolving_animation = false;

GLuint programs[3];

GLuint vboName;

unsigned vertices_count;
unsigned coords_count;
GLfloat* coords;

unsigned draw_mode = 0;
const unsigned draw_mode_count = 3;

struct MyVertex {
	float x, y, z;        // Vertex
	float nx, ny, nz;     // Normal
	float u, v;         // Texcoord0
};
MyVertex *vertices;

inline void assign(float vec[3], float a, float b, float c) {
	vec[0] = a, vec[1] = b, vec[2] = c;
}


int main(int argc, char *argv[])
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutCreateWindow("CG_HW2_0756078");
	glutReshapeWindow(512, 512);

	glewInit();

	init();

	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyboardup);
	glutTimerFunc(30, timetick, 0);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	

	glutMainLoop();


	free(vertices);
	glmDelete(model);
	glDeleteBuffers(1, &vboName);

	return 0;
}
struct _locations {
	GLint draw_mode;
	//phone
	GLint light0_pos;
	GLint light0_la;
	GLint light0_ld;
	GLint light0_ls;
	GLint light1_pos;
	GLint light1_la;
	GLint light1_ld;
	GLint light1_ls;
	GLint light_count;
	GLint m_ka;
	GLint m_kd;
	GLint m_ks;
	GLint m_sh;
	//dissolving
	GLint dissolving_color;
	GLint dissolving_threshold;
	GLint NM;
	GLint MV;
	GLint MVP;
} locations;

void calcNormalMatrix(float * ans, float * mv)
{
	// mv = 4x4 model view matrix
	// ans
	// 0 1 2
	// 3 4 5
	// 6 7 8

	// 11 12 13      mv[0] mv[1] mv[2]
	// 21 22 23   =  mv[4] mv[5] mv[6]
	// 31 32 33      mv[8] mv[9] mv[10]
	// inverse + transpose
	ans[0] = mv[5] * mv[10] - mv[6] * mv[9];
	ans[1] = -(mv[4] * mv[10] - mv[6] * mv[8]);
	ans[2] = mv[4] * mv[9] - mv[5] * mv[8];
	ans[3] = -(mv[1] * mv[10] - mv[2] * mv[9]);
	ans[4] = mv[0] * mv[10] - mv[2] * mv[8];
	ans[5] = -(mv[0] * mv[9] - mv[1] * mv[8]);
	ans[6] = mv[1] * mv[6] - mv[2] * mv[5];
	ans[7] = -(mv[0] * mv[6] - mv[2] * mv[7]);
	ans[8] = mv[0] * mv[5] - mv[1] * mv[4];

	float det = ans[0] * ans[4] * ans[8]
		+ ans[1] * ans[5] * ans[6]
		+ ans[2] * ans[3] * ans[7]
		- ans[2] * ans[4] * ans[6]
		- ans[1] * ans[3] * ans[8]
		- ans[0] * ans[5] * ans[7];
	for (unsigned i = 0; i < 9; ++i) {
		ans[i] /= det;
	}
}

void calcMVP(float * mvp, float * mv, float * p)
{
	unsigned i, j, k;
	for (i = 0; i < 4; ++i) {
		for (j = 0; j < 4; ++j) {
			mvp[i + j * 4] = 0.0;
			for (k = 0; k < 4; ++k) {
				mvp[i + j*4] += p[i + k*4] * mv[k + j*4];
			}
		}
	}
}

void init(void)
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glEnable(GL_CULL_FACE);
	model = glmReadOBJ(obj_file_dir);

	mainTextureID = loadTexture(main_tex_dir, 512, 256);
	noiseTextureID = loadTexture(noise_tex_dir, 360, 360);
	rampTextureID = loadTexture(ramp_tex_dir, 256, 256);
	
	glmUnitize(model);
	glmFacetNormals(model);
	glmVertexNormals(model, 90.0, GL_FALSE);
	glEnable(GL_DEPTH_TEST);
	print_model_info(model);

	//you may need to do something here(create shaders/program(s) and create vbo(s)/vao from GLMmodel model)



	vertices_count = model->numtriangles * 3;
	coords_count = vertices_count * 8;

	vertices = (MyVertex*)malloc(sizeof(MyVertex) * vertices_count);

	unsigned indx = 0;
	for (unsigned i = 0; i < model->numtriangles; ++i) {
		for (unsigned j = 0; j < 3; ++j) {
			memcpy(&vertices[indx].x,&model->vertices[model->triangles[i].vindices[j] * 3],sizeof(float)*3);
			memcpy(&vertices[indx].nx, &model->normals[model->triangles[i].nindices[j] * 3], sizeof(float) * 3);
			memcpy(&vertices[indx].u, &model->texcoords[model->triangles[i].tindices[j] * 2], sizeof(float) * 2);
			indx++;
			
		}
	}

	#define BUFFER_OFFSET(i) ((void*)(i))

	glGenBuffers(1, &vboName);
	glBindBuffer(GL_ARRAY_BUFFER, vboName);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * coords_count, &vertices[0].x, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3,GL_FLOAT,GL_FALSE,sizeof(MyVertex),BUFFER_OFFSET(0));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(MyVertex), BUFFER_OFFSET(12));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(MyVertex), BUFFER_OFFSET(24));




	GLuint vert = createShader("Shaders/VS.vert", "vertex");
	GLuint frag = createShader("Shaders/FS.frag", "fragment");
	programs[0] = createProgram(vert, frag);

	locations.draw_mode = glGetUniformLocation(programs[0], "_draw_mode");
	//phone
	locations.light0_pos = glGetUniformLocation(programs[0], "Light[0].Position");
	locations.light0_la = glGetUniformLocation(programs[0], "Light[0].La");
	locations.light0_ld = glGetUniformLocation(programs[0], "Light[0].Ld");
	locations.light0_ls = glGetUniformLocation(programs[0], "Light[0].Ls");
	locations.light1_pos = glGetUniformLocation(programs[0], "Light[1].Position");
	locations.light1_la = glGetUniformLocation(programs[0], "Light[1].La");
	locations.light1_ld = glGetUniformLocation(programs[0], "Light[1].Ld");
	locations.light1_ls = glGetUniformLocation(programs[0], "Light[1].Ls");
	locations.light_count = glGetUniformLocation(programs[0], "light_count");
	locations.m_ka = glGetUniformLocation(programs[0], "Material.Ka");
	locations.m_kd = glGetUniformLocation(programs[0], "Material.Kd");
	locations.m_ks = glGetUniformLocation(programs[0], "Material.Ks");
	locations.m_sh = glGetUniformLocation(programs[0], "Material.Shininess");
	//dissolving
	locations.dissolving_color = glGetUniformLocation(programs[0], "dissolving_color");
	locations.dissolving_threshold = glGetUniformLocation(programs[0], "dissolving_threshold");

	locations.NM = glGetUniformLocation(programs
		[0], "NM");
	locations.MV = glGetUniformLocation(programs[0], "MV");
	locations.MVP = glGetUniformLocation(programs[0], "MVP");
}



void display(void)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//you may need to do something here(de	clare some local variables you need and maybe load Model matrix here...)
	GLfloat modelview[16];
	GLfloat projection[16];
	GLfloat normalMatrix[9]; 
	GLfloat MVP[16];
	GLfloat light_modelview[16];
	GLfloat *m;


	//please try not to modify the following block of code(you can but you are not supposed to)

	glMatrixMode(GL_PROJECTION);

	glGetFloatv(GL_PROJECTION_MATRIX, projection);

	/*
	m = projection;
	printf("%f %f %f %f\n", m[0], m[4], m[8], m[12]);
	printf("%f %f %f %f\n", m[1], m[5], m[9], m[13]);
	printf("%f %f %f %f\n", m[2], m[6], m[10], m[14]);
	printf("%f %f %f %f\n", m[3], m[7], m[11], m[15]);
	*/

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
	gluLookAt(
		eyex,
		eyey,
		eyez,
		eyex + cos(eyet*M_PI / 180)*cos(eyep*M_PI / 180),
		eyey + sin(eyet*M_PI / 180),
		eyez - cos(eyet*M_PI / 180)*sin(eyep*M_PI / 180),
		0.0,
		1.0,
		0.0);
	draw_light_bulb();


	glGetFloatv(GL_MODELVIEW_MATRIX, light_modelview);
	
	glPushMatrix();
	glTranslatef(ball_pos[0], ball_pos[1], ball_pos[2]);
	glRotatef(ball_rot[0], 1, 0, 0);
	glRotatef(ball_rot[1], 0, 1, 0);
	glRotatef(ball_rot[2], 0, 0, 1);

	glGetFloatv(GL_MODELVIEW_MATRIX, modelview);

	glPopMatrix();
	// please try not to modify the previous block of code

	// you may need to do something here(pass uniform variable(s) to shader and render the model)

	//glmDraw(model,GLM_TEXTURE);// please delete this line in your final code! It's just a preview of rendered object


	GLfloat light0_localpos[3] = {
		light_modelview[0] * light_pos[0]+ light_modelview[4] * light_pos[1] + light_modelview[8] * light_pos[2]+ light_modelview[12],
		light_modelview[1] * light_pos[0] + light_modelview[5] * light_pos[1] + light_modelview[9] * light_pos[2] + light_modelview[13],
		light_modelview[2] * light_pos[0] + light_modelview[6] * light_pos[1] + light_modelview[10] * light_pos[2] + light_modelview[14]
	};
	GLfloat light1_localpos[3] = {
	light_modelview[0] * light1_pos[0] + light_modelview[4] * light1_pos[1] + light_modelview[8] * light1_pos[2] + light_modelview[12],
	light_modelview[1] * light1_pos[0] + light_modelview[5] * light1_pos[1] + light_modelview[9] * light1_pos[2] + light_modelview[13],
	light_modelview[2] * light1_pos[0] + light_modelview[6] * light1_pos[1] + light_modelview[10] * light1_pos[2] + light_modelview[14]
	};

	
	
	glUseProgram(programs[0]);

	//mode
	glUniform1ui(locations.draw_mode, draw_mode );
	//phone
	glUniform1i(locations.light_count, light_count);
	glUniform3fv(locations.light0_pos,1,light0_localpos);
	glUniform3fv(locations.light0_la, 1, light0_ambient);
	glUniform3fv(locations.light0_ld, 1, light0_disffuse);
	glUniform3fv(locations.light0_ls, 1, light0_spec);
	glUniform3fv(locations.light1_pos, 1, light1_localpos);
	glUniform3fv(locations.light1_la, 1, light1_ambient);
	glUniform3fv(locations.light1_ld, 1, light1_disffuse);
	glUniform3fv(locations.light1_ls, 1, light1_spec);
	glUniform3fv(locations.m_ka, 1, material_ra);
	glUniform3fv(locations.m_kd, 1, material_rd);
	glUniform3fv(locations.m_ks, 1, material_rs);
	glUniform1f(locations.m_sh, material_shininess);
	//dissolving
	glUniform3fv(locations.dissolving_color, 1, dissolving_color);
	glUniform1f(locations.dissolving_threshold, dissolving_threshold);


	glActiveTexture(GL_TEXTURE0 + 0); //GL_TEXTUREi = GL_TEXTURE0 + i
	glBindTexture(GL_TEXTURE_2D, mainTextureID);
	glActiveTexture(GL_TEXTURE0 + 1); //GL_TEXTUREi = GL_TEXTURE0 + i
	glBindTexture(GL_TEXTURE_2D, noiseTextureID);
	glActiveTexture(GL_TEXTURE0 + 2); //GL_TEXTUREi = GL_TEXTURE0 + i
	glBindTexture(GL_TEXTURE_2D, rampTextureID);

	glUniform1i( glGetUniformLocation(programs[0], "Tex1") , 0);
	glUniform1i( glGetUniformLocation(programs[0], "Tex2") , 1);
	glUniform1i( glGetUniformLocation(programs[0], "Tex3"),  2);

	glUniformMatrix4fv(locations.MV, 1, GL_FALSE, modelview);
	//precalculate MVP
	
	calcMVP(MVP,modelview,projection);

	glUniformMatrix4fv(locations.MVP, 1, GL_FALSE, MVP);
	
	
	//precalculate normal matrix
	calcNormalMatrix(normalMatrix,modelview);

	glUniformMatrix3fv(locations.NM, 1, GL_FALSE, normalMatrix);

	glDrawArrays(GL_TRIANGLES, 0, coords_count);
	glUseProgram(0);

	

	glutSwapBuffers();
	camera_light_ball_move();
}

//please implement mode toggle(switch mode between phongShading/Dissolving/Ramp) in case 'b'(lowercase)
void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 27:
	{	//ESC
		break;
	}
	case 'b'://toggle mode
	{
		draw_mode = (draw_mode + 1) % draw_mode_count;

		if(draw_mode == 1){
			enable_dissolving_animation = true;
		} else {
			dissolving_threshold = 0;
			enable_dissolving_animation = false;
		}
		if (draw_mode == 2) {
			assign(light0_disffuse, 1, 1, 1);
			light_count = 1;
		}
		break;
	}
	case 96:
	{
		printf("Light 0 Pos: %f,%f,%f\n", light_pos[0], light_pos[1], light_pos[2]);
		printf("Light 1 Pos: %f,%f,%f\n", light1_pos[0], light1_pos[1], light1_pos[2]);
		break;
	}
	case 'd':
	{
		right = true;
		break;
	}
	case 'a':
	{
		left = true;
		break;
	}
	case 'w':
	{
		forward = true;
		break;
	}
	case 's':
	{
		backward = true;
		break;
	}
	case 'q':
	{
		up = true;
		break;
	}
	case 'e':
	{
		down = true;
		break;
	}
	case 't':
	{
		lforward = true;
		break;
	}
	case 'g':
	{
		lbackward = true;
		break;
	}
	case 'h':
	{
		lright = true;
		break;
	}
	case 'f':
	{
		lleft = true;
		break;
	}
	case 'r':
	{
		lup = true;
		break;
	}
	case 'y':
	{
		ldown = true;
		break;
	}
	case '[':
	{
		llforward = true;
		break;
	}
	case '\'':
	{
		llbackward = true;
		break;
	}
	case 13:
	{
		llright = true;
		break;
	}
	case ';':
	{
		llleft = true;
		break;
	}
	case 'p':
	{
		llup = true;
		break;
	}
	case ']':
	{
		lldown = true;
		break;
	}
	case 'i':
	{
		bforward = true;
		break;
	}
	case 'k':
	{
		bbackward = true;
		break;
	}
	case 'l':
	{
		bright = true;
		break;
	}
	case 'j':
	{
		bleft = true;
		break;
	}
	case 'u':
	{
		bup = true;
		break;
	}
	case 'o':
	{
		bdown = true;
		break;
	}
	case '1': // toogle light 2
	{
		light_count = light_count==1 ? 2 : 1;
		break;
	}
	case '2':
	{
		assign(light0_disffuse, float_rand(), float_rand(), float_rand());
		assign(light1_disffuse, float_rand(), float_rand(), float_rand());
		break;
	}
	case '3':
	{
		if(draw_mode == 1)
			enable_dissolving_animation = !enable_dissolving_animation;
		break;
	}
	case '0':
	{
		if (draw_mode == 1)
			assign(dissolving_color, float_rand(), float_rand(), float_rand());
		break;
	}
	case '7':
	{
		bx = true;
		break;
	}
	case '8':
	{
		by = true;
		break;
	}
	case '9':
	{
		bz = true;
		break;
	}
	
	case '4':
	{
		brx = true;
		break;
	}
	case '5':
	{
		bry = true;
		break;
	}
	case '6':
	{
		brz = true;
		break;
	}

	//special function key
	case 'z'://move light source to front of camera
	{
		light_pos[0] = eyex + cos(eyet*M_PI / 180)*cos(eyep*M_PI / 180);
		light_pos[1] = eyey + sin(eyet*M_PI / 180);
		light_pos[2] = eyez - cos(eyet*M_PI / 180)*sin(eyep*M_PI / 180);
		break;
	}
	case 'x'://two light demo
	{
		light_count = 2;
		assign(light0_disffuse, 1, 0.141, 0.019);
		assign(light1_disffuse, 0.019, 1, 1);
		assign(light_pos, 0.920000, 0.700000, 1.180000);
		assign(light1_pos, -1.190000, 0.670000, 1.030000);
		break; 
	}
	case 'c'://reset all pose
	{
		light_count = 1;
		assign(light_pos, 1.1, 1.0, 1.3);
		assign(light1_pos, -1.1, 1.0, 1.3);

		assign(light0_ambient,  0.2, 0.2, 0.2 );
		assign(light0_disffuse,  0.8, 0.8, 0.8 );
		assign(light0_spec,  0.5, 0.5, 0.5);
		assign(light1_ambient, 1.0, 1.0, 0.0);

		ball_rot[0] = 0;
		ball_rot[1] = 0;
		ball_rot[2] = 0;
		eyex = 0.0;
		eyey = 0.0;
		eyez = 5.6;
		eyet = 0;
		eyep = 90;
		break;
	}
	default:
	{
		break;
	}
	}
}

//no need to modify the following functions
void reshape(int width, int height)
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.001f, 100.0f);

	glMatrixMode(GL_MODELVIEW);
}

void motion(int x, int y)
{
	if (mleft)
	{
		eyep -= (x-mousex)*0.1;
		eyet -= (y - mousey)*0.12;
		if (eyet > 89.9)
			eyet = 89.9;
		else if (eyet < -89.9)
			eyet = -89.9;
		if (eyep > 360)
			eyep -= 360;
		else if (eyep < 0)
			eyep += 360;
	}
	mousex = x;
	mousey = y;
}

void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
	{
		if(state == GLUT_DOWN && !mright && !mmiddle)
		{
			mleft = true;
			mousex = x;
			mousey = y;
		}
		else
			mleft = false;
	}
	else if (button == GLUT_RIGHT_BUTTON)
	{
		if (state == GLUT_DOWN && !mleft && !mmiddle)
		{
			mright = true;
			mousex = x;
			mousey = y;
		}
		else
			mright = false;
	}
	else if (button == GLUT_MIDDLE_BUTTON)
	{
		if (state == GLUT_DOWN && !mleft && !mright)
		{
			mmiddle = true;
			mousex = x;
			mousey = y;
		}
		else
			mmiddle = false;
	}
}

void camera_light_ball_move()
{
	GLfloat dx = 0, dy = 0, dz=0;
	if(left|| right || forward || backward || up || down)
	{ 
		if (left)
			dx = -speed;
		else if (right)
			dx = speed;
		if (forward)
			dy = speed;
		else if (backward)
			dy = -speed;
		eyex += dy*cos(eyet*M_PI / 180)*cos(eyep*M_PI / 180) + dx*sin(eyep*M_PI / 180);
		eyey += dy*sin(eyet*M_PI / 180);
		eyez += dy*(-cos(eyet*M_PI / 180)*sin(eyep*M_PI / 180)) + dx*cos(eyep*M_PI / 180);
		if (up)
			eyey += speed;
		else if (down)
			eyey -= speed;
	}
	if(lleft || lright || lforward || lbackward || lup || ldown)
	{
		dx = 0;
		dy = 0;
		if (lleft)
			dx = -speed;
		else if (lright)
			dx = speed;
		if (lforward)
			dy = speed;
		else if (lbackward)
			dy = -speed;
		light_pos[0] += dy*cos(eyet*M_PI / 180)*cos(eyep*M_PI / 180) + dx*sin(eyep*M_PI / 180);
		light_pos[1] += dy*sin(eyet*M_PI / 180);
		light_pos[2] += dy*(-cos(eyet*M_PI / 180)*sin(eyep*M_PI / 180)) + dx*cos(eyep*M_PI / 180);
		if (lup)
			light_pos[1] += speed;
		else if(ldown)
			light_pos[1] -= speed;
	}
	if (llleft || llright || llforward || llbackward || llup || lldown)
	{
		printf("???\n");
		dx = 0;
		dy = 0;
		if (llleft)
			dx = -speed;
		else if (llright)
			dx = speed;
		if (llforward)
			dy = speed;
		else if (llbackward)
			dy = -speed;
		light1_pos[0] += dy * cos(eyet*M_PI / 180)*cos(eyep*M_PI / 180) + dx * sin(eyep*M_PI / 180);
		light1_pos[1] += dy * sin(eyet*M_PI / 180);
		light1_pos[2] += dy * (-cos(eyet*M_PI / 180)*sin(eyep*M_PI / 180)) + dx * cos(eyep*M_PI / 180);
		if (llup)
			light1_pos[1] += speed;
		else if (lldown)
			light1_pos[1] -= speed;
	}
	if (bleft || bright || bforward || bbackward || bup || bdown)
	{
		dx = 0;
		dy = 0;
		if (bleft)
			dx = -speed;
		else if (bright)
			dx = speed;
		if (bforward)
			dy = speed;
		else if (bbackward)
			dy = -speed;
		ball_pos[0] += dy*cos(eyet*M_PI / 180)*cos(eyep*M_PI / 180) + dx*sin(eyep*M_PI / 180);
		ball_pos[1] += dy*sin(eyet*M_PI / 180);
		ball_pos[2] += dy*(-cos(eyet*M_PI / 180)*sin(eyep*M_PI / 180)) + dx*cos(eyep*M_PI / 180);
		if (bup)
			ball_pos[1] += speed;
		else if (bdown)
			ball_pos[1] -= speed;
	}
	if(bx||by||bz || brx || bry || brz)
	{
		dx = 0;
		dy = 0;
		dz = 0;
		if (bx)
			dx = -rotation_speed;
		else if (brx)
			dx = rotation_speed;
		if (by)
			dy = rotation_speed;
		else if (bry)
			dy = -rotation_speed;
		if (bz)
			dz = rotation_speed;
		else if (brz)
			dz = -rotation_speed;
		ball_rot[0] += dx;
		ball_rot[1] += dy;
		ball_rot[2] += dz;
	}
}

void timetick(int)
{
	static const float speed = 0.01F;
	if (dissolving_threshold + speed > 1.0F)
		dissolving_threshold = 0.0F;
	if (enable_dissolving_animation)
		dissolving_threshold += speed;
	glutTimerFunc(30, timetick, 0);
}

inline float float_rand()
{
	return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}

void draw_light_bulb()
{
	if (draw_mode == 1) return;
	GLUquadric *quad;
	quad = gluNewQuadric();
	glPushMatrix();
	glColor3fv(light0_disffuse);
	glTranslatef(light_pos[0], light_pos[1], light_pos[2]);
	gluSphere(quad, light_rad, 40, 20);
	glPopMatrix();
	if (light_count == 2) {
		glPushMatrix();
		glColor3fv(light1_disffuse);
		glTranslatef(light1_pos[0], light1_pos[1], light1_pos[2]);
		gluSphere(quad, light_rad, 40, 20);
		glPopMatrix();
	}
}

void keyboardup(unsigned char key, int x, int y)
{
	switch (key) {
	case 'd':
	{
		right =false;
		break;
	}
	case 'a':
	{
		left = false;
		break;
	}
	case 'w':
	{
		forward = false;
		break;
	}
	case 's':
	{
		backward = false;
		break;
	}
	case 'q':
	{
		up = false;
		break;
	}
	case 'e':
	{
		down = false;
		break;
	}
	case 't':
	{
		lforward = false;
		break;
	}
	case 'g':
	{
		lbackward = false;
		break;
	}
	case 'h':
	{
		lright = false;
		break;
	}
	case 'f':
	{
		lleft = false;
		break;
	}
	case 'r':
	{
		lup = false;
		break;
	}
	case 'y':
	{
		ldown = false;
		break;
	}
	case '[':
	{
		llforward = false;
		break;
	}
	case '\'':
	{
		llbackward = false;
		break;
	}
	case 13:
	{
		llright = false;
		break;
	}
	case ';':
	{
		llleft = false;
		break;
	}
	case 'p':
	{
		llup = false;
		break;
	}
	case ']':
	{
		lldown = false;
		break;
	}
	case 'i':
	{
		bforward = false;
		break;
	}
	case 'k':
	{
		bbackward = false;
		break;
	}
	case 'l':
	{
		bright = false;
		break;
	}
	case 'j':
	{
		bleft = false;
		break;
	}
	case 'u':
	{
		bup = false;
		break;
	}
	case 'o':
	{
		bdown = false;
		break;
	}
	case '7':
	{
		bx = false;
		break;
	}
	case '8':
	{
		by = false;
		break;
	}
	case '9':
	{
		bz = false;
		break;
	}
	case '4':
	{
		brx = false;
		break;
	}
	case '5':
	{
		bry = false;
		break;
	}
	case '6':
	{
		brz = false;
		break;
	}

	default:
	{
		break;
	}
	}
}

void idle(void)
{
	glutPostRedisplay();
}

GLuint loadTexture(char* name, GLfloat width, GLfloat height)
{
	return glmLoadTexture(name, false, true, true, true, &width, &height);
}
