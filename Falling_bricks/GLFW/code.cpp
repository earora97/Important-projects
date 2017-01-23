#include <iostream>
#include <cmath>
#include <fstream>
#include <vector>
#include <time.h>
#include <cstdlib>
#include <ctime>
#include <map>
#include <cstring>
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <ao/ao.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace std;

/*  Functions */
int sound_effects();
void shootlaser();
/*  Functions */

/* Initialisations for Sound*/
#define BUF_SIZE 4096
int default_driver;
char *buff;
int buf_size;
int sample;
float freq = 330.0;
int z;
ao_device *device;
ao_sample_format format;
/* Initialisations for Sound*/

/* Structs */

typedef struct COLOR
{
    float r;
    float g;
    float b; 
}Color;

struct VAO {
    GLuint VertexArrayID;
    GLuint VertexBuffer;
    GLuint ColorBuffer;

    GLenum PrimitiveMode;
    GLenum FillMode;
    int NumVertices;
};
typedef struct VAO VAO;

typedef struct Sprite {
    string name;
    COLOR color;
    float x,y;
    VAO* object;
    int status;
    float height,width;
    int tone;
}Sprite;

map <string, Sprite> TEXT;
typedef struct brick_t {	
  int color;
  float X;
  float Y;
  VAO *brick;
  bool hit;
  struct brick_t *next;
}bricks;

typedef struct laser_t {	
  float X,Y,Z;
  VAO *laser;
  float laser_rotation;
  bool hit,reflected;
  struct laser_t *next;
}lasers;

typedef struct state {
  bool rotating;    
  float X,Y,Z;      
  int current_axis; 
}obj_state;

typedef struct mirror_t {
	float X,Y,Z;
	float mirror_rotation;
	VAO * mirror;
}mirrors;

typedef struct star_t {
    int key;
    float X,Y;
    VAO * star;
}stars;

struct GLMatrices {
	glm::mat4 projection;
	glm::mat4 model;
	glm::mat4 view;
	GLuint MatrixID;
} Matrices;

/* Structs */

/* Initialisations */
bricks * head = NULL;
bricks * tail = NULL;
bricks * temp1 = NULL;
bricks * temp2 = NULL;
lasers * head1 = NULL;
lasers * tail1 = NULL;
lasers * node1 = NULL;
lasers * node2 = NULL;
mirrors mirarr[4];
stars stararr[4];
float canon_rotation = 0;
obj_state rec1,rec2,Canon,Bullet;
float speed_up = 0.4;
int score = 0;
float zoom_camera = 1;
float x_change = 0; //For the camera pan
float y_change = 0; //For the camera pan
int mouse_clicked=0;
int right_mouse_clicked=0;
double mouse_x,mouse_y;
double mouse_x_old,mouse_y_old;
float basket_width = 1.0f;
float basket_height = 0.5f;
double mouse_pos_x, mouse_pos_y;
int keypress=0;
float canon_height = 0.3f;
float canon_width = 0.4f;
float limit=4.0f;
int key1,key2,key3,key4;
//int gameover=0;
int mirror_number = 0;
float mirror0_rotation , mirror1_rotation , mirror2_rotation;
int count=0;
/* Initialisations */

GLuint programID;

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
	printf("Compiling shader : %s\n", vertex_file_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> VertexShaderErrorMessage(InfoLogLength);
	glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
	fprintf(stdout, "%s\n", &VertexShaderErrorMessage[0]);

	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> FragmentShaderErrorMessage(InfoLogLength);
	glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
	fprintf(stdout, "%s\n", &FragmentShaderErrorMessage[0]);

	// Link the program
	fprintf(stdout, "Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> ProgramErrorMessage( max(InfoLogLength, int(1)) );
	glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
	fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);

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

void mousescroll(GLFWwindow* window, double xoffset, double yoffset)
{

    if (yoffset==-1) { 
        zoom_camera /= 0.5; //make it bigger than current size
    }
    else if(yoffset==1){
        zoom_camera *= 0.5; //make it bigger than current size
    }
    if (zoom_camera<=1) {
        zoom_camera = 1;
    }
    if (zoom_camera>=4) {
        zoom_camera=4;
    }
    if(x_change-limit/zoom_camera<-4)
        x_change=-4+limit/zoom_camera;
    else if(x_change+limit/zoom_camera>4)
        x_change=4-limit/zoom_camera;
    if(y_change-3.0f/zoom_camera<-3)
        y_change=-3+3.0f/zoom_camera;
    else if(y_change+3.0f/zoom_camera>3)
        y_change=3-3.0f/zoom_camera;
      //  Matrices.projection = glm::ortho(-limit, limit, -limit, limit, 0.1f, 500.0f);
    Matrices.projection = glm::ortho((float)(-limit/zoom_camera+x_change), (float)(limit/zoom_camera+x_change), (float)(-limit/zoom_camera+y_change), (float)(limit/zoom_camera+y_change), 0.1f, 500.0f);
}
void check_pan(){
    if(x_change-limit/zoom_camera<-4)
        x_change=-4+limit/zoom_camera;
    else if(x_change+limit/zoom_camera>4)
        x_change=4-limit/zoom_camera;
    if(y_change-3.0f/zoom_camera<-3)
        y_change=-3+3.0f/zoom_camera;
    else if(y_change+3.0f/zoom_camera>3)
        y_change=3-3.0f/zoom_camera;
}

float triangle_rot_dir = 1;
float rectangle_rot_dir = 1;
bool triangle_rot_status = true;
bool rectangle_rot_status = true;

/* Executed when a regular key is pressed/released/held-down */
/* Prefered for Keyboard events */
void keyboard (GLFWwindow* window, int key, int scancode, int action, int mods)
{
     // Function is called first on GLFW_PRESS.

    if (action == GLFW_RELEASE) {
        switch (key) {
           case (GLFW_KEY_RIGHT_ALT):
            	key1=0;
            	break;
            case (GLFW_KEY_LEFT_ALT):
            	key1=0;
            	break;
            case (GLFW_KEY_RIGHT_CONTROL):
            	key2=0;
            	break;
            case (GLFW_KEY_LEFT_CONTROL):
            	key2=0;
            	break;
            default:
                break;
        }
    }
    else if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        switch (key) {
            case GLFW_KEY_ESCAPE:
                quit(window);
                break;
            case GLFW_KEY_UP:
                mousescroll(window,0,-1);
                check_pan();
                break;
            case GLFW_KEY_DOWN:
                mousescroll(window,0,+1);
                check_pan();
                break;
            case GLFW_KEY_S:
            	if(Canon.Y <= 3.5f)
            		Canon.Y += 0.04f ;
            	break;
            case GLFW_KEY_F:
               	if(Canon.Y > -3.2f)
            		Canon.Y -= 0.04f ;
            	break;
            case GLFW_KEY_A:
            	if(canon_rotation<=55)
            		canon_rotation += 10;
               	break;
            case GLFW_KEY_D:
            	if(canon_rotation>=-55)
            		canon_rotation -= 10;
            	break;
            case GLFW_KEY_N:
            	if(speed_up<=2.0f)
            		speed_up += 0.2f;
            	break;
            case GLFW_KEY_M:
            	if(speed_up>=0.2f)
            		speed_up -= 0.2f;
            	break;
            case (GLFW_KEY_RIGHT_ALT):
            	key1=1;
            	break;
            case (GLFW_KEY_LEFT_ALT):
            	key1=1;
            	break;
			case (GLFW_KEY_RIGHT_CONTROL):
            	key2=1;
            	break;
            case (GLFW_KEY_LEFT_CONTROL):
            	key2=1;
            	break;
            case (GLFW_KEY_LEFT):
                x_change-=2;
                check_pan();
            	if(key1)
            		if(rec2.X > -3.2f)
            			rec2.X -= 0.04f ;
            	if(key2)
					if(rec1.X > -3.2f)
            			rec1.X -= 0.04f;
            	break;
            case (GLFW_KEY_RIGHT):
                x_change+=2;
                check_pan();
            	if(key1)
            		if(rec2.X < 3.2f)
            			rec2.X += 0.04f ;
            	if(key2)
            		if(rec1.X < 3.2f)
            		rec1.X += 0.04f ;
            	break;
            case GLFW_KEY_SPACE:
            {
            	shootlaser();
				//sound_effects();
				//ao_play(device, buff, buf_size);
            	break;
            }
            default:
                break;
        }
    }
}


