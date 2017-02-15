#include <iostream>
#include <cmath>
#include <fstream>
#include <vector>
#include <map>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp> 
using namespace std;

#include <ao/ao.h>
#include <mpg123.h>
#define BITS 8


mpg123_handle *mh;
unsigned char *buffer;
size_t buffer_size;
size_t done;
int err;

int driver;
ao_device *dev;

ao_sample_format format;
int channels, encoding;
long rate;


void audio_init() {
    /* initializations */
    ao_initialize();
    driver = ao_default_driver_id();
    mpg123_init();
    mh = mpg123_new(NULL, &err);
    buffer_size= 3000;
    buffer = (unsigned char*) malloc(buffer_size * sizeof(unsigned char));

    /* open the file and get the decoding format */
    mpg123_open(mh, "./breakout.mp3");
    mpg123_getformat(mh, &rate, &channels, &encoding);

    /* set the output format and open the output device */
    format.bits = mpg123_encsize(encoding) * BITS;
    format.rate = rate;
    format.channels = channels;
    format.byte_format = AO_FMT_NATIVE;
    format.matrix = 0;
    dev = ao_open_live(driver, &format, NULL);
}


void audio_play() {
    /* decode and play */
    if (mpg123_read(mh, buffer, buffer_size, &done) == MPG123_OK)
        ao_play(dev, (char*) buffer, done);
    else mpg123_seek(mh, 0, SEEK_SET);
}

void audio_close() {
    /* clean up */
    free(buffer);
    ao_close(dev);
    mpg123_close(mh);
    mpg123_delete(mh);
    mpg123_exit();
    ao_shutdown();
}

/* Structures */
typedef struct VAO {
	GLuint VertexArrayID;
	GLuint VertexBuffer;
	GLuint ColorBuffer;

	GLenum PrimitiveMode;
	GLenum FillMode;
	int NumVertices;
}VAO;
typedef struct cubeS {
	float x,y,z;
	float width,height;
	float rotationX,rotationY;
	VAO* object;
}cubeS;
typedef struct COLOR
{
	float r;
	float g;
	float b; 
}Color;
typedef struct star_t {
	int key;
	float X,Y;
	VAO * star;
}stars;
typedef struct Structure {
	string name;
	COLOR color;
	float x,y;
	VAO* object;
	int status;
	float height,width;
	int tone;
}Structure;
typedef struct brick_ts {
	float x1;
	float y1;
	float z1;
	int typebridge;
	VAO * mybrick;
}brick;
struct GLMatrices {
	glm::mat4 projection;
	glm::mat4 projectionP;
	glm::mat4 projectionO;
	glm::mat4 model;
	glm::mat4 view;
	GLuint MatrixID;
} Matrices;
typedef struct cameraI {
    float Ex,Ey,Ez,Tx,Ty,Tz;
}cameraI;
/* Structures */


map <string, Structure> TEXT;

/* Function Definations */
void check_cube_fall();
void values();
void initialise_all();
void level_up();
void rotateslow(GLFWwindow* window);
/* Function Definations */


/*Initialisations */
int x[200][200][200] = {1};
int score=0,i,j=0,count=0,var=1,countstar=2,direction=0,fallen=0,bridgeflag3=0,bridgeflag5=0,viewT=1,proj_type=1;
float w=0.5f,h=0.25f,w2 = 0.5f, h2=0.5f,temp,jump=0.0625f,gotox,gotoy;
float camera_rotation_angle = 90 , block_rotation_angle = 90;
float boardx = -4.5f,boardy = -4.5f;
float block1X = -3.5f , block1Y = -3.5f, block1Z = 0.0f;
float block2X = -3.5f , block2Y = -3.5f, block2Z = 0.0f;
float translateInX = 0.0f, translateInY = 0.0f;
double mouse_pos_x, mouse_pos_y;
float cuboid_lengthX=w,cuboid_lengthY=w,cuboid_lengthZ=2*w,rotationX=0,rotationY=0;
cubeS cube1,cube2;
stars stararr[4];
glm::mat4 translateCubeEdge = glm::mat4(1.0f);
glm::mat4 rotateCube = glm::mat4(1.0f);
glm::mat4 rotationOverallCube = glm::mat4(1.0f);
glm::mat4 translateCube1EdgeR = glm::mat4(1.0f);
brick brickpos[200];
int do_rot, floor_rel;;
GLuint programID;
double last_update_time, current_time;
glm::vec3 rect_pos, floor_pos;
float rectangle_rotation = 0;
cameraI camV;
bool mouse_right_pressed = false;
/*Initialisations */

/* Function to load Shaders - Use it as it is */
GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path) {

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if(VertexShaderStream.is_open())
	{
		std::string Line = "";
		while(getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if(FragmentShaderStream.is_open()){
		std::string Line = "";
		while(getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	//    printf("Compiling shader : %s\n", vertex_file_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> VertexShaderErrorMessage(InfoLogLength);
	glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
	//    fprintf(stdout, "%s\n", &VertexShaderErrorMessage[0]);

	// Compile Fragment Shader
	//    printf("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> FragmentShaderErrorMessage(InfoLogLength);
	glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
	//    fprintf(stdout, "%s\n", &FragmentShaderErrorMessage[0]);

	// Link the program
	//    fprintf(stdout, "Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> ProgramErrorMessage( max(InfoLogLength, int(1)) );
	glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
	//    fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}

static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

void quit(GLFWwindow *window)
{
	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}

void initGLEW(void){
	glewExperimental = GL_TRUE;
	if(glewInit()!=GLEW_OK){
		fprintf(stderr,"Glew failed to initialize : %s\n", glewGetErrorString(glewInit()));
	}
	if(!GLEW_VERSION_3_3)
		fprintf(stderr, "3.3 version not available\n");
}



/* Generate VAO, VBOs and return VAO handle */
struct VAO* create3DObject (GLenum primitive_mode, int numVertices, const GLfloat* vertex_buffer_data, const GLfloat* color_buffer_data, GLenum fill_mode=GL_FILL)
{
	struct VAO* vao = new struct VAO;
	vao->PrimitiveMode = primitive_mode;
	vao->NumVertices = numVertices;
	vao->FillMode = fill_mode;

	// Create Vertex Array Object
	// Should be done after CreateWindow and before any other GL calls
	glGenVertexArrays(1, &(vao->VertexArrayID)); // VAO
	glGenBuffers (1, &(vao->VertexBuffer)); // VBO - vertices
	glGenBuffers (1, &(vao->ColorBuffer));  // VBO - colors

	glBindVertexArray (vao->VertexArrayID); // Bind the VAO 
	glBindBuffer (GL_ARRAY_BUFFER, vao->VertexBuffer); // Bind the VBO vertices 
	glBufferData (GL_ARRAY_BUFFER, 3*numVertices*sizeof(GLfloat), vertex_buffer_data, GL_STATIC_DRAW); // Copy the vertices into VBO
	glVertexAttribPointer(
			0,                  // attribute 0. Vertices
			3,                  // size (x,y,z)
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
			);

	glBindBuffer (GL_ARRAY_BUFFER, vao->ColorBuffer); // Bind the VBO colors 
	glBufferData (GL_ARRAY_BUFFER, 3*numVertices*sizeof(GLfloat), color_buffer_data, GL_STATIC_DRAW);  // Copy the vertex colors
	glVertexAttribPointer(
			1,                  // attribute 1. Color
			3,                  // size (r,g,b)
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
			);

	return vao;
}

/* Generate VAO, VBOs and return VAO handle - Common Color for all vertices */
struct VAO* create3DObject (GLenum primitive_mode, int numVertices, const GLfloat* vertex_buffer_data, const GLfloat red, const GLfloat green, const GLfloat blue, GLenum fill_mode=GL_FILL)
{
	GLfloat* color_buffer_data = new GLfloat [3*numVertices];
	for (int i=0; i<numVertices; i++) {
		color_buffer_data [3*i] = red;
		color_buffer_data [3*i + 1] = green;
		color_buffer_data [3*i + 2] = blue;
	}

	return create3DObject(primitive_mode, numVertices, vertex_buffer_data, color_buffer_data, fill_mode);
}

/* Render the VBOs handled by VAO */
void draw3DObject (struct VAO* vao)
{
	// Change the Fill Mode for this object
	glPolygonMode (GL_FRONT_AND_BACK, vao->FillMode);

	// Bind the VAO to use
	glBindVertexArray (vao->VertexArrayID);

	// Enable Vertex Attribute 0 - 3d Vertices
	glEnableVertexAttribArray(0);
	// Bind the VBO to use
	glBindBuffer(GL_ARRAY_BUFFER, vao->VertexBuffer);

	// Enable Vertex Attribute 1 - Color
	glEnableVertexAttribArray(1);
	// Bind the VBO to use
	glBindBuffer(GL_ARRAY_BUFFER, vao->ColorBuffer);

	// Draw the geometry !
	glDrawArrays(vao->PrimitiveMode, 0, vao->NumVertices); // Starting from vertex 0; 3 vertices total -> 1 triangle
}

/**************************
 * Customizable functions *
 **************************/

float rectangle_rot_dir = 1;
bool rectangle_rot_status = true;

/* Executed when a regular key is pressed/released/held-down */
/* Prefered for Keyboard events */
void keyboard (GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Function is called first on GLFW_PRESS.

	if (action == GLFW_RELEASE) {
		switch (key) {
			case GLFW_KEY_C:
				rectangle_rot_status = !rectangle_rot_status;
				break;
			case GLFW_KEY_P:
				break;
			case GLFW_KEY_X:
				// do something ..keyboardkeyboard
				break;
			default:
				break;
		}
	}
	else if (action == GLFW_PRESS) {
		switch (key) {
			case GLFW_KEY_ESCAPE:
				quit(window);
				break;
			case GLFW_KEY_RIGHT:
				score++;
				direction=1;
				break;
			case GLFW_KEY_LEFT:
				score++;
				direction=2;
				break;
			case GLFW_KEY_UP:
				score++;
				direction=3;
				break;
			case GLFW_KEY_DOWN:
				score++;
				direction=4;
				break;
			case GLFW_KEY_A:
				direction = -1;
				gotox = cube1.x - 1.0f;
				rotateslow(window);
				break;
			case GLFW_KEY_D:
				direction = -2;
				gotox = cube1.x + 1.0f;
				rotateslow(window);
				break;
			case GLFW_KEY_W:
				direction= -3;
				gotoy = cube1.y + 1.0f;
				rotateslow(window);
				break;
			case GLFW_KEY_X:
				direction = -4;
				gotoy = cube1.y - 1.0f;
				rotateslow(window);
				break;
			default:
				break;
		}
	}
}

/* Executed for character input (like in text boxes) */
void keyboardChar (GLFWwindow* window, unsigned int key)
{
	switch (key) {
		case 'b':
		case 'B':
			//block view
			viewT=2;
			break;
		case 'n':
		case 'N':
			//normal view
			viewT=1;
			break;
		case 'o':
		case 'O':
			//top down
			viewT=3;
			break;
		case 'p':
		case 'P':
			//tower
			viewT=4;
			break;
		case 'f':
		case'F':
			//follow cam
			viewT=5;
			break;
		case 'Q':
		case 'q':
			quit(window);
			break;
		case ' ':
			do_rot ^= 1;
			break;
		case 't':
			proj_type ^= 1 ;
			break;
		default:
			break;
	}
}

/* Executed when a mouse button is pressed/released */
void mouseButton (GLFWwindow* window, int button, int action, int mods)
{

	switch (button) 
	{
		case GLFW_MOUSE_BUTTON_RIGHT:
			if (action == GLFW_RELEASE) 
			{
				rectangle_rot_dir *= -1;
			}
			break;
		case GLFW_MOUSE_BUTTON_LEFT:
			{
				if (action == GLFW_RELEASE) 
				{	direction = 5;
					rotateslow(window);
				}
			}
			break;
		default:
			break;

	}
}


/* Executed when window is resized to 'width' and 'height' */
/* Modify the bounds of the screen here in glm::ortho or Field of View in glm::Perspective */
void reshapeWindow (GLFWwindow* window, int width, int height)
{
	int fbwidth=width, fbheight=height;
	glfwGetFramebufferSize(window, &fbwidth, &fbheight);

	GLfloat fov = M_PI/2;

	// sets the viewport of openGL renderer
	glViewport (0, 0, (GLsizei) fbwidth, (GLsizei) fbheight);

	// Store the projection matrix in a variable for future use
	// Perspective projection for 3D views
	Matrices.projectionP = glm::perspective(fov, (GLfloat) fbwidth / (GLfloat) fbheight, 0.1f, 500.0f);

	// Ortho projection for 2D views
	Matrices.projectionO = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 0.0f, 500.0f);
}

VAO *rectangle, *cam, *floor_vao , *myblock;

void createRectangleScore (string name, int tone, COLOR colorA, COLOR colorB, COLOR colorC, COLOR colorD, float x, float y, float height, float width, string component)
{
	// GL3 accepts only Triangles. Quads are not supported
	float w=width/2,h=height/2;
	GLfloat vertex_buffer_data [] = {
		-w,-h,0, // vertex 1
		-w,h,0, // vertex 2
		w,h,0, // vertex 3

		w,h,0, // vertex 3
		w,-h,0, // vertex 4
		-w,-h,0  // vertex 1
	};

	GLfloat color_buffer_data [] = {
		colorA.r,colorA.g,colorA.b, // color 1
		colorB.r,colorB.g,colorB.b, // color 2
		colorC.r,colorC.g,colorC.b, // color 3

		colorC.r,colorC.g,colorC.b, // color 4
		colorD.r,colorD.g,colorD.b, // color 5
		colorA.r,colorA.g,colorA.b // color 6
	};

	// create3DObject creates and returns a handle to a VAO that can be used later
	VAO *rectangle = create3DObject(GL_TRIANGLES, 6, vertex_buffer_data, color_buffer_data, GL_FILL);
	Structure vishsprite = {};
	vishsprite.color = colorA;
	vishsprite.name = name;
	vishsprite.object = rectangle;
	vishsprite.x=x;
	vishsprite.y=y;
	vishsprite.height=height;
	vishsprite.width=width;
	vishsprite.status=1;
	vishsprite.tone=tone;

	if(component=="score")
		TEXT[name] = vishsprite;
}


void setStroke(char val){
	TEXT["top"].status=0;
	TEXT["bottom"].status=0;
	TEXT["middle"].status=0;
	TEXT["left1"].status=0;
	TEXT["left2"].status=0;
	TEXT["right1"].status=0;
	TEXT["right2"].status=0;
	TEXT["middle1"].status=0;
	TEXT["middle2"].status=0;
	if( val=='O' || val=='0' || val=='2' || val=='3' || val=='5' || val=='6'|| val=='7' || val=='8' || val=='9'){
		TEXT["top"].status=1;
		//cout << "entry" << endl;
	}
	if( val=='2' || val=='3' || val=='4' || val=='5' || val=='6' || val=='8' || val=='9'){
		TEXT["middle"].status=1;
	}
	if( val=='0' || val=='2' || val=='3' || val=='5' || val=='6' || val=='8' || val=='9'){
		TEXT["bottom"].status=1;
	}
	if( val=='0' || val=='4' || val=='5' || val=='6' || val=='8' || val=='9' ){
		TEXT["left1"].status=1;
	}
	if(val=='0' || val=='2' || val=='6' || val=='8'){
		TEXT["left2"].status=1;
	}
	if(val=='O' || val=='0' || val=='1' || val=='2' || val=='3' || val=='4' || val=='7' || val=='8' || val=='9'){
		TEXT["right1"].status=1;
	}
	if( val=='0' || val=='1' || val=='3' || val=='4' || val=='5' || val=='6' || val=='7' || val=='8' || val=='9'){
		TEXT["right2"].status=1;
	}

}


// Creates the rectangle object used in this sample code
void createRectangle ()
{
	float wt=w-0.01;
	// GL3 accepts only Triangles. Quads are not supported
	static const GLfloat vertex_buffer_data [] = {
		0.0, wt, 0.0, //1
		0.0, 0.0, 0.0, //2
		wt, 0.0, 0.0,  //3
		0.0, wt, 0.0, //1
		wt, 0.0, 0.0,  //3
		wt, wt, 0.0,  //4
		wt, wt, 0.0,  //4
		wt, 0.0, 0.0,  //3
		wt, 0.0, -1*h,  //5
		wt, wt, 0.0,  //4
		wt, 0.0, -1*h,  //5
		wt, wt, -1.0*h,  //6
		wt, wt, -1.0*h,  //6
		wt, 0.0, -1*h,  //5
		0.0, 0.0, -1*h,  //7
		wt, wt, -1.0*h,  //6
		0.0, 0.0, -1*h,  //7
		0.0, wt, -1*h,  //8
		0.0, wt, -1*h,  //8
		0.0, 0.0, -1*h,  //7
		0.0, 0.0, 0.0, //2
		0.0, wt, -1*h,  //8
		0.0, 0.0, 0.0, //2
		0.0, wt, 0.0, //1
		0.0, wt, -1*h,  //8
		0.0, wt, 0.0, //1
		wt, wt, 0.0,  //4
		0.0, wt, -1*h,  //8
		wt, wt, 0.0,  //4
		wt, wt, -1.0*h,  //6
		0.0, 0.0, 0.0, //2
		0.0, 0.0, -1*h,  //7
		wt, 0.0, -1*h,  //5
		0.0, 0.0, 0.0, //2
		wt, 0.0, -1*h,  //5
		wt, 0.0, 0.0,  //3
	};

	static const GLfloat color_buffer_data [] = {
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,   
		0.0f, 1.0f, 0.0f,   
		0.0f, 1.0f, 0.0f,   
		0.0f, 1.0f, 0.0f,   
		0.0f, 1.0f, 0.0f,   
		0.0f, 1.0f, 0.0f,   
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
	};

	static const GLfloat color_buffer_data2 [] = {
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,   
		0.0f, 1.0f, 0.0f,   
		0.0f, 1.0f, 0.0f,   
		0.0f, 1.0f, 0.0f,   
		0.0f, 1.0f, 0.0f,   
		0.0f, 1.0f, 0.0f,   
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
	};
	static const GLfloat color_buffer_data3 [] = {
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,   
		0.0f, 1.0f, 0.0f,   
		0.0f, 1.0f, 0.0f,   
		0.0f, 1.0f, 0.0f,   
		0.0f, 1.0f, 0.0f,   
		0.0f, 1.0f, 0.0f,   
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
	};
	static const GLfloat color_buffer_data4 [] = {
		1.0f, 69.0f/255.0f, 0.0f,
		1.0f, 69.0f/255.0f, 0.0f,
		1.0f, 69.0f/255.0f, 0.0f,
		1.0f, 69.0f/255.0f, 0.0f,
		1.0f, 69.0f/255.0f, 0.0f,
		1.0f, 69.0f/255.0f, 0.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,   
		0.0f, 1.0f, 0.0f,   
		0.0f, 1.0f, 0.0f,   
		0.0f, 1.0f, 0.0f,   
		0.0f, 1.0f, 0.0f,   
		0.0f, 1.0f, 0.0f,   
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
	};
	// create3DObject creates and returns a handle to a VAO that can be used later
	count=0;
	values();

	//	printf("count:%d\n",count);
	for(i=0;i<=10;i++)
	{
		for(j=0;j<=10;j++)
		{
			//	printf("%d %d %d\n",i,j,x[i][j][var]);
			//printf("var:%d\n",var);
			//		printf("%d %d\n",i,j);
			if(x[i][j][var]==1)
			{
				count++;
				brickpos[count].x1 = boardx + w*(i);
				brickpos[count].y1 = boardy + w*(j);
				brickpos[count].z1 = 0;
				brickpos[count].typebridge = x[i][j][var];
				brickpos[count].mybrick = create3DObject(GL_TRIANGLES, 12*3, vertex_buffer_data, color_buffer_data, GL_FILL);
			}
			if(x[i][j][var]==4 || x[i][j][var]==6)
			{
				count++;
				brickpos[count].x1 = boardx + w*(i);
				brickpos[count].y1 = boardy + w*(j);
				brickpos[count].z1 = 0;
				brickpos[count].typebridge = x[i][j][var];
				brickpos[count].mybrick = create3DObject(GL_TRIANGLES, 12*3, vertex_buffer_data, color_buffer_data4, GL_FILL);
			}
			else if(x[i][j][var]==3)
			{
				count++;
				brickpos[count].x1 = boardx + w*(i);
				brickpos[count].y1 = boardy + w*(j);
				brickpos[count].z1 = 0;
				brickpos[count].typebridge = x[i][j][var];
				brickpos[count].mybrick = create3DObject(GL_TRIANGLES, 12*3, vertex_buffer_data, color_buffer_data2, GL_FILL);
			}
			else if(x[i][j][var]==5)
			{
				count++;
				brickpos[count].x1 = boardx + w*(i);
				brickpos[count].y1 = boardy + w*(j);
				brickpos[count].z1 = 0;
				brickpos[count].typebridge = x[i][j][var];
				brickpos[count].mybrick = create3DObject(GL_TRIANGLES, 12*3, vertex_buffer_data, color_buffer_data3, GL_FILL);
			}
		}
	}
	//printf("count:%d\n",count);
}


VAO* createStar ()
{
	static const GLfloat vertex_buffer_data [] = {
		0,-0.2,0, // vertex 1
		-0.2,0.0,0, // vertex 2
		0, 0.2,0, // vertex 3

		0, 0.2,0, // vertex 3
		0.2, 0.0,0, // vertex 4
		0.,-0.2,0  // vertex 1
	};

	static const GLfloat color_buffer_data [] = {
		255.0f/255.0f, 153.0f/255.0f, 0.0f/255.0f, // color 1
		255.0f/255.0f, 153.0f/255.0f, 0.0f/255.0f, // color 2
		255.0f/255.0f, 153.0f/255.0f, 0.0f/255.0f, // color 4

		255.0f/255.0f, 153.0f/255.0f, 0.0f/255.0f, // color 1
		255.0f/255.0f, 153.0f/255.0f, 0.0f/255.0f, // color 2
		255.0f/255.0f, 153.0f/255.0f, 0.0f/255.0f
	};
	return create3DObject(GL_TRIANGLES, 6, vertex_buffer_data, color_buffer_data, GL_FILL);
}

VAO* createCube (cubeS* curr_cube)
{
	float w=(curr_cube->width),h=(curr_cube->height);
	// GL3 accepts only Triangles. Quads are not supported
	static const GLfloat vertex_buffer_data [] = {
		0.0, w, 0.0, //1
		0.0, 0.0, 0.0, //2
		w, 0.0, 0.0,  //3
		0.0, w, 0.0, //1
		w, 0.0, 0.0,  //3
		w, w, 0.0,  //4

		w, w, 0.0,  //4
		w, 0.0, 0.0,  //3
		w, 0.0, -1*h,  //5
		w, w, 0.0,  //4
		w, 0.0, -1*h,  //5
		w, w, -1.0*h,  //6

		w, w, -1.0*h,  //6
		w, 0.0, -1*h,  //5
		0.0, 0.0, -1*h,  //7
		w, w, -1.0*h,  //6
		0.0, 0.0, -1*h,  //7
		0.0, w, -1*h,  //8

		0.0, w, -1*h,  //8
		0.0, 0.0, -1*h,  //7
		0.0, 0.0, 0.0, //2
		0.0, w, -1*h,  //8
		0.0, 0.0, 0.0, //2
		0.0, w, 0.0, //1

		0.0, w, -1*h,  //8
		0.0, w, 0.0, //1
		w, w, 0.0,  //4
		0.0, w, -1*h,  //8
		w, w, 0.0,  //4
		w, w, -1.0*h,  //6

		0.0, 0.0, 0.0, //2
		0.0, 0.0, -1*h,  //7
		w, 0.0, -1*h,  //5
		0.0, 0.0, 0.0, //2
		w, 0.0, -1*h,  //5
		w, 0.0, 0.0,  //3
	};

	static const GLfloat color_buffer_data [] = {
		1.0f,0,1.0f,
		1.0f,0,1.0f,
		1.0f,0,1.0f,
		1.0f,0,1.0f,
		1.0f,0,1.0f,
		1.0f,0,1.0f,
		139.0f/255.0f,0,139.0f/255.0f,
		139.0f/255.0f,0,139.0f/255.0f,
		139.0f/255.0f,0,139.0f/255.0f,
		139.0f/255.0f,0,139.0f/255.0f,
		139.0f/255.0f,0,139.0f/255.0f,
		139.0f/255.0f,0,139.0f/255.0f,
		1.0f,0,1.0f,
		1.0f,0,1.0f,
		1.0f,0,1.0f,
		1.0f,0,1.0f,
		1.0f,0,1.0f,
		1.0f,0,1.0f,
		139.0f/255.0f,0,139.0f/255.0f,
		139.0f/255.0f,0,139.0f/255.0f,
		139.0f/255.0f,0,139.0f/255.0f,
		139.0f/255.0f,0,139.0f/255.0f,
		139.0f/255.0f,0,139.0f/255.0f,
		139.0f/255.0f,0,139.0f/255.0f,
		75.0f/255.0f,0,130.0f/255.0f,
		75.0f/255.0f,0,130.0f/255.0f,
		75.0f/255.0f,0,130.0f/255.0f,
		75.0f/255.0f,0,130.0f/255.0f,
		75.0f/255.0f,0,130.0f/255.0f,
		75.0f/255.0f,0,130.0f/255.0f,
		75.0f/255.0f,0,130.0f/255.0f,
		75.0f/255.0f,0,130.0f/255.0f,
		75.0f/255.0f,0,130.0f/255.0f,
		75.0f/255.0f,0,130.0f/255.0f,
		75.0f/255.0f,0,130.0f/255.0f,
		75.0f/255.0f,0,130.0f/255.0f,

	};
	// create3DObject creates and returns a handle to a VAO that can be used later
	//create3DObject(GL_TRIANGLES, 12*3, vertex_buffer_data, color_buffer_data, GL_LINE);
	return create3DObject(GL_TRIANGLES, 12*3, vertex_buffer_data, color_buffer_data, GL_FILL);
}

/* Render the scene with openGL */
/* Edit this function according to your assignment */

double mx,my;
void getView(GLFWwindow* window)
{
    //normal view
    if(viewT==1)
    {
        camV.Ex=5*cos(camera_rotation_angle*M_PI/180.0f);
        camV.Ey=0;
        camV.Ez=5*sin(camera_rotation_angle*M_PI/180.0f);
        camV.Tx=0;
        camV.Ty=0;
        camV.Tz=0;
    }
    //block view
    if(viewT==2)
    {
        camV.Ex=cube1.x+cuboid_lengthX;
        camV.Ey=cube1.y+cuboid_lengthY;
        camV.Ez=(cube1.z+cuboid_lengthZ);
        camV.Tx=0;
        camV.Ty=0;
        camV.Tz=0;

    }
    //top down
    if(viewT==3)
    {
        camV.Ex=0;
        camV.Ey=0;
        camV.Ez=5;
        camV.Tx=0;
        camV.Ty=0;
        camV.Tz=0;

    }
    //tower
    if(viewT==4)
    {
        camV.Ex=boardx;
        camV.Ey=boardy;
        camV.Ez=5;
        camV.Tx=0;
        camV.Ty=0;
        camV.Tz=0;

    }
    //follow cam
    if(viewT==5)
    {
        camV.Ex=cube1.x;
        camV.Ey=cube1.y;
        camV.Ez=3;
        camV.Tx=0;
        camV.Ty=0;
        camV.Tz=0;

    }
    //helicopter
    if(viewT==6)
    {
        if(mouse_right_pressed)
        {
            glfwGetCursorPos(window, &mx, &my);
            mx=(mx-120)*0.5/30-3.0f;
            my=(-1*my+481)*0.5/30-3.0f;
        }
        camV.Ex=mx;
        camV.Ey=my;
        //camV.Ez=3;
        camV.Tx=0;
        camV.Ty=0;
        camV.Tz=0;

    }
}

void draw (GLFWwindow* window, float x, float y, float w, float h, int doM, int doV, int doP)
{
	int fbwidth, fbheight;
	glfwGetFramebufferSize(window, &fbwidth, &fbheight);
	glViewport((int)(x*fbwidth), (int)(y*fbheight), (int)(w*fbwidth), (int)(h*fbheight));


	// use the loaded shader program
	// Don't change unless you know what you are doing
	glUseProgram(programID);

	// Eye - Location of camera. Don't change unless you are sure!!

	getView(window);

    // Eye - Location of camera. Don't change unless you are sure!!
    glm::vec3 eye (camV.Ex,camV.Ey,camV.Ez);
    //glm::vec3 eye(1,2,2);
    // Target - Where is the camera looking at.  Don't change unless you are sure!!
    glm::vec3 target (camV.Tx,camV.Ty,camV.Tz);
    // Up - Up vector defines tilt of camera.  Don't change unless you are sure!!
    glm::vec3 up (0, 1, 0);
	
	glm:: vec3 eye2 (1,1,2);

	// Compute Camera matrix (view)
	if(doV)
		Matrices.view = glm::lookAt(eye, target, up); // Fixed camera for 2D (ortho) in XY plane
	else
		Matrices.view = glm::mat4(1.0f);
	glm::mat4 VP = (proj_type?Matrices.projectionP:Matrices.projectionO) * Matrices.view;
	//glm::mat4 VP = Matrices.projectionO * Matrices.view;
		// Compute ViewProject matrix as view/camera might not be changed for this frame (basic scenario)

	/*if (doP)
		VP = Matrices.projection * Matrices.view;
	else
		VP = Matrices.view;
	*/// Send our transformation to the currently bound shader, in the "MVP" uniform
	// For each model you render, since the MVP will be different (at least the M part)
	glm::mat4 MVP;  // MVP = Projection * View * Model

	// Load identity to model matrix
	Matrices.model = glm::mat4(1.0f);

	int j=0;

	for(i=1;i<=count;i++)
	{
		//	printf("i:%d\n",i);
		//printf("%f %f %f\n",brickpos[i].x1,brickpos[i].y1,brickpos[i].z1);
		Matrices.model = glm::mat4(1.0f);
		glm::mat4 translateRectangle = glm::translate(glm::vec3(brickpos[i].x1,brickpos[i].y1,brickpos[i].z1));        // glTranslatef  
		glm::mat4 rotateRectangle = glm::rotate((float)(rectangle_rotation*M_PI/180.0f), glm::vec3(0,0,1));
		Matrices.model *= (translateRectangle * rotateRectangle);
		//if(floor_rel)
		//	Matrices.model = Matrices.model * glm::translate(floor_pos);
		if(doM)
			MVP = VP * Matrices.model;
		else
			MVP = VP;
		glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);

		// draw3DObject draws the VAO given to it using current MVP matrix
		if(brickpos[i].typebridge == 4)
		{
			if(bridgeflag3)
				draw3DObject(brickpos[i].mybrick);
		}
		else if(brickpos[i].typebridge == 6)
		{
			if(bridgeflag5)
				draw3DObject(brickpos[i].mybrick);
		}
		else
			draw3DObject(brickpos[i].mybrick);
	}
	/********* Stars ***********/
	// Load identity to model matrix
	/*Matrices.model = glm::mat4(1.0f);
	  glm::mat4 translateCam = glm::translate(eye);
	  glm::mat4 rotateCam = glm::rotate((float)((90 - camera_rotation_angle)*M_PI/180.0f), glm::vec3(0,1,0));
	  Matrices.model *= (translateCam * rotateCam);
	  MVP = VP * Matrices.model;
	  glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
	// draw3DObject draws the VAO given to it using current MVP matrix
	draw3DObject(cam);*/

	Matrices.model = glm::translate(floor_pos);
	MVP = VP * Matrices.model;
	glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);

	// draw3DObject draws the VAO given to it using current MVP matrix
	//    draw3DObject(floor_vao);
	//cube1
	Matrices.model = glm::mat4(1.0f);

	glm::mat4 translateCube1 = glm::translate(glm::vec3(cube1.x,cube1.y,cube1.z));
	Matrices.model = translateCube1*rotationOverallCube;
	MVP = VP * Matrices.model;
	glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);

	// draw3DObject draws the VAO given to it using current MVP matrix
	draw3DObject(cube1.object);

	//cube2
	glm::mat4 translateonCube1 = glm::translate(glm::vec3(0.0,0.0,cube1.width));
	glm::mat4 translateCube2 = glm::translate(glm::vec3(cube1.x,cube1.y,cube1.z));
	Matrices.model = (translateCube2 * rotationOverallCube * translateonCube1);
	MVP = VP * Matrices.model;
	glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);

	// draw3DObject draws the VAO given to it using current MVP matrix
	draw3DObject(cube2.object);
	/* 7 SEGMENT FOR MOVES*/
	rotateslow(window);

	glm::vec3 eyeF (0,0,5);
    //glm::vec3 eye(1,2,2);
    // Target - Where is the camera looking at.  Don't change unless you are sure!!
    glm::vec3 targetF (0,0,0);
    // Up - Up vector defines tilt of camera.  Don't change unless you are sure!!
    //glm::vec3 up (0, 1, 0);

    // Compute Camera matrix (view)
    if(doV)
	Matrices.view = glm::lookAt(eyeF, targetF, up); // Fixed camera for 2D (ortho) in XY plane
    else
	Matrices.view = glm::mat4(1.0f);

    // Compute ViewProject matrix as view/camera might not be changed for this frame (basic scenario)
    //glm::mat4 VP;
    //VP = (proj_type?Matrices.projectionP:Matrices.projectionO) * Matrices.view;
    VP = Matrices.projectionP * Matrices.view;


	//	printf("done\n");
	/******* Stars ************/
		for(j=0;j<=2;j++)
	{
		Matrices.model = glm::mat4(1.0f);
		glm::mat4 translateStar = glm::translate (glm::vec3(stararr[j].X, stararr[j].Y, 0));        // glTranslatef
		//     glm::mat4 rotateMirror = glm::rotate((float)(mirarr[i].mirror_rotation*M_PI/180.0f), glm::vec3(0,0,1));
		glm::mat4 StarTransform = translateStar ;
		Matrices.model *= (StarTransform);
		MVP = VP * Matrices.model;
		glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
		if(stararr[j].key==0)
			draw3DObject(stararr[j].star);
	}
	int k;
	for(k=1;k<=4;k++)
	{
		float translation;
		float translation1=3.7f;
		if(k==1)
		{
			int temp=score%10;
			setStroke(temp+'0');
			translation=3.4f;
		}
		else if(k==2)           
		{
			int temp=(score/10)%10;
			setStroke(temp+'0');          
			translation=3.0;
		}
		else if(k==3)
		{
			int temp=(score/100)%10;
			setStroke(temp+'0');          
			translation=2.6;
		}
		else if(k==4)
		{
			int temp=(score/1000)%10;
			setStroke(temp+'0');          
			translation=2.2;
		}

		for(map<string,Structure>::iterator it=TEXT.begin();it!=TEXT.end();it++){
			string current = it->first; 
			if(TEXT[current].status==1)
			{
				//    cout << "1" << endl;
				glm::mat4 MVP;  // MVP = Projection * View * Model

				Matrices.model = glm::mat4(1.0f);


				glm::mat4 ObjectTransform;
				glm::mat4 translateObject = glm::translate (glm::vec3(TEXT[current].x+translation,TEXT[current].y+translation1  ,0.0f)); // glTranslatef
				// glm::mat4 rotateTriangle = glm::rotate((float)((TEXT[current].curr_angle)*M_PI/180.0f), glm::vec3(0,0,1));  // rotate about vector (1,0,0)
				ObjectTransform=translateObject;
				Matrices.model *= ObjectTransform;
				MVP = VP * Matrices.model; // MVP = p * V * M

				glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
				draw3DObject(TEXT[current].object);
			}
		}
	}
	/* 7 SEGMENT FOR MOVES*/
	/* 7 SEGMENT FOR LEVEL*/
	for(k=1;k<=1;k++)
	{
		float translation;
		float translation1=2.7f;
		if(k==1)
		{
			int temp=var;
			setStroke(temp+'0');
			translation=-3.7f;
		}

		for(map<string,Structure>::iterator it=TEXT.begin();it!=TEXT.end();it++){
			string current = it->first; 
			if(TEXT[current].status==1)
			{
				//    cout << "1" << endl;
				glm::mat4 MVP;  // MVP = Projection * View * Model

				Matrices.model = glm::mat4(1.0f);


				glm::mat4 ObjectTransform;
				glm::mat4 translateObject = glm::translate (glm::vec3(TEXT[current].x+translation,TEXT[current].y+translation1  ,0.0f)); // glTranslatef
				// glm::mat4 rotateTriangle = glm::rotate((float)((TEXT[current].curr_angle)*M_PI/180.0f), glm::vec3(0,0,1));  // rotate about vector (1,0,0)
				ObjectTransform=translateObject;
				Matrices.model *= ObjectTransform;
				MVP = VP * Matrices.model; // MVP = p * V * M

				glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
				draw3DObject(TEXT[current].object);
			}
		}
	}

	/* 7 SEGMENT FOR LEVEL*/
}

/* Initialise glfw window, I/O callbacks and the renderer to use */
/* Nothing to Edit here */
GLFWwindow* initGLFW (int width, int height){
	GLFWwindow* window; // window desciptor/handle

	glfwSetErrorCallback(error_callback);
	if (!glfwInit()) {
		exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(width, height, "Sample OpenGL 3.3 Application", NULL, NULL);

	if (!window) {
		exit(EXIT_FAILURE);
		glfwTerminate();
	}

	glfwMakeContextCurrent(window);
	//    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
	glfwSwapInterval( 1 );
	glfwSetFramebufferSizeCallback(window, reshapeWindow);
	glfwSetWindowSizeCallback(window, reshapeWindow);
	glfwSetWindowCloseCallback(window, quit);
	glfwSetKeyCallback(window, keyboard);      // general keyboard input
	glfwSetCharCallback(window, keyboardChar);  // simpler specific character handling
	glfwSetMouseButtonCallback(window, mouseButton);  // mouse button clicks

	return window;
}

/* Initialize the OpenGL rendering properties */
/* Add all the models to be created here */
void initGL (GLFWwindow* window, int width, int height)
{
	/* Objects should be created before any other gl function and shaders */
	// Create the models
	createRectangle ();
	for(i=0;i<=2;i++)
	{
		stararr[i].star = createStar();
	}
	stararr[1].X =-3.7f ; stararr[1].Y=3.7f ; stararr[1].key = 0; 
	stararr[2].X =-3.3f ; stararr[2].Y=3.7f ; stararr[2].key = 0; 
	stararr[0].X =-2.9f ; stararr[0].Y=3.7f ; stararr[0].key = 0;
	// createFloor();
	COLOR black = {0/255.0,0/255.0,0/255.0};
	float height1 = 0.02f;
	float width1 = 0.2f;
	createRectangleScore("top",0,black,black,black,black,0,0.2,height1,width1,"score");
	createRectangleScore("bottom",0,black,black,black,black,0,-0.2,height1,width1,"score");
	createRectangleScore("middle",0,black,black,black,black,0,0,height1,width1,"score");
	createRectangleScore("left1",0,black,black,black,black,-0.1,0.1,width1,height1,"score");
	createRectangleScore("left2",0,black,black,black,black,-0.1,-0.1,width1,height1,"score");
	createRectangleScore("right1",0,black,black,black,black,0.1,0.1,width1,height1,"score");
	createRectangleScore("right2",0,black,black,black,black,0.1,-0.1,width1,height1,"score");
	// Create and compile our GLSL program from the shaders
	programID = LoadShaders( "Sample_GL.vert", "Sample_GL.frag" );
	// Get a handle for our "MVP" uniform
	Matrices.MatrixID = glGetUniformLocation(programID, "MVP");

	reshapeWindow (window, width, height);

	// Background color of the scene
	glClearColor (1.0f, 1.0f, 1.0f, 0.0f); // R, G, B, A
	glClearDepth (1.0f);

	glEnable (GL_DEPTH_TEST);
	glDepthFunc (GL_LEQUAL);

	// cout << "VENDOR: " << glGetString(GL_VENDOR) << endl;
	// cout << "RENDERER: " << glGetString(GL_RENDERER) << endl;
	// cout << "VERSION: " << glGetString(GL_VERSION) << endl;
	// cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
}

void rotateslow(GLFWwindow* window)
{
	float temp,translateInX,translateInY;
	//rotate right
	if(fallen)
	{
		cube1.z -= 0.25f;
		if(cube1.z < -5.0f)
		{
			fallen=0;
			initialise_all();
		}
	}
	if(direction==1)
	{
		cube1.rotationX+=10;
		//matrices
		rotateCube = glm::rotate((float)(10*M_PI/180.0f), glm::vec3(0,1,0));
		translateInX=cuboid_lengthX;
		translateCubeEdge = glm::translate(glm::vec3(-1*(translateInX),0.0,cube1.height));
		if(cube1.rotationX==90)
			translateCube1EdgeR = glm::translate(glm::vec3(0.0,0.0,-1*cube1.height));
		else
			translateCube1EdgeR = glm::translate(glm::vec3((translateInX),0.0,-1*cube1.height));
		rotationOverallCube = translateCube1EdgeR * rotateCube * translateCubeEdge * rotationOverallCube;

		if (cube1.rotationX>=90)
		{
			cube1.rotationX=0;
			direction=0;
			cube1.x+=cuboid_lengthX;
			//interchange cuboid z x lenghts
			temp=cuboid_lengthZ;
			cuboid_lengthZ=cuboid_lengthX;
			cuboid_lengthX=temp;
			check_cube_fall();
		}
	}
	//rotate left
	else if(direction==2)
	{
		cube1.rotationX-=10;
		//Matrices
		rotateCube = glm::rotate((float)(-10*M_PI/180.0f), glm::vec3(0,1,0));
		translateInX=cuboid_lengthZ;
		translateCubeEdge = glm::translate(glm::vec3(0.0,0.0,cube1.height));
		if(cube1.rotationX==-90)
			translateCube1EdgeR = glm::translate(glm::vec3(cuboid_lengthZ,0.0,-1*cube1.height));
		else
			translateCube1EdgeR = glm::translate(glm::vec3(0.0,0.0,-1*cube1.height));
		rotationOverallCube = translateCube1EdgeR * rotateCube * translateCubeEdge * rotationOverallCube;

		if(cube1.rotationX<=-90)
		{
			cube1.rotationX=0;
			direction=0;
			//interchange cuboid z x lenghts
			temp=cuboid_lengthZ;
			cuboid_lengthZ=cuboid_lengthX;
			cuboid_lengthX=temp;

			//position cube1
			cube1.x-=cuboid_lengthX;
			//position cube2

			//if(cube1.rotationX)
			check_cube_fall();

		}

	}

	//rotate up
	else if(direction==3)
	{
		cube1.rotationY-=10;
		//matrices
		rotateCube = glm::rotate((float)(-10*M_PI/180.0f), glm::vec3(1,0,0));
		translateInY=cuboid_lengthY;
		translateCubeEdge = glm::translate(glm::vec3(0.0,-1*(translateInY),cube1.height));
		if(cube1.rotationY==-90)
			translateCube1EdgeR = glm::translate(glm::vec3(0.0,0.0,-1*cube1.height));
		else
			translateCube1EdgeR = glm::translate(glm::vec3(0.0,translateInY,-1*cube1.height));
		rotationOverallCube = translateCube1EdgeR * rotateCube * translateCubeEdge * rotationOverallCube;

		if (cube1.rotationY<=-90)
		{
			cube1.rotationY=0;
			direction=0;
			cube1.y+=cuboid_lengthY;

			//interchange cuboid z x lenghts
			temp=cuboid_lengthZ;
			cuboid_lengthZ=cuboid_lengthY;
			cuboid_lengthY=temp;
			check_cube_fall();
		}

	}

	//rotate Down
	else if(direction==4)
	{
		cube1.rotationY+=10;
		//matrices

		rotateCube = glm::rotate((float)(10*M_PI/180.0f), glm::vec3(1,0,0));
		translateInY=cuboid_lengthZ;
		translateCubeEdge = glm::translate(glm::vec3(0.0,0.0,cube1.height));
		if(cube1.rotationY==90)
			translateCube1EdgeR = glm::translate(glm::vec3(0.0,translateInY,-1*cube1.height));
		else
			translateCube1EdgeR = glm::translate(glm::vec3(0.0,0.0,-1*cube1.height));
		rotationOverallCube = translateCube1EdgeR * rotateCube * translateCubeEdge * rotationOverallCube;

		if(cube1.rotationY>=90)
		{
			cube1.rotationY=0;
			direction=0;
			temp=cuboid_lengthZ;
			cuboid_lengthZ=cuboid_lengthY;
			cuboid_lengthY=temp;
			//position cube1
			cube1.y-=cuboid_lengthY;
			check_cube_fall();

		}


	}
	else if(direction==5)
	{
		glfwGetCursorPos(window, &mouse_pos_x, &mouse_pos_y);
		mouse_pos_x = mouse_pos_x*10.0f/600.0f -5.2f;
		mouse_pos_y = -1*(mouse_pos_y*10.0f/600.0f - 4.5f);
		if(fallen)
			direction=0;
		//	printf("mouse:%f %f\n",(mouse_pos_x),(mouse_pos_y));
		int a = mouse_pos_x/0.5;
		int b = mouse_pos_y/0.5;
		float finalx = a*0.5;
		float finaly = b*0.5;
		//		printf("%d %d\n",a,b);
		//		printf("%f %f\n",finalx,finaly);
		if(cube1.x < finalx)
			cube1.x += jump;
		else if(cube1.x > finalx)
			cube1.x -= jump;
		if(cube1.y < finaly)
			cube1.y += jump;
		else if(cube1.y > finaly)
			cube1.y -= jump;
		if(cube1.x == finalx && cube1.y == finaly)
			direction=0;
		
		check_cube_fall();
	}
	else if(direction== -1)
	{
		if(cube1.x > gotox)
			cube1.x -= jump;
		if(cube1.x == gotox)
			direction=0;
		check_cube_fall();
	}
	else if(direction==-2)
	{
		if(cube1.x < gotox)
			cube1.x += jump;
		if(cube1.x == gotox)
			direction=0;
		check_cube_fall();
	}
	else if(direction==-3)
	{
		if(cube1.y < gotoy)
			cube1.y += jump;
		if(cube1.y == gotoy)
			direction=0;
		check_cube_fall();
	}
	else if(direction==-4)
	{
		if(cube1.y > gotoy)
			cube1.y -= jump;
		if(cube1.y == gotoy)
			direction=0;
		check_cube_fall();
	}
}
void check_cube_fall()
{	
	//printf("%f %f\n",cube1.x,cube1.y);
	int xx = (cube1.x-boardx)/w;
	int yy = (cube1.y-boardy)/w;
	//printf("%d %d %d\n",xx,yy,var);
	//printf("%d\n",x[xx][yy][var]);
	if(xx<0 || yy<0)
	{
		stararr[countstar].key=1;
		fallen=1;
		direction=0;
	}
	else
	{
/** HOLE **/
	 //GOAL hole
		if(x[xx][yy][var]==2) // left/below ka part is in hole
		{
			//printf("hi\n");
			if(cuboid_lengthX== w*2) //left part in hole
			{
				if(x[xx+1][yy][var]==2)
					level_up();
			}
			else if(cuboid_lengthY==w*2)
			{
				if(x[xx][yy+1][var]==2)
					level_up();
			}
			else if(cuboid_lengthZ==w*2)
			{
				level_up();
			}
		}
		/** HOLE **/
		/* OUTSIDE BOUNDARY */
		else if(x[xx][yy][var]==0 || x[xx][yy][var]==4 ||  x[xx][yy][var]==6) // left/below ka part is outside
		{
			if(cuboid_lengthX== w*2) //left part is outside
			{
				if(x[xx+1][yy][var]==0 || x[xx+1][yy][var]==4)
				{
					stararr[countstar].key=1;
					countstar--;
					fallen=1;
					direction=0;
				}
			}

			else if(cuboid_lengthY==w*2)
			{
				if(x[xx][yy+1][var]==0 || x[xx][yy+1][var]==4)
				{
					stararr[countstar].key=1;
					countstar--;
					fallen=1;
					direction=0;
				}
			}
			else if(cuboid_lengthZ==w*2)
			{
				{
					stararr[countstar].key=1;
					countstar--;
					fallen=1;
					direction=0;
				}	
			}
		}
		/* OUTSIDE BOUNDARY */
		/* BRIDGE */
		else if(x[xx][yy][var]==5)
		{
			//printf("heya\n");
			if(cuboid_lengthZ == w*2)
			{
				if(bridgeflag5==0)
				{
					bridgeflag5=1;
				}
				else
					bridgeflag5=0;
			}
		}
		else if(x[xx][yy][var]==3) // left/below ka part is on special brick
		{
			if(bridgeflag3==0)
			{
				bridgeflag3=1;
			}
			else
				bridgeflag3=0;
		}
		else if(x[xx][yy][var]==1) // left/below ka part is in hole
		{
			if(cuboid_lengthX== w*2) //left part in hole
			{
				if(x[xx+1][yy][var]==3)
				{
					if(bridgeflag3==0)
					{	
						bridgeflag3=1;
					}
					else
						bridgeflag3=0;
				}
			}
			else if(cuboid_lengthY==w*2)
			{
				if(x[xx][yy+1][var]==3)
				{
					if(bridgeflag3==0)
					{
						bridgeflag3=1;
					}
					else
						bridgeflag3=0;
				}
			}
		}
		/* BRIDGE */
	}
}	

void level_up()
{
	score=0;
	var++;
	//printf("var:%d\n",var);
	stararr[0].key=0,stararr[1].key=0, stararr[2].key=0;
	countstar=2;
	bridgeflag3=0;
	bridgeflag5=0;
	createRectangle();
	initialise_all();
}

void initialise_all()
{
	cube1.x=block1X;
	cube1.y=block1Y;
	cube1.z=0.5f;
	cube1.width=w;
	cube1.height=w;
	cube1.z=block1Z+cube1.height;
	cube1.rotationX=0.0;
	cube1.rotationY=0.0;
	cube1.object=createCube(&cube1);
	direction=0;
	//cube2
	cube2.x=block2X;
	cube2.y=block2Y;
	cube2.z=block2Z+w;
	cube2.width=w;
	cube2.height=w;
	cube2.object=createCube(&cube2);
}

int main (int argc, char** argv)
{
	int width = 600;
	int height = 600;
	rect_pos = glm::vec3(0, 0, 0);
	floor_pos = glm::vec3(0, 0, 0);
	do_rot = 0,floor_rel = 1;

	GLFWwindow* window = initGLFW(width, height);
	initGLEW();
	initGL (window, width, height);
	initialise_all();
	last_update_time = glfwGetTime();
	/* Draw in loop */
	audio_init();
	while (!glfwWindowShouldClose(window)) {

		if(countstar==-1)
		{
			printf("GAME OVER\n");
			break;
		}
		// clear the color and depth in the frame buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// OpenGL Draw commands
		current_time = glfwGetTime();
		if(do_rot)
			camera_rotation_angle += 90*(current_time - last_update_time); // Simulating camera rotation
		if(camera_rotation_angle > 720)
			camera_rotation_angle -= 720;

		last_update_time = current_time;
		audio_play();
		draw(window, 0, 0, 1.0, 1.0, 1, 1, 1);

		// Swap Frame Buffer in double buffering
		glfwSwapBuffers(window);

		// Poll for Keyboard and mouse events
		glfwPollEvents();
	}
	audio_close();
	glfwTerminate();
	//    exit(EXIT_SUCCESS);
}