/* Executed for character input (like in text boxes) */
void keyboardChar (GLFWwindow* window, unsigned int key)
{
	switch (key) {
		case 'Q':
		case 'q':
            quit(window);
            break;
		default:
			break;
	}
}

/* Executed when a mouse button is pressed/released */
void mouseButton (GLFWwindow* window, int button, int action, int mods)
{
    switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT:
            glfwGetCursorPos(window, &mouse_pos_x, &mouse_pos_y);
            if (action == GLFW_PRESS)
                mouse_pos_x = mouse_pos_x*8.0f/700.0f -limit;
                mouse_pos_y = -1*(mouse_pos_y*8.0f/700.0f -limit);
               // printf("rec1y:%lf\n",rec1.Y);
                //printf("mouse:%lf %lf\n",mouse_pos_x,mouse_pos_y);
                if((mouse_pos_x >= rec1.X-(basket_width/2)) && (mouse_pos_x <= rec1.X+(basket_width/2)) && (mouse_pos_y >= rec1.Y-(basket_height/2)) && (mouse_pos_y <= rec1.Y+(basket_height/2)))
                {
                    keypress=1;
                }
                else if((mouse_pos_x >= rec2.X-(basket_width/2)) && (mouse_pos_x <= rec2.X+(basket_width/2)) && (mouse_pos_y >= rec2.Y-(basket_height/2)) && (mouse_pos_y <= rec2.Y+(basket_height/2)))
                {
                    keypress=2;
                }
                else if((mouse_pos_y >= Canon.Y-(canon_height)) && (mouse_pos_y <= Canon.Y+(canon_height)) && (mouse_pos_x >= Canon.X-(canon_width/2)) && (mouse_pos_x <= Canon.X+(canon_width/2)))
                {
                    keypress=3;
                }
                else
                {
                    keypress=4;
                }
            if (action == GLFW_RELEASE)
                keypress=0;
            break;
        case GLFW_MOUSE_BUTTON_RIGHT:
            if (action == GLFW_PRESS)
                key3=1;
            if (action == GLFW_RELEASE)
                key3=0;
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
    /* With Retina display on Mac OS X, GLFW's FramebufferSize
     is different from WindowSize */
    glfwGetFramebufferSize(window, &fbwidth, &fbheight);

	GLfloat fov = 90.0f;

	// sets the viewport of openGL renderer
	glViewport (0, 0, (GLsizei) fbwidth, (GLsizei) fbheight);

	// set the projection matrix as perspective
	/* glMatrixMode (GL_PROJECTION);
	   glLoadIdentity ();
	   gluPerspective (fov, (GLfloat) fbwidth / (GLfloat) fbheight, 0.1, 500.0); */
	// Store the projection matrix in a variable for future use
    // Perspective projection for 3D views
    // Matrices.projection = glm::perspective (fov, (GLfloat) fbwidth / (GLfloat) fbheight, 0.1f, 500.0f);

    // Ortho projection for 2D views
    Matrices.projection = glm::ortho((float)(-limit/zoom_camera+x_change), (float)(limit/zoom_camera+x_change), (float)(-limit/zoom_camera+y_change), (float)(limit/zoom_camera+y_change), 0.1f, 500.0f);
}

VAO *rectangle;
VAO *rectangle2;
VAO *bgrectangle;
VAO *sline;
VAO *border1;
VAO *border2;
VAO *canon;
VAO *canonpart2;

VAO *createMirror (int mirror_number)
{
  static const GLfloat vertex_buffer_data [] = {
    -2.0f,-3.0f,0, // vertex 1
    -1.5f,-2.5f,0, // vertex 2

  };
     static const GLfloat vertex1_buffer_data [] = {
    1.0f,0.3f,0, // vertex 1
    1.0f,-0.3f,0, // vertex 2

  };
   static const GLfloat vertex2_buffer_data [] = {
    1.5f,2.5f,0, // vertex 1
    2.0f,3.0f,0, // vertex 2

  };
    static const GLfloat color_buffer_data [] = {
    0,0,0, // color 1
    0,0,0, // color 2

  };
  if(mirror_number==0)
	return create3DObject(GL_LINES, 2, vertex_buffer_data, color_buffer_data, GL_FILL);
  else if(mirror_number==1)
    return create3DObject(GL_LINES, 2, vertex1_buffer_data, color_buffer_data, GL_FILL);
  else if(mirror_number==2)
    return create3DObject(GL_LINES, 2, vertex2_buffer_data, color_buffer_data, GL_FILL);

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

//float mirror_width = 0.04f;
float mirror_length =0.72111f;

VAO *createBrick (int random_number)
{
	static const GLfloat vertex_buffer_brick [] = {
    -0.11,-0.2,0, // vertex 1
    0.11,-0.2,0, // vertex 2
    0.11, 0.2,0, // vertex 3

    0.11, 0.2,0, // vertex 3
    -0.11, 0.2,0, // vertex 4
    -0.11,-0.2,0  // vertex 1
  };
    static const GLfloat color1_buffer_data [] = {
    1,0,0, // color 1
    1,0,0, // color 2
    1,0,0, // color 3

    0.5,0,0, // color 3
    0.5,0,0, // color 4
    0.5,0,0  // color 1
  };
  static const GLfloat color2_buffer_data [] = {
    0,1,0, // color 1
    0,1,0, // color 2
    0,1,0, // color 3

    0,0.5,0, // color 3
    0,0.5,0, // color 4
    0,0.5,0  // color 1
  };
  static const GLfloat color3_buffer_data [] = {
    0,0,0, // color 1
    0,0,0, // color 2
    0,0,0, // color 3

    0,0,0.3, // color 3
    0,0,0.3, // color 4
    0,0,0.3  // color 1
  };
  	if(random_number == 0)
    	return create3DObject(GL_TRIANGLES, 6, vertex_buffer_brick, color1_buffer_data, GL_FILL);
    else if(random_number == 1)
    	return create3DObject(GL_TRIANGLES, 6, vertex_buffer_brick, color2_buffer_data, GL_FILL);
    else if(random_number == 2)
        return create3DObject(GL_TRIANGLES, 6, vertex_buffer_brick, color3_buffer_data, GL_FILL);
    
}
float brick_height = 0.4f;
float brick_width = 0.22f;

VAO *createLaser ()
{
  static const GLfloat vertex_buffer_data3 [] = {
    -0.16,-0.02,0, // vertex 1
    0.16,-0.02,0, // vertex 2
    0.16, 0.02,0, // vertex 3

    0.16, 0.02,0, // vertex 3
    -0.16, 0.02,0, // vertex 4
    -0.16,-0.02,0  // vertex 1
  };
    static const GLfloat color_buffer_data [] = {
    1,1,1, // color 1
    1,1,1, // color 2
    1,1,1, // color 3

    1,1,1, // color 3
    1,1,1, // color 4
    1,1,1  // color 1
  };
	return create3DObject(GL_TRIANGLES, 6, vertex_buffer_data3, color_buffer_data, GL_FILL);
}

void createRectangle (string name, int tone, COLOR colorA, COLOR colorB, COLOR colorC, COLOR colorD, float x, float y, float height, float width, string component)
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
    Sprite vishsprite = {};
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

float laser_length = 0.32f;
float laser_width = 0.04f;


// Creates the rectangle object used in this sample code
void createBasket ()
{
  // GL3 accepts only Triangles. Quads are not supported
  //for baskets
  static const GLfloat vertex_buffer_data [] = {
    -0.25,-0.25,0, // vertex 1
    0.25,-0.25,0, // vertex 2
    0.5, 0.25,0, // vertex 3

    0.5, 0.25,0, // vertex 3
    -0.5, 0.25,0, // vertex 4
    -0.25,-0.25,0  // vertex 1
  };

static const GLfloat vertex_buffer_data2 [] = {
    -20.0,-1,0, // vertex 1
    -20.0,0.5,0, // vertex 2
    20.0, 0.5,0, // vertex 3

    20.0, 0.5,0, // vertex 3
    20.0, -1.0,0, // vertex 4
    -20.0,-1.0,0  // vertex 1
  };
  static const GLfloat vertex_buffer_data3 [] = {
    -20.0,-0.03,0, // vertex 1
    -20.0,0.03,0, // vertex 2
    20.0, 0.03,0, // vertex 3

    20.0, 0.03,0, // vertex 3
    20.0, -0.03,0, // vertex 4
    -20.0,-0.03,0  // vertex 1
  };
  static const GLfloat vertex_buffer_data4 [] = {
    -0.02,-20.03,0, // vertex 1
    -0.02,20.03,0, // vertex 2
    0.02, 20.03,0, // vertex 3

    0.02, 20.03,0, // vertex 3
    0.02, -20.03,0, // vertex 4
    -0.02,-20.03,0  // vertex 1
  };
  static const GLfloat color_buffer_data [] = {
    1,0,0, // color 1
    1,0,0, // color 2
    0.4,0,0, // color 3

    0.4,0,0, // color 3
    0.4,0,0, // color 4
    1,0,0  // color 1
  };
  static const GLfloat color_buffer_data2 [] = {
    0,0.8,0, // color 1
    0,0.8,0, // color 2
    0,0.4,0, // color 3

    0,0.4,0, // color 3
    0,0.4,0, // color 4
    0,0.8,0  // color 1
  };
  static const GLfloat color_buffer_data3 [] = {
    255.0f/255.0f, 191.0f/255.0f, 0.0f/255.0f, // color 1
    255.0f/255.0f, 191.0f/255.0f, 0.0f/255.0f, // color 2
    255.0f/255.0f, 191.0f/255.0f, 0.0f/255.0f, // color 3

    255.0f/255.0f, 191.0f/255.0f, 0.0f/255.0f, // color 3
    255.0f/255.0f, 191.0f/255.0f, 0.0f/255.0f, // color 4
    255.0f/255.0f, 191.0f/255.0f, 0.0f/255.0f,  // color 1
  };
  static const GLfloat color_buffer_data4 [] = {
    0,0,0, // color 1
    0,0,0, // color 2
    0,0,0, // color 3

    0,0,0, // color 3
    0,0,0, // color 4
    0,0,0  // color 1
  };
  // create3DObject creates and returns a handle to a VAO that can be used later
  rectangle = create3DObject(GL_TRIANGLES, 6, vertex_buffer_data, color_buffer_data, GL_FILL);
  rectangle2 = create3DObject(GL_TRIANGLES, 6, vertex_buffer_data, color_buffer_data2, GL_FILL);
  bgrectangle = create3DObject(GL_TRIANGLES, 6, vertex_buffer_data2, color_buffer_data3, GL_FILL);
  sline = create3DObject(GL_TRIANGLES, 6, vertex_buffer_data3, color_buffer_data4, GL_FILL);
    border1 = create3DObject(GL_TRIANGLES, 6, vertex_buffer_data4, color_buffer_data4, GL_FILL);
      border2 = create3DObject(GL_TRIANGLES, 6, vertex_buffer_data4, color_buffer_data4, GL_FILL);
}


float camera_rotation_angle = 90;


void createCanon ()
{
  static const GLfloat vertex_buffer_data [] = {
    -0.2,-0.15,0, // vertex 1
    0.2,-0.15,0, // vertex 2
    0.2, 0.15,0, // vertex 3

    0.2, 0.15,0, // vertex 3
    -0.2, 0.15,0, // vertex 4
    -0.2,-0.15,0  // vertex 1
  };
  static const GLfloat vertex_buffer_data2 [] = {
    -0.15,-0.1,0, // vertex 1
    0.15,-0.1,0, // vertex 2
    0.15, 0.1,0, // vertex 3

    0.15, 0.1,0, // vertex 3
    -0.15, 0.1,0, // vertex 4
    -0.15,-0.1,0  // vertex 1
  };
    static const GLfloat color_buffer_data [] = {
    1,0,1, // color 1
    1,0,1, // color 2
    1,0,1, // color 4

   1,0,1, // color 1
    1,0,1, // color 2
    1,0,1
  };
	canon = create3DObject(GL_TRIANGLES, 6, vertex_buffer_data, color_buffer_data, GL_FILL);
	canonpart2 = create3DObject(GL_TRIANGLES, 6, vertex_buffer_data2, color_buffer_data, GL_FILL);

}

float canonpart2_width = 0.3f;
float canonpart2_height = 0.2f;


void shootlaser()
{
            lasers * newlaser = (lasers*)(malloc(sizeof(lasers)));
            newlaser->Y = Canon.Y;
            newlaser->X = Canon.X+(canon_width+canonpart2_width+laser_length)/2;
            newlaser->hit = false;
            newlaser->laser_rotation = canon_rotation;
            newlaser->laser = createLaser();
            newlaser->next = NULL;
            newlaser->reflected=false;
            if(head1 == NULL)
                head1 = newlaser;
            if(tail1 == NULL)
                tail1 = newlaser;
            else
            {
                tail1->next = newlaser;
                tail1 = newlaser;
            }
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
    if(val=='A'|| val=='G' || val=='E' || val=='F' || val=='T' || val=='R' || val=='P' || val=='O' || val=='C' || val=='S' || val=='0' || val=='2' || val=='3' || val=='5' || val=='6'|| val=='7' || val=='8' || val=='9'){
        TEXT["top"].status=1;
        //cout << "entry" << endl;
    }
    if(val=='A' || val=='E' || val=='F' || val=='P' || val=='S' || val=='R' || val=='2' || val=='3' || val=='4' || val=='5' || val=='6' || val=='8' || val=='9'){
        TEXT["middle"].status=1;
    }
    if(val=='G' || val=='W' || val=='U' || val=='E' || val=='L' || val=='O' || val=='C' || val=='S' || val=='0' || val=='2' || val=='3' || val=='5' || val=='6' || val=='8' || val=='9'){
        TEXT["bottom"].status=1;
    }
    if(val=='A' || val=='G' || val=='M' || val=='W' || val=='U' || val=='R' || val=='V' || val=='E' || val=='F' || val=='L' || val=='N' || val=='P' || val=='O' || val=='C' || val=='S' || val=='0' || val=='4' || val=='5' || val=='6' || val=='8' || val=='9' ){
        TEXT["left1"].status=1;
    }
    if(val=='A' || val=='G' || val=='M' || val=='W' || val=='U' || val=='E' || val=='R' || val=='F' || val=='L' || val=='N' || val=='P' || val=='O' || val=='C' || val=='0' || val=='2' || val=='6' || val=='8'){
        TEXT["left2"].status=1;
    }
    if(val=='A' || val=='M' || val=='W' || val=='U' || val=='N' || val=='V' || val=='P' || val=='R' || val=='O' || val=='0' || val=='1' || val=='2' || val=='3' || val=='4' || val=='7' || val=='8' || val=='9'){
        TEXT["right1"].status=1;
    }
    if(val=='A' || val=='G' || val=='M' || val=='W' || val=='U' || val=='N' || val=='O' || val=='S' || val=='0' || val=='1' || val=='3' || val=='4' || val=='5' || val=='6' || val=='7' || val=='8' || val=='9'){
        TEXT["right2"].status=1;
    }
    if(val=='T' || val=='I')
    {
       TEXT["middle1"].status=1;
    }
   
}


/* Render the scene with openGL */
/* Edit this function according to your assignment */
void draw (GLFWwindow* window)
{
  // clear the color and depth in the frame buffer
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // use the loaded shader program
  // Don't change unless you know what you are doing
  glUseProgram (programID);

  // Eye - Location of camera. Don't change unless you are sure!!
  glm::vec3 eye ( 5*cos(camera_rotation_angle*M_PI/180.0f), 0, 5*sin(camera_rotation_angle*M_PI/180.0f) );
  // Target - Where is the camera looking at.  Don't change unless you are sure!!
  glm::vec3 target (0, 0, 0);
  // Up - Up vector defines tilt of camera.  Don't change unless you are sure!!
  glm::vec3 up (0, 1, 0);

  // Compute Camera matrix (view)
  // Matrices.view = glm::lookAt( eye, target, up ); // Rotating Camera for 3D
  //  Don't change unless you are sure!!
  Matrices.view = glm::lookAt(glm::vec3(0,0,3), glm::vec3(0,0,0), glm::vec3(0,1,0)); // Fixed camera for 2D (ortho) in XY plane

  // Compute ViewProject matrix as view/camera might not be changed for this frame (basic scenario)
  //  Don't change unless you are sure!!
  Matrices.projection = glm::ortho((float)(-limit/zoom_camera+x_change), (float)(limit/zoom_camera+x_change), (float)(-limit/zoom_camera+y_change), (float)(limit/zoom_camera+y_change), 0.1f, 500.0f);
  glm::mat4 VP = Matrices.projection * Matrices.view;

  // Send our transformation to the currently bound shader, in the "MVP" uniform
  // For each model you render, since the MVP will be different (at least the M part)
  //  Don't change unless you are sure!!
  glm::mat4 MVP;	// MVP = Projection * View * Model

  // Load identity to model matrix
  Matrices.model = glm::mat4(1.0f);

  /* Render your scene */

  //glm::mat4 translateTriangle = glm::translate (glm::vec3(-2.0f, 0.0f, 0.0f)); // glTranslatef
  //Matrices.model *= triangleTransform; 
  //MVP = VP * Matrices.model; // MVP = p * V * M

  //  Don't change unless you are sure!!
  glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
  /************** BLack Lines *************/
   Matrices.model = glm::mat4(1.0f);

  glm::mat4 translatesline = glm::translate (glm::vec3(0.0f, 3.4f, 0));        // glTranslatef
 // glm::mat4 rotateRectangle = glm::rotate((float)(rectangle_rotation*M_PI/180.0f), glm::vec3(0,0,1)); // rotate about vector (-1,1,1)
  Matrices.model *= (translatesline);
  MVP = VP * Matrices.model;
  glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
 
  // draw3DObject draws the VAO given to it using current MVP matrix
  draw3DObject(sline);
  Matrices.model = glm::mat4(1.0f);

  glm::mat4 translateline1 = glm::translate (glm::vec3(3.9745f, 0.0f, 0));        // glTranslatef
 // glm::mat4 rotateRectangle = glm::rotate((float)(rectangle_rotation*M_PI/180.0f), glm::vec3(0,0,1)); // rotate about vector (-1,1,1)
  Matrices.model *= (translateline1);
  MVP = VP * Matrices.model;
  glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
 
  // draw3DObject draws the VAO given to it using current MVP matrix
  draw3DObject(border1);
  Matrices.model = glm::mat4(1.0f);

  glm::mat4 translateline2 = glm::translate (glm::vec3(-3.9745f, 0.0f, 0));        // glTranslatef
 // glm::mat4 rotateRectangle = glm::rotate((float)(rectang5le_rotation*M_PI/180.0f), glm::vec3(0,0,1)); // rotate about vector (-1,1,1)
  Matrices.model *= (translateline2);
  MVP = VP * Matrices.model;
  glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
 
  // draw3DObject draws the VAO given to it using current MVP matrix
  draw3DObject(border2);
  /************** BLack Lines *************/
  // Pop matrix to undo transformations till last push matrix instead of recomputing model matrix
  // glPopMatrix ();

  /********* Left Basket + Yellow Background ************/
  Matrices.model = glm::mat4(1.0f);

  glm::mat4 translateRectangle = glm::translate (glm::vec3(rec1.X, rec1.Y, rec1.Z));        // glTranslatef
 // glm::mat4 rotateRectangle = glm::rotate((float)(rectangle_rotation*M_PI/180.0f), glm::vec3(0,0,1)); // rotate about vector (-1,1,1)
  Matrices.model *= (translateRectangle);
  MVP = VP * Matrices.model;
  glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
 
  // draw3DObject draws the VAO given to it using current MVP matrix
  draw3DObject(bgrectangle);
  draw3DObject(rectangle);
 /********* Left Basket + Yellow Background ************/

  // Increment angles
  float increments = 1;
  Matrices.model = glm::mat4(1.0f);

  glm::mat4 translateRectangle2 = glm::translate (glm::vec3(rec2.X, rec2.Y, rec2.Z));        // glTranslatef
 // glm::mat4 rotateRectangle = glm::rotate((float)(rectangle_rotation*M_PI/180.0f), glm::vec3(0,0,1)); // rotate about vector (-1,1,1)
  Matrices.model *= (translateRectangle2);
  MVP = VP * Matrices.model;
  glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
 
  // draw3DObject draws the VAO given to it using current MVP matrix
  draw3DObject(rectangle2);

  /************** CANON ************/
  
  Matrices.model = glm::mat4(1.0f);
  glm::mat4 translateCanon = glm::translate (glm::vec3(Canon.X, Canon.Y, 0));        // glTranslatef
  glm::mat4 rotateCanon = glm::rotate((float)(canon_rotation*M_PI/180.0f), glm::vec3(0,0,1));
  glm::mat4 canonTransform = translateCanon * rotateCanon;
  Matrices.model *= (canonTransform);
  MVP = VP * Matrices.model;
  glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
  draw3DObject(canon);
  
  /************** CANON ************/

  /************** CANON2 ************/
  
  float canon_end1x = Canon.X + cos(canon_rotation* M_PI/180)*((canon_width+canonpart2_width)/2);
  float canon_end1y = Canon.Y + sin(canon_rotation* M_PI/180)*((canon_width+canonpart2_width)/2);
  Matrices.model = glm::mat4(1.0f);
  glm::mat4 translateCanonOtoGP = glm::translate (glm::vec3((canon_width+canonpart2_width)/2,0.0, 0.0));
  glm::mat4 translateCanonGPtoO = glm::translate (glm::vec3(-1*(canon_width+canonpart2_width)/2,0.0, 0.0));
  glm::mat4 translateCanon2 = glm::translate (glm::vec3(Canon.X+(canon_width+canonpart2_width)/2,Canon.Y, 0));
  glm::mat4 rotateCanon2 = glm::rotate((float)((canon_rotation)*M_PI/180.0f), glm::vec3(0,0,1));  // rotate about vector (1,0,0)
  Matrices.model *= translateCanon2*translateCanonGPtoO*rotateCanon2*translateCanonOtoGP;
  MVP = VP * Matrices.model;
  glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);  
  draw3DObject(canonpart2);

  /************** CANON2 ************/

  /******* Mirrors ************/
    int i=0;
	for(i=0;i<=2;i++)
	{
		Matrices.model = glm::mat4(1.0f);
	  	glm::mat4 translateMirror = glm::translate (glm::vec3(mirarr[i].X, mirarr[i].Y, 0));        // glTranslatef
	 // 	glm::mat4 rotateMirror = glm::rotate((float)(mirarr[i].mirror_rotation*M_PI/180.0f), glm::vec3(0,0,1));
	  	glm::mat4 MirrorTransform = translateMirror ;
	  	Matrices.model *= (MirrorTransform);
	  	MVP = VP * Matrices.model;
	  	glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
	  	draw3DObject(mirarr[i].mirror);
	}

/********* Mirrors ***********/

     /******* Stars ************/
    int j=0;
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

/********* STars ***********/
 
  /***** BRICK *****/
  temp2 = head;
  while(temp2!=NULL)
  {
  	if(temp2->hit==false)
  	{
	  	Matrices.model = glm::mat4(1.0f);
	 	 //glm::mat4 rotateCanon = glm::mat4(1.0f);
	  	glm::mat4 translateBrick = glm::translate (glm::vec3(temp2->X, temp2->Y, 0));        // glTranslatef
	  	//glm::mat4 rotateCanon = glm::rotate((float)(canon_rotation*M_PI/180.0f), glm::vec3(canonZ,canonY,canonX));
	  	glm::mat4 BrickTransform = translateBrick; //* rotateCanon;
	  	Matrices.model *= (BrickTransform);
	  	MVP = VP * Matrices.model;
	  	glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
	 
	  // draw3DObject draws the VAO given to it using current MVP matrix
	  	draw3DObject(temp2->brick);
	  	
	}
	temp2=temp2->next;
  }
    /***** BRICK *****/

  /***** LASER *****/
  node2 = head1;
  while(node2!=NULL)
  {
  	if((node2->hit)==false)
  	{
	  	Matrices.model = glm::mat4(1.0f);
	 	 //glm::mat4 rotateCanon = glm::mat4(1.0f);
	  	glm::mat4 translateLaserOtoGP = glm::translate (glm::vec3((canon_width+canonpart2_width + laser_length)/2,0.0, 0));
	    glm::mat4 translateLaserGPtoO = glm::translate (glm::vec3(-1*((canon_width+canonpart2_width + laser_length)/2),0.0, 0));
	    glm::mat4 translateLaser = glm::translate (glm::vec3(node2->X,node2->Y, 0));
	    glm::mat4 rotateLaser = glm::rotate((float)((node2->laser_rotation)*M_PI/180.0f), glm::vec3(0,0,1));  // rotate about vector (1,0,0)
	    Matrices.model *= translateLaser*translateLaserGPtoO*rotateLaser*translateLaserOtoGP;
	 // 	Matrices.model *= (LaserTransform);
	  	MVP = VP * Matrices.model;
	  	glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
	 
	  // draw3DObject draws the VAO given to it using current MVP matrix
	  	draw3DObject(node2->laser);
	  	
	}
	node2=node2->next;
  }
    /***** LASER *****/
    if(keypress==1)
    {
        glfwGetCursorPos(window, &mouse_pos_x, &mouse_pos_y);
        mouse_pos_x = mouse_pos_x*8.0f/700.0f -limit;
        mouse_pos_y = -1*(mouse_pos_y*8.0f/700.0f -limit);
        if(mouse_pos_x < -3.5f)
            mouse_pos_x = -3.5f;
        if(mouse_pos_x > 3.5f)
            mouse_pos_x = 3.5f;
        rec1.X = mouse_pos_x;
    }
    else if(keypress==2)
    {
        glfwGetCursorPos(window, &mouse_pos_x, &mouse_pos_y);
        mouse_pos_x = mouse_pos_x*8.0f/700.0f -limit;
        mouse_pos_y = -1*(mouse_pos_y*8.0f/700.0f -limit);
        if(mouse_pos_x < -3.5f)
            mouse_pos_x = -3.5f;
        if(mouse_pos_x > 3.5f)
            mouse_pos_x = 3.5f;
        rec2.X = mouse_pos_x;
    }
    else if(keypress==3)
    {
        glfwGetCursorPos(window, &mouse_pos_x, &mouse_pos_y);
        mouse_pos_x = mouse_pos_x*8.0f/700.0f -limit;
        mouse_pos_y = -1*(mouse_pos_y*8.0f/700.0f -limit);
        if(mouse_pos_y > 3.5f)
            mouse_pos_y = 3.5f;
        if(mouse_pos_y < -3.2f)
            mouse_pos_y = -3.2f;
        Canon.Y = mouse_pos_y;
    }
    else if(keypress==4)
    {
        glfwGetCursorPos(window, &mouse_pos_x, &mouse_pos_y);
        mouse_pos_x = mouse_pos_x*8.0f/700.0f -limit;
        mouse_pos_y = -1*(mouse_pos_y*8.0f/700.0f -limit); 
        float theta = (mouse_pos_y-Canon.Y)/(mouse_pos_x-Canon.X);  
        canon_rotation = atan(theta) * 180.0f/M_PI;
       // printf("%f\n",canon_rotation);
    }
    if(key3)
    {
        glfwGetCursorPos(window, &mouse_pos_x, &mouse_pos_y);
        mouse_pos_x = mouse_pos_x*8.0f/700.0f -limit;
        mouse_pos_y = -1*(mouse_pos_y*8.0f/700.0f -limit);
        if(mouse_pos_x < 0)
            x_change-=0.05;
        else
            x_change += 0.05;
        check_pan();
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

        for(map<string,Sprite>::iterator it=TEXT.begin();it!=TEXT.end();it++){
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
}

/* Initialise glfw window, I/O callbacks and the renderer to use */
/* Nothing to Edit here */
GLFWwindow* initGLFW (int width, int height)
{
    GLFWwindow* window; // window desciptor/handle

    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) {
//        exit(EXIT_FAILURE);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, "Sample OpenGL 3.3 Application", NULL, NULL);

    if (!window) {
        glfwTerminate();
//        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    glfwSwapInterval( 1 );

    /* --- register callbacks with GLFW --- */

    /* Register function to handle window resizes */
    /* With Retina display on Mac OS X GLFW's FramebufferSize
     is different from WindowSize */
    glfwSetFramebufferSizeCallback(window, reshapeWindow);
    glfwSetWindowSizeCallback(window, reshapeWindow);

    /* Register function to handle window close */
    glfwSetWindowCloseCallback(window, quit);

    /* Register function to handle keyboard input */
    glfwSetKeyCallback(window, keyboard);      // general keyboard input
    glfwSetCharCallback(window, keyboardChar);  // simpler specific character handling

    /* Register function to handle mouse click */
    glfwSetMouseButtonCallback(window, mouseButton);  // mouse button clicks
    glfwSetScrollCallback(window, mousescroll); // mouse scroll

    return window;
}

/* Initialize the OpenGL rendering properties */
/* Add all the models to be created here */
void initGL (GLFWwindow* window, int width, int height)
{
    /* Objects should be created before any other gl function and shaders */
	// Create the models
//	createTriangle (); // Generate the VAO, VBOs, vertices data & copy into the array buffer
	createBasket ();
	createCanon();
    COLOR black = {0/255.0,0/255.0,0/255.0};
    float height1 = 0.02f;
    float width1 = 0.2f;
    createRectangle("top",0,black,black,black,black,0,0.2,height1,width1,"score");
    createRectangle("bottom",0,black,black,black,black,0,-0.2,height1,width1,"score");
    createRectangle("middle",0,black,black,black,black,0,0,height1,width1,"score");
    createRectangle("left1",0,black,black,black,black,-0.1,0.1,width1,height1,"score");
    createRectangle("left2",0,black,black,black,black,-0.1,-0.1,width1,height1,"score");
    createRectangle("right1",0,black,black,black,black,0.1,0.1,width1,height1,"score");
    createRectangle("right2",0,black,black,black,black,0.1,-0.1,width1,height1,"score");

	int i=0;
	for(i=0;i<=2;i++)
	{
		mirarr[i].mirror = createMirror(i);
        stararr[i].star = createStar();
	}
	// Create and compile our GLSL program from the shaders
	programID = LoadShaders( "Sample_GL.vert", "Sample_GL.frag" );
	// Get a handle for our "MVP" uniform
	Matrices.MatrixID = glGetUniformLocation(programID, "MVP");

	rec1.X = -2; rec1.Y = -3.77; rec1.Z = 0;
	rec2.X =2; rec2.Y = -3.77; rec2.Z = 0;
	Canon.X = -3.82; Canon.Y = 0; Canon.Z = 0;
	Bullet.X = -4;  Bullet.Y = Canon.Y; Bullet.Z = Canon.Z ;
	stararr[1].X =-3.7f ; stararr[1].Y=3.7f ; stararr[1].key = 0.; 
	stararr[2].X =-3.3f ; stararr[2].Y=3.7f ; stararr[2].key = 0; 
	stararr[0].X =-2.9f ; stararr[0].Y=3.7f ; stararr[0].key = 0;
    mirror0_rotation = 45.0f;
    mirror1_rotation = 90.0f;
    mirror2_rotation = 45.0f;
	reshapeWindow (window, width, height);

    // Background color of the scene
	glClearColor (77.0f/255.0f, 255.0f/255.0f, 219.0f/255.0f, 0.0f); // R, G, B, A
	glClearDepth (1.0f);

	glEnable (GL_DEPTH_TEST);
	glDepthFunc (GL_LEQUAL);

    cout << "VENDOR: " << glGetString(GL_VENDOR) << endl;
    cout << "RENDERER: " << glGetString(GL_RENDERER) << endl;
    cout << "VERSION: " << glGetString(GL_VERSION) << endl;
    cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
}

void check_collision(int colorofbrick,int colorkey)
{
	if(colorofbrick == 2)
	{
		score -= 5;
		stararr[count].key=1;
        count++;
	}
    if(colorkey==2)
    {
        if(colorofbrick==0)
            score += 10;
        else
            score -= 1;
    }
    else if(colorkey==3)
    {
        if(colorofbrick==1)
            score += 10;
        else
            score -= 1;
    }
    if(score<0)
        score=0;
    key4=0;
    if(score%100==0)
        speed_up -= 0.05f;
    printf("Score:%d\n",score);

}

void check_collisionbrick(lasers * node4)
{
	int flag=0;
	bricks * nodex = head;
	while(nodex!=NULL)
	{
		/*float brick_end1x = nodex->X - (brick_width/2);
		float brick_end1y = nodex->Y - (brick_height/2);
		float brick_end2x = nodex->X + (brick_width/2);
		float brick_end2y = nodex->Y + (brick_height/2);

		float laser_end1x = node4->X - cos((node4->laser_rotation )* M_PI/180)*(laser_length/2);
		float laser_end1y = node4->Y - sin((node4->laser_rotation )* M_PI/180)*(laser_length/2);
		float laser_end2x = node4->X + cos((node4->laser_rotation )* M_PI/180)*(laser_length/2);
		float laser_end2y = node4->Y + sin((node4->laser_rotation )* M_PI/180)*(laser_length/2);
        float dis = sqrt((nodex->X - node4->X )*(nodex->X - node4->X ) + (nodex->Y - node4->Y )*(nodex->Y - node4->Y ) );
        float dis2 = abs(brick_width + laser_length)/2;
		//if((((brick_end1x)<=laser_end2x) && ((brick_end1x)>=(laser_end1x)) && ((brick_end1y)<=laser_end2y) && ((brick_end1y)+(brick_height)>=laser_end2y))||(((brick_end1x)+(brick_width)>=laser_end2x) && ((brick_end1x)+(brick_width)<=(laser_end1x)) && ((brick_end1y)<=laser_end2y) && ((brick_end1y)+(brick_height)>=(laser_end2y))))
        if(dis <= dis2)
		//if((laser_end1x >= brick_end1x) && (laser_end2x >= brick_end2x) && (brick_end1 <=node4->Y) && (node4->Y <= brick_end2) && (nodex->hit)==false)
		*/
        float dis1x= abs(nodex->X - node4->X);
        float dis1y= abs(nodex->Y - node4->Y);
        float dis2x= abs(brick_width+laser_length)/2;
        float dis2y= abs(brick_height+laser_width)/2;
        if(dis1x < dis2x && dis1y < dis2y)
        {
			(nodex->hit)=true;
			(node4->hit)=true;
            if((nodex->color)==2)
            {
           //     printf("%d\n",count);
                stararr[count].key=1;
                count++;
               // gameover=1;
            }
            else
                score+=20;
            printf("Score:%d\n",score);
			break;
		}
		nodex=nodex->next;
	}
}
void check_collision_mirror(lasers * node5)
{
	float laser_tipx = node5->X + (laser_length*cos((node5->laser_rotation)*M_PI/180))/2;
	float laser_tipy = node5->Y + (laser_length*sin((node5->laser_rotation)*M_PI/180))/2;
	int i=0;
	for(i=0;i<=2;i++)
	{
		/*float angle=mirarr[i].mirror_rotation;
		float mirend1x = mirarr[i].X - (mirror_length*cos((mirarr[i].mirror_rotation)*M_PI/180))/2;
		float mirend1y = mirarr[i].Y - (mirror_length*sin((mirarr[i].mirror_rotation)*M_PI/180))/2;
		float mirend2x = mirarr[i].X + (mirror_length*cos((mirarr[i].mirror_rotation)*M_PI/180))/2;
		float mirend2y = mirarr[i].Y + (mirror_length*sin((mirarr[i].mirror_rotation)*M_PI/180))/2;
		printf("i:  %d\n",i);
        printf("e1:  %f %f\n",mirend1x,mirend1y);
        printf("e2:  %f %f\n",mirend2x,mirend2y);
        printf("laser:  %f %f\n",laser_tipx,laser_tipy);
		if((mirend1x<=laser_tipx) && (laser_tipx <= mirend2x) && (mirend1y<=laser_tipy) && (laser_tipy <= mirend2y))
            if((3*laser_tipy >= (2*laser_tipx - 1.0f)) && (3*laser_tipy <= (2*laser_tipx + 1.0f)))
		      {*/
    		//	printf("Reflect\n");
            if((laser_tipx>=-2.3) && (laser_tipx<=-1.2) && (laser_tipy>=-3.3) && (laser_tipy<=-2.2))
            {
                if((laser_tipy >= (laser_tipx-1.2)) && (laser_tipy <= (laser_tipx-0.8)))
                {
                   // printf("Working\n");
                    node5->X = node5->X + laser_length*cos((node5->laser_rotation)*M_PI/180);
                    node5->Y = node5->Y + laser_length*sin((node5->laser_rotation)*M_PI/180);
                    node5->laser_rotation = (2*(mirror0_rotation)) - (node5->laser_rotation);
                    node5->reflected=true;
                    return;
                }
            }
            if((laser_tipx>=1.2) && (laser_tipx<=2.3) && (laser_tipy>=2.2) && (laser_tipy<=3.3))
            {
                 if((laser_tipy>= (laser_tipx+0.8)) && (laser_tipy <=(laser_tipx+1.2) ))
                {
                   // printf("Working\n");
                    node5->X = node5->X + laser_length*cos((node5->laser_rotation)*M_PI/180);
                    node5->Y = node5->Y + laser_length*sin((node5->laser_rotation)*M_PI/180);
                    node5->laser_rotation = (2*(mirror2_rotation)) - (node5->laser_rotation);
                    node5->reflected=true;
                    return;                    
                }
            }
            if((laser_tipx<=1.3) && (laser_tipx>=0.8) && (laser_tipy>=-0.6) && (laser_tipy<=0.6))
            {
               //     printf("released3\n");
                    node5->laser_rotation = (2*(mirror1_rotation)) - (node5->laser_rotation);
                  //  printf("mirror:%f\n",mirror1_rotation);
                    //printf("%f\n",node5->laser_rotation);
                    node5->X = node5->X + laser_length*cos((node5->laser_rotation)*M_PI/180);
                    node5->Y = node5->Y + laser_length*sin((node5->laser_rotation)*M_PI/180);
                    node5->reflected=true;
                    return;                   
            }
        }
}

int init_sound_effects()
{
	/* -- Initialize -- */
	ao_initialize();
	/* -- Setup for default driver -- */
	default_driver = ao_default_driver_id();

    memset(&format, 0, sizeof(format));
	format.bits = 16;
	format.channels = 2;
	format.rate = 44100 ;
	format.byte_format = AO_FMT_LITTLE;
	/* -- Open driver -- */
	device = ao_open_live(default_driver, &format, NULL /* no options */);
	if (device == NULL) {
		fprintf(stderr, "Error opening device.\n");
		return 1;
	}
	buf_size = format.bits/8 * format.channels * format.rate;
	buff = (char *)calloc(buf_size,
			sizeof(char));
}

int sound_effects()
{
	for (z = 0; z < format.rate; z++) {
		sample = (int)(0.75 * 32768.0 *
			sin(2 * M_PI * freq * ((float) z/format.rate)));

		/* Put the same stuff in left and right channel */
		buff[4*z] = buff[4*z+2] = sample & 0xff;
		buff[4*z+1] = buff[4*z+3] = (sample >> 8) & 0xff;
	}
}

int main (int argc, char** argv)
{
	
	init_sound_effects();
	sound_effects();

	//ao_play(device, buff, buf_size);

	int width = 700;
	int height = 700;
    GLFWwindow* window = initGLFW(width, height);

	initGL (window, width, height);

    double last_update_time = glfwGetTime(), current_time;
    double last_update_time2 = glfwGetTime(), current_time2;

	srand(time(NULL));
	
    /* Draw in loop */
    while (!glfwWindowShouldClose(window)) {
        
        if(count==3)
        {
            printf("GAME OVER! Final Score:%d\n",score);
            break;
        }
        // OpenGL Draw commands
        draw(window);
        // Swap Frame Buffer in double buffering
        glfwSwapBuffers(window);

        // Poll for Keyboard and mouse events
        glfwPollEvents();

        // Control based on time (Time based transformation like 5 degrees rotation every 0.5s)
        current_time = glfwGetTime(); // Time in seconds
        if ((current_time - last_update_time) >= speed_up) 
        {
        	// ** BRICKS ** //
        	bricks * newbrick = (bricks*)(malloc(sizeof(bricks)));
        	newbrick->Y = 3.4f;
        	newbrick->X = std::rand()%7 - 2.8f; 
        	newbrick->color = 0;
        	newbrick->color = std::rand()%3;
        	newbrick->brick = createBrick(newbrick->color);
        	newbrick->hit = false;
        	newbrick->next = NULL;
        	if(head == NULL)
        		head = newbrick;
        	if(tail == NULL)
        		tail = newbrick;
        	else
        	{
        		tail->next = newbrick;
        		tail = newbrick;
        	}
        	bricks * node;
        	if(head->Y <= rec1.Y+0.68)
        	{
        		if(head->X >= rec1.X-0.25 && head->X <= rec1.X+0.25)
        		 {
                        key4=2;
                      check_collision(head->color,key4);
                  }
                if(head->X >= rec2.X-0.25 && head->X <= rec2.X+0.25)
                   {
                        key4=3;
                      check_collision(head->color,key4);
                  }
        		head = head->next;
        	}
        	node = head;
        	while(node!=NULL)
        	{	
        		node->Y -= 0.3;
        		node = node->next;
        	}
        	// ** BRICKS ** //
        	mirrors * newmirror = (mirrors*)(malloc(sizeof(mirrors)));

            last_update_time = current_time;
        }
        if ((current_time - last_update_time2) >= 0.01) 
        {
        	// ** LASER ** //
        	lasers * node3;
        	node3 = head1;
	        while(node3!=NULL)
	        {	
	        	//printf("hoho\n");
	        	(node3->X) += 0.1*cos((node3->laser_rotation)*M_PI/180);
	        	(node3->Y) += 0.1*sin((node3->laser_rotation)*M_PI/180);
	        	if((node3->hit)==false)
	        	   	{
	        	   		check_collisionbrick(node3);
	        	   		if(node3->reflected==false);
	        	   		check_collision_mirror(node3);
	        	   	}
	        	node3 = node3->next;
	        }
        	if(head1!=NULL && head1->X >= limit)
        	{
        		head1=head1->next;
        	}
        	// ** LASER ** //
            last_update_time2 = current_time;
        }
    }
    /** AO Close and shutdown**/
    ao_close(device);
	ao_shutdown();

    glfwTerminate();
    exit(EXIT_SUCCESS);
}