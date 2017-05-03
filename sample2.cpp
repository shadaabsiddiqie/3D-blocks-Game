//ROTATION NOT DONE
#include <iostream>
#include <cmath>
#include <fstream>
#include <vector>
#define GLM_FORCE_RADIANS
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
// #include "mpg123"
using namespace std;
struct VAO {
    GLuint VertexArrayID;
    GLuint VertexBuffer;
    GLuint ColorBuffer;

    GLenum PrimitiveMode;
    GLenum FillMode;
    int NumVertices;
};
typedef struct VAO VAO;

struct GLMatrices {
	glm::mat4 projection;
	glm::mat4 model;
	glm::mat4 view;
	GLuint MatrixID;
} Matrices;
GLuint programID;
GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path) {
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if(VertexShaderStream.is_open())
	{
		std::string Line = "";
		while(getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}
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
	printf("Compiling shader : %s\n", vertex_file_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
	glCompileShader(VertexShaderID);
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> VertexShaderErrorMessage(InfoLogLength);
	glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
	fprintf(stdout, "%s\n", &VertexShaderErrorMessage[0]);
	printf("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(FragmentShaderID);
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> FragmentShaderErrorMessage(InfoLogLength);
	glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
	fprintf(stdout, "%s\n", &FragmentShaderErrorMessage[0]);
	fprintf(stdout, "Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> ProgramErrorMessage( max(InfoLogLength, int(1)) );
	glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
	fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);
	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);
	return ProgramID;
}

static void error_callback(int error, const char* description){
    fprintf(stderr, "Error: %s\n", description);
}

void quit(GLFWwindow *window){
    glfwDestroyWindow(window);
    glfwTerminate();
//    exit(EXIT_SUCCESS);
}
struct VAO* create3DObject (GLenum primitive_mode, int numVertices, const GLfloat* vertex_buffer_data, const GLfloat* color_buffer_data, GLenum fill_mode=GL_FILL){
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
struct VAO* create3DObject (GLenum primitive_mode, int numVertices, const GLfloat* vertex_buffer_data, const GLfloat red, const GLfloat green, const GLfloat blue, GLenum fill_mode=GL_FILL){
    GLfloat* color_buffer_data = new GLfloat [3*numVertices];
    for (int i=0; i<numVertices; i++) {
        color_buffer_data [3*i] = red;
        color_buffer_data [3*i + 1] = green;
        color_buffer_data [3*i + 2] = blue;
    }
    return create3DObject(primitive_mode, numVertices, vertex_buffer_data, color_buffer_data, fill_mode);
}

/* Render the VBOs handled by VAO */
void draw3DObject (struct VAO* vao){
    glPolygonMode (GL_FRONT_AND_BACK, vao->FillMode);
    glBindVertexArray (vao->VertexArrayID);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vao->VertexBuffer);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, vao->ColorBuffer);
    glDrawArrays(vao->PrimitiveMode, 0, vao->NumVertices); // Starting from vertex 0; 3 vertices total -> 1 triangle
}

/**************************
 * Customizable functions *
 **************************/

 struct COLOR {
     float r;
     float g;
     float b;
 };
 typedef struct COLOR COLOR;

 COLOR broungold ={153/255.0,101/255.0,21/255.0};
 COLOR coingold = {255.0/255.0,223.0/255.0,0.0/255.0};
 COLOR steel ={123/255.0,137/255.0,155/255.0};
 COLOR gold = {218.0/255.0,165.0/255.0,32.0/255.0};
 COLOR red = {255.0/255.0,51.0/255.0,51.0/255.0};
 COLOR darkred ={255.0/255.0,0.0/255.0,0.0/255.0};
 COLOR lightgreen = {80/255.0,230/255.0,0/255.0};
 COLOR darkbrown = {46/255.0,46/255.0,31/255.0};
 COLOR lightbrown = {95/255.0,63/255.0,32/255.0};
 COLOR brown1 = {117/255.0,78/255.0,40/255.0};
 COLOR brown2 = {134/255.0,89/255.0,40/255.0};
 COLOR white = {255/255.0,255/255.0,255/255.0};
 COLOR grey = {100.0/255.0,100.0/255.0,100.0/255.0};

 GLfloat digitopbar[] = {
                   0.1,0.35,0,
                   -0.1,0.35,0,
                   -0.1,0.30,0,

                   -0.1,0.30,0,
                   0.1,0.30,0,
                   0.1,0.35,0
                 };
 GLfloat digitmidbar [] = {
                   0.1,0.05,0,
                   -0.1,0.05,0,
                   -0.1,-0.05,0,

                   -0.1,-0.05,0,
                   0.1,-0.05,0,
                   0.1,0.05,0
                 };
 GLfloat digitbotbar [] = {
                 0.1,-0.30,0,
                 -0.1,-0.30,0,
                 -0.1,-0.35,0,

                 -0.1,-0.35,0,
                 0.1,-0.35,0,
                 0.1,-0.30,0
                 };
 GLfloat digitlefttopbar [] = {
                               -0.05,0.30,0,
                               -0.1,0.30,0,
                               -0.1,0.05,0,

                               -0.1,0.05,0,
                               -0.05,0.05,0,
                               -0.05,0.30,0
                             };
 GLfloat digitleftbotbar [] = {
                               -0.05,-0.05,0,
                               -0.1,-0.05,0,
                               -0.1,-0.30,0,

                               -0.1,-0.30,0,
                               -0.05,-0.30,0,
                               -0.05,-0.05,0
                               };
 GLfloat digitrighttopbar [] = {
                               0.1,0.30,0,
                               0.05,0.30,0,
                               0.05,0.05,0,

                               0.05,0.05,0,
                               0.1,0.05,0,
                               0.1,0.30,0
                               };
 GLfloat digitrightbotbar [] = {
                             0.1,-0.05,0,
                             0.05,-0.05,0,
                             0.05,-0.30,0,

                             0.05,-0.30,0,
                             0.1,-0.30,0,
                             0.1,-0.05,0
                             };

 float camera_rotation_angle = 90;
 float cam_eye_x = 6,cam_eye_y = 6,cam_eye_z = 6;
 float cam_target_x=0,cam_target_y=0,cam_target_z=0;
 float mouse_left_clicked_loc_x,mouse_left_clicked_loc_y ;
 int mouse_left_clicked = 0;
 int block1_xi=0,block1_zi=1,block1_yi=0;
 int block2_xi=1,block2_zi=1,block2_yi=0;
 int left_clicked=0;int right_clicked=0;int up_clicked=0;int down_clicked=0;

 int selected_block = 0;//can be 01234 two 1-select1st 2-select2st 3-fall 4-solved
 int rotating_block = 0;
 int view_ortho=1;int view_top; int view_tower ;int view_helicoptor=0;int view_follow_block =0 ; int view_block_view=0;
 int latest_left_clicked=0;int latest_right_clicked=0;int latest_up_clicked=1;int latest_down_clicked=0;
 int perspective_projection = 0;

void keyboard (GLFWwindow* window, int key, int scancode, int action, int mods){
    if (action == GLFW_RELEASE) {
        switch (key) {
            case GLFW_KEY_C:

                break;
            case GLFW_KEY_P:
                break;
            case GLFW_KEY_SPACE:
                if(selected_block==1){
                    selected_block=2;
                }
                else if(selected_block==2){
                    selected_block=1;
                }
                // do something ..
                break;
            case GLFW_KEY_LEFT:

                if(view_ortho||view_top||view_tower){
                    down_clicked=1;
                }
                else if(view_helicoptor){
                    left_clicked=1;
                }
                else if (view_follow_block||view_block_view){
                    if(latest_left_clicked){
                        down_clicked=1;
                        latest_left_clicked=0;
                        latest_right_clicked=0;
                        latest_up_clicked=0;
                        latest_down_clicked=0;
                        latest_down_clicked=1;
                    }
                    else if (latest_right_clicked){
                        up_clicked=1;
                        latest_left_clicked=0;
                        latest_right_clicked=0;
                        latest_up_clicked=0;
                        latest_down_clicked=0;
                        latest_up_clicked=1;
                    }
                    else if (latest_up_clicked){
                        left_clicked=1;
                        latest_left_clicked=0;
                        latest_right_clicked=0;
                        latest_up_clicked=0;
                        latest_down_clicked=0;
                        latest_left_clicked=1;
                    }
                    else if (latest_down_clicked){
                        right_clicked=1;
                        latest_left_clicked=0;
                        latest_right_clicked=0;
                        latest_up_clicked=0;
                        latest_down_clicked=0;
                        latest_right_clicked=1;
                    }
                }
                rotating_block=1;
                break;
            case GLFW_KEY_RIGHT:
                if(view_ortho||view_top||view_tower){
                    up_clicked=1;

                }
                else if(view_helicoptor){
                    right_clicked=1;

                }
                else if (view_follow_block||view_block_view){
                    if(latest_left_clicked){
                        up_clicked=1;
                        latest_left_clicked=0;
                        latest_right_clicked=0;
                        latest_up_clicked=0;
                        latest_down_clicked=0;
                        latest_up_clicked=1;
                    }
                    else if (latest_right_clicked){
                        down_clicked=1;
                        latest_left_clicked=0;
                        latest_right_clicked=0;
                        latest_up_clicked=0;
                        latest_down_clicked=0;
                        latest_down_clicked=1;
                    }
                    else if (latest_up_clicked){
                        right_clicked=1;
                        latest_left_clicked=0;
                        latest_right_clicked=0;
                        latest_up_clicked=0;
                        latest_down_clicked=0;
                        latest_right_clicked=1;
                    }
                    else if (latest_down_clicked){
                        left_clicked=1;
                        latest_left_clicked=0;
                        latest_right_clicked=0;
                        latest_up_clicked=0;
                        latest_down_clicked=0;
                        latest_left_clicked=1;
                    }
                }
                rotating_block=1;
                break;
            case GLFW_KEY_UP:
                if(view_ortho||view_top||view_tower){
                    left_clicked=1;

                }
                else if(view_helicoptor){
                    up_clicked=1;

                }
                else if (view_follow_block||view_block_view){
                    if(latest_left_clicked){
                        left_clicked=1;
                        latest_left_clicked=0;
                        latest_right_clicked=0;
                        latest_up_clicked=0;
                        latest_down_clicked=0;
                        latest_left_clicked=1;
                    }
                    else if (latest_right_clicked){
                        right_clicked=1;
                        latest_left_clicked=0;
                        latest_right_clicked=0;
                        latest_up_clicked=0;
                        latest_down_clicked=0;
                        latest_right_clicked=1;
                    }
                    else if (latest_up_clicked){
                        up_clicked=1;
                        latest_left_clicked=0;
                        latest_right_clicked=0;
                        latest_up_clicked=0;
                        latest_down_clicked=0;
                        latest_up_clicked=1;
                    }
                    else if (latest_down_clicked){
                        down_clicked=1;
                        latest_left_clicked=0;
                        latest_right_clicked=0;
                        latest_up_clicked=0;
                        latest_down_clicked=0;
                        latest_down_clicked=1;
                    }
                }
                rotating_block=1;
                break;
            case GLFW_KEY_DOWN:
                if(view_ortho||view_top||view_tower){
                    right_clicked=1;
                }
                else if(view_helicoptor){
                    down_clicked=1;

                }
                else if (view_follow_block||view_block_view){
                    if(latest_left_clicked){
                        right_clicked=1;
                        latest_left_clicked=0;
                        latest_right_clicked=0;
                        latest_up_clicked=0;
                        latest_down_clicked=0;
                        latest_right_clicked=1;
                    }
                    else if (latest_right_clicked){
                        left_clicked=1;
                        latest_left_clicked=0;
                        latest_right_clicked=0;
                        latest_up_clicked=0;
                        latest_down_clicked=0;
                        latest_left_clicked=1;
                    }
                    else if (latest_up_clicked){
                        down_clicked=1;
                        latest_left_clicked=0;
                        latest_right_clicked=0;
                        latest_up_clicked=0;
                        latest_down_clicked=0;
                        latest_down_clicked=1;
                    }
                    else if (latest_down_clicked){
                        up_clicked=1;
                        latest_left_clicked=0;
                        latest_right_clicked=0;
                        latest_up_clicked=0;
                        latest_down_clicked=0;
                        latest_up_clicked=1;
                    }
                }
                rotating_block=1;
                break;
            case GLFW_KEY_1:
                view_helicoptor=0;
                view_top = 0;
                view_tower =0;
                view_ortho=1;
                view_follow_block = 0;
                view_block_view=0;
                break;
            case GLFW_KEY_2:
                view_top = 1;
                view_tower =0;
                view_follow_block = 0;
                view_helicoptor=0;
                view_ortho=0;
                view_block_view=0;
                break;
            case GLFW_KEY_3:
                view_top = 0;
                view_tower =1;
                view_follow_block = 0;
                view_helicoptor=0;
                view_ortho=0;
                view_block_view=0;
                break;
            case GLFW_KEY_4:
                view_top = 0;
                view_tower =0;
                view_follow_block = 0;
                view_helicoptor=1;
                view_ortho=0;
                view_block_view=0;
                break;
            case GLFW_KEY_5:
                view_top = 0;
                view_tower =0;
                view_follow_block = 1;
                view_helicoptor=0;
                view_ortho=0;
                view_block_view=0;
                break;
            case GLFW_KEY_6:
                view_top = 0;
                view_tower =0;
                view_follow_block = 0;
                view_helicoptor=0;
                view_ortho=0;
                view_block_view=1;

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
            default:
                break;
        }
    }
}

void keyboardChar (GLFWwindow* window, unsigned int key){
	switch (key) {
		case 'Q':
		case 'q':
            quit(window);
            break;
		default:
			break;
	}
}

void mouseScroll(GLFWwindow* window, double xoffset, double yoffset){
    // if(yoffset == 1){
    //     zoom+=0.1;
    //   reshapeWindow(window, 800,800);
    // }
    // if(yoffset == -1){
    //     zoom-=0.1;
    //   reshapeWindow(window, 800,800);
    // }
}

void mouseButton (GLFWwindow* window, int button, int action, int mods){
    switch (button) {

        case GLFW_MOUSE_BUTTON_LEFT:
            if (action == GLFW_RELEASE){
                mouse_left_clicked = 0;
            }
            else if (action == GLFW_PRESS){
                double a,b;
                glfwGetCursorPos(window,&a,&b);
                double cur_x=(a-400)/100 ,cur_y= (b-400)/100;
                mouse_left_clicked_loc_x=cur_x;
                mouse_left_clicked_loc_y=cur_y;
                mouse_left_clicked = 1;
            }
            break;

        case GLFW_MOUSE_BUTTON_RIGHT:
            if (action == GLFW_RELEASE) {

            }
            break;
        default:
            break;
    }
}

void reshapeWindow (GLFWwindow* window, int width, int height){
    int fbwidth=width, fbheight=height;
    glfwGetFramebufferSize(window, &fbwidth, &fbheight);
	GLfloat fov = 90.0f;
	glViewport (0, 0, (GLsizei) fbwidth, (GLsizei) fbheight);
    if(perspective_projection==1){
        Matrices.projection = glm::perspective (fov, (GLfloat) fbwidth / (GLfloat) fbheight, 0.1f, 500.0f);
    }
    else{
        Matrices.projection = glm::ortho(-8.0f, 8.0f, -8.0f, 8.0f, 0.1f, 400.0f);
    }

}

class slabClass{
    public:
        VAO *drawingObj;
        float x,y,z ,fst_x,fst_y,fst_z;
        float rotation_angle;
        int ang_x,ang_y,ang_z;
    public:
        slabClass(float x_coor=0, float y_coor=0,  float z_coor=0,float angle=0,int ang_x_set=0,int ang_y_set=0,int ang_z_set=1,float f_x=0,float f_y=0,float f_z=0){
            x = x_coor;             y = y_coor;                    z = z_coor;
            ang_x=ang_x_set;        ang_y=ang_y_set;               ang_z=ang_z_set;
            fst_x = f_x;            fst_y = f_y;                   fst_z = f_z;
            rotation_angle = angle;
        }
        void loc_trans(float a,float b,float c ,float ang){
            x = x + a;  y = y + b; z = z + c;
            rotation_angle = rotation_angle + ang ;
        }
        void ang_set(float ang ,int x,int y,int z){
            rotation_angle=ang;
            ang_x=x;  ang_y=y; ang_z=z;
        }
        void loc_set(float a,float b,float c){
            x=a;  y=b; z=c;
        }
        void fst_loc_set(float a,float b,float c){
            fst_x=a;  fst_y=b; fst_z=c;
        }
        VAO* createRectangleAll(float w,float h,float b,COLOR col_fb,COLOR col_lr,COLOR col_tb)
        {
            GLfloat vertex_buffer_data [12*9]={
                //front
                0,0,0,
                w,0,0,
                0,h,0,
                0,h,0,
                w,0,0,
                w,h,0,
                //Back
                0,0,-b,
                w,0,-b,
                0,h,-b,
                0,h,-b,
                w,0,-b,
                w,h,-b,
                // left
                0,0,0,
                0,h,0,
                0,0,-b,
                0,h,0,
                0,0,-b,
                0,h,-b,
                // right
                w,0,0,
                w,h,0,
                w,0,-b,
                w,h,0,
                w,0,-b,
                w,h,-b,
                //top
                0,h,0,
                w,h,0,
                0,h,-b,
                w,h,0,
                0,h,-b,
                w,h,-b,
                //bottom
                0,0,0,
                w,0,0,
                0,0,-b,
                w,0,0,
                0,0,-b,
                w,0,-b
            };
            GLfloat color_buffer_data [12*9]={
                col_fb.r,col_fb.g,col_fb.b,
                col_fb.r,col_fb.g,col_fb.b,
                col_fb.r,col_fb.g,col_fb.b,
                col_fb.r,col_fb.g,col_fb.b,
                col_fb.r,col_fb.g,col_fb.b,
                col_fb.r,col_fb.g,col_fb.b,

                col_fb.r,col_fb.g,col_fb.b,
                col_fb.r,col_fb.g,col_fb.b,
                col_fb.r,col_fb.g,col_fb.b,
                col_fb.r,col_fb.g,col_fb.b,
                col_fb.r,col_fb.g,col_fb.b,
                col_fb.r,col_fb.g,col_fb.b,

                col_lr.r,col_lr.g,col_lr.b,
                col_lr.r,col_lr.g,col_lr.b,
                col_lr.r,col_lr.g,col_lr.b,
                col_lr.r,col_lr.g,col_lr.b,
                col_lr.r,col_lr.g,col_lr.b,
                col_lr.r,col_lr.g,col_lr.b,

                col_lr.r,col_lr.g,col_lr.b,
                col_lr.r,col_lr.g,col_lr.b,
                col_lr.r,col_lr.g,col_lr.b,
                col_lr.r,col_lr.g,col_lr.b,
                col_lr.r,col_lr.g,col_lr.b,
                col_lr.r,col_lr.g,col_lr.b,

                col_tb.r,col_tb.g,col_tb.b,
                col_tb.r,col_tb.g,col_tb.b,
                col_tb.r,col_tb.g,col_tb.b,
                col_tb.r,col_tb.g,col_tb.b,
                col_tb.r,col_tb.g,col_tb.b,
                col_tb.r,col_tb.g,col_tb.b,

                col_tb.r,col_tb.g,col_tb.b,
                col_tb.r,col_tb.g,col_tb.b,
                col_tb.r,col_tb.g,col_tb.b,
                col_tb.r,col_tb.g,col_tb.b,
                col_tb.r,col_tb.g,col_tb.b,
                col_tb.r,col_tb.g,col_tb.b,
            };
            drawingObj= create3DObject(GL_TRIANGLES, 12*3 , vertex_buffer_data, color_buffer_data, GL_FILL);
        }
        void draw ()
        {
            glUseProgram (programID);
            glm::vec3 eye ( cam_eye_x, cam_eye_y, cam_eye_z );
            glm::vec3 target (cam_target_x, cam_target_y, cam_target_z);
            glm::vec3 up (0, 1, 0);
            Matrices.view = glm::lookAt( eye, target, up );
            // Matrices.view = glm::lookAt(glm::vec3(0,0,3), glm::vec3(0,0,0), glm::vec3(0,1,0)); // Fixed camera for 2D (ortho) in XY plane
            glm::mat4 VP = Matrices.projection * Matrices.view;
            glm::mat4 MVP;	// MVP = Projection * View * Model

            Matrices.model = glm::mat4(1.0f);
            glm::mat4 trn_Bf_rot = glm::translate (glm::vec3(fst_x,fst_y,fst_z));
            glm::mat4 transl = glm::translate (glm::vec3(x,y,z));
            glm::mat4 rotate = glm::rotate((float)(rotation_angle*M_PI/180.0f), glm::vec3(ang_x,ang_y,ang_z)); // rotate about vector (-1,1,1)
            Matrices.model *= (transl*rotate*trn_Bf_rot);
            MVP = VP * Matrices.model;
            glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
            draw3DObject(drawingObj);
        }
};

GLFWwindow* initGLFW (int width, int height){
    GLFWwindow* window; // window desciptor/handle
    if (!glfwInit()) {
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(width, height, "Sample OpenGL 3.3 Application", NULL, NULL);
    if (!window) {
        glfwTerminate();
    }
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    glfwSwapInterval( 1 );
    glfwSetFramebufferSizeCallback(window, reshapeWindow);
    glfwSetWindowSizeCallback(window, reshapeWindow);
    glfwSetWindowCloseCallback(window, quit);
    glfwSetKeyCallback(window, keyboard);      // general keyboard input
    glfwSetCharCallback(window, keyboardChar);  // simpler specific character handling
    glfwSetMouseButtonCallback(window, mouseButton);  // mouse button clicks
    return window;
}

void initGL (GLFWwindow* window, int width, int height){
	programID = LoadShaders( "Sample_GL.vert", "Sample_GL.frag" );
	Matrices.MatrixID = glGetUniformLocation(programID, "MVP");
	reshapeWindow (window, width, height);
	glClearColor (0.01f, 0.01f, 0.1f, 0.0f); // R, G, B, A
	glClearDepth (1.0f);
	glEnable (GL_DEPTH_TEST);
	glDepthFunc (GL_LEQUAL);
    cout << "VENDOR: " << glGetString(GL_VENDOR) << endl;
    cout << "RENDERER: " << glGetString(GL_RENDERER) << endl;
    cout << "VERSION: " << glGetString(GL_VERSION) << endl;
    cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
}

 //<<<<<<<<<<<<<<<<<<<<<ALL GLOBAL VARIABLES>>>>>>>>>>>>>>>>>>>>>
slabClass slab[3][20][20],block1 ,block2;
int slab_type[3][20][20];//0123456 0-notthere 1-normal 2-bridge_create2 3-loos_slab 4-break_slab 5-answer 6-bridge_create1 7-bridge_itself
COLOR slab_colors[2]={brown1,brown2};
int no_stages=3;
float slab_width = 1,slab_height= 0.2, slab_breath = 1 ;
int TotalScore =0;int TotalScorePre=0;
int Game_Stage=0;
// commen for all stages
int a1[] ={1,1,12,1,12,1,10};
int a2[] = {1,9,5,1,3,1,4};
int a3[] = {1,9,11,1,6,1,7};
int a4[] = {1,9,11,2,12,3,12};
int a5[] = {2,8,2,0,14,1,14};
int a6[] = {2,8,2,1,14,2,14};
int a7[] = {2,8,6,7,4,7,5};
int a8[] = {2,1,8,2,9,2,10};
int len_special = 8;
int *specialData[8] = {
    a1,a2,a3,a4,a5,a6,a7,a8
};
int b1[] = {1,0,1,1,1,1};//101111
int b2[] = {1,0,9,1,1,9};
int b3[] = {7,0,12,7,1,12};
int *Initialblock[3]={
    b1,b2,b3
};
int stage_width[]={6,10,10};
int stage_breath[]={10,14,16};
void numberdraw(int number){
    int num = number;
    float x=0;
    // glm::vec3 eye ( cam_eye_x, cam_eye_y, cam_eye_z );
    // glm::vec3 target (cam_target_x, cam_target_y, cam_target_z);
    // glm::vec3 up (0, 1, 0);
    // Matrices.view = glm::lookAt(glm::vec3(0,0,3), glm::vec3(0,0,0), glm::vec3(0,1,0)); // Fixed camera for 2D (ortho) in XY plane
    glm::mat4 VP = Matrices.projection * Matrices.view;
    glm::mat4 MVP;
    GLfloat color_buffer_data []={
        1,1,1,
        1,1,1,
        1,1,1,
        1,1,1,
        1,1,1,
        1,1,1
    };
    while(num>0)
    {
        switch(num%10)
        {
            case 1:
                Matrices.model = glm::mat4(1.0f);
                Matrices.model *= glm::translate(glm::vec3(5+x,5,-2));
                MVP = VP * Matrices.model;
                glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
                draw3DObject(create3DObject(GL_TRIANGLES, 6 , digitrighttopbar, color_buffer_data, GL_FILL));
                draw3DObject(create3DObject(GL_TRIANGLES, 6 , digitrightbotbar, color_buffer_data, GL_FILL));
                break;
            case 2:
                Matrices.model = glm::mat4(1.0f);
                Matrices.model *= glm::translate(glm::vec3(5+x,5,-2));
                MVP = VP * Matrices.model;
                glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
                draw3DObject(create3DObject(GL_TRIANGLES, 6 , digitopbar, color_buffer_data, GL_FILL));
                draw3DObject(create3DObject(GL_TRIANGLES, 6 , digitbotbar, color_buffer_data, GL_FILL));
                draw3DObject(create3DObject(GL_TRIANGLES, 6 , digitmidbar, color_buffer_data, GL_FILL));
                draw3DObject(create3DObject(GL_TRIANGLES, 6 , digitleftbotbar, color_buffer_data, GL_FILL));
                draw3DObject(create3DObject(GL_TRIANGLES, 6 , digitrighttopbar, color_buffer_data, GL_FILL));
                break;
            case 3:
                Matrices.model = glm::mat4(1.0f);
                Matrices.model *= glm::translate(glm::vec3(5+x,5,-2));
                MVP = VP * Matrices.model;
                glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
                draw3DObject(create3DObject(GL_TRIANGLES, 6 , digitopbar, color_buffer_data, GL_FILL));
                draw3DObject(create3DObject(GL_TRIANGLES, 6 , digitbotbar, color_buffer_data, GL_FILL));
                draw3DObject(create3DObject(GL_TRIANGLES, 6 , digitmidbar, color_buffer_data, GL_FILL));
                draw3DObject(create3DObject(GL_TRIANGLES, 6 , digitrightbotbar, color_buffer_data, GL_FILL));
                draw3DObject(create3DObject(GL_TRIANGLES, 6 , digitrighttopbar, color_buffer_data, GL_FILL));
                break;
            case 4:
                Matrices.model = glm::mat4(1.0f);
                Matrices.model *= glm::translate(glm::vec3(5+x,5,-2));
                MVP = VP * Matrices.model;
                glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
                draw3DObject(create3DObject(GL_TRIANGLES, 6 , digitrighttopbar, color_buffer_data, GL_FILL));
                draw3DObject(create3DObject(GL_TRIANGLES, 6 , digitrightbotbar, color_buffer_data, GL_FILL));
                draw3DObject(create3DObject(GL_TRIANGLES, 6 , digitmidbar, color_buffer_data, GL_FILL));
                draw3DObject(create3DObject(GL_TRIANGLES, 6 , digitlefttopbar, color_buffer_data, GL_FILL));
                break;
            case 5:
                Matrices.model = glm::mat4(1.0f);
                Matrices.model *= glm::translate(glm::vec3(5+x,5,-2));
                MVP = VP * Matrices.model;
                glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
                draw3DObject(create3DObject(GL_TRIANGLES, 6 , digitopbar, color_buffer_data, GL_FILL));
                draw3DObject(create3DObject(GL_TRIANGLES, 6 , digitbotbar, color_buffer_data, GL_FILL));
                draw3DObject(create3DObject(GL_TRIANGLES, 6 , digitmidbar, color_buffer_data, GL_FILL));
                draw3DObject(create3DObject(GL_TRIANGLES, 6 , digitrightbotbar, color_buffer_data, GL_FILL));
                draw3DObject(create3DObject(GL_TRIANGLES, 6 , digitlefttopbar, color_buffer_data, GL_FILL));
                break;
            case 6:
                Matrices.model = glm::mat4(1.0f);
                Matrices.model *= glm::translate(glm::vec3(5+x,5,-2));
                MVP = VP * Matrices.model;
                glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
                draw3DObject(create3DObject(GL_TRIANGLES, 6 , digitopbar, color_buffer_data, GL_FILL));
                draw3DObject(create3DObject(GL_TRIANGLES, 6 , digitbotbar, color_buffer_data, GL_FILL));
                draw3DObject(create3DObject(GL_TRIANGLES, 6 , digitmidbar, color_buffer_data, GL_FILL));
                draw3DObject(create3DObject(GL_TRIANGLES, 6 , digitrightbotbar, color_buffer_data, GL_FILL));
                draw3DObject(create3DObject(GL_TRIANGLES, 6 , digitlefttopbar, color_buffer_data, GL_FILL));
                draw3DObject(create3DObject(GL_TRIANGLES, 6 , digitleftbotbar, color_buffer_data, GL_FILL));
                break;
            case 7:
                Matrices.model = glm::mat4(1.0f);
                Matrices.model *= glm::translate(glm::vec3(5+x,5,-2));
                MVP = VP * Matrices.model;
                glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
                draw3DObject(create3DObject(GL_TRIANGLES, 6 , digitrighttopbar, color_buffer_data, GL_FILL));
                draw3DObject(create3DObject(GL_TRIANGLES, 6 , digitrightbotbar, color_buffer_data, GL_FILL));
                draw3DObject(create3DObject(GL_TRIANGLES, 6 , digitopbar, color_buffer_data, GL_FILL));
                break;
            case 8:
                Matrices.model = glm::mat4(1.0f);
                Matrices.model *= glm::translate(glm::vec3(5+x,5,-2));
                MVP = VP * Matrices.model;
                glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
                draw3DObject(create3DObject(GL_TRIANGLES, 6 , digitopbar, color_buffer_data, GL_FILL));
                draw3DObject(create3DObject(GL_TRIANGLES, 6 , digitbotbar, color_buffer_data, GL_FILL));
                draw3DObject(create3DObject(GL_TRIANGLES, 6 , digitmidbar, color_buffer_data, GL_FILL));
                draw3DObject(create3DObject(GL_TRIANGLES, 6 , digitrightbotbar, color_buffer_data, GL_FILL));
                draw3DObject(create3DObject(GL_TRIANGLES, 6 , digitlefttopbar, color_buffer_data, GL_FILL));
                draw3DObject(create3DObject(GL_TRIANGLES, 6 , digitleftbotbar, color_buffer_data, GL_FILL));
                draw3DObject(create3DObject(GL_TRIANGLES, 6 , digitrighttopbar, color_buffer_data, GL_FILL));
                break;
            case 9:
                Matrices.model = glm::mat4(1.0f);
                Matrices.model *= glm::translate(glm::vec3(5+x,5,-2));
                MVP = VP * Matrices.model;
                glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
                draw3DObject(create3DObject(GL_TRIANGLES, 6 , digitopbar, color_buffer_data, GL_FILL));
                draw3DObject(create3DObject(GL_TRIANGLES, 6 , digitmidbar, color_buffer_data, GL_FILL));
                draw3DObject(create3DObject(GL_TRIANGLES, 6 , digitrightbotbar, color_buffer_data, GL_FILL));
                draw3DObject(create3DObject(GL_TRIANGLES, 6 , digitlefttopbar, color_buffer_data, GL_FILL));
                draw3DObject(create3DObject(GL_TRIANGLES, 6 , digitrighttopbar, color_buffer_data, GL_FILL));
                break;
            case 0:
                Matrices.model = glm::mat4(1.0f);
                Matrices.model *= glm::translate(glm::vec3(5+x,5,-2));
                MVP = VP * Matrices.model;
                glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
                draw3DObject(create3DObject(GL_TRIANGLES, 6 , digitopbar, color_buffer_data, GL_FILL));
                draw3DObject(create3DObject(GL_TRIANGLES, 6 , digitbotbar, color_buffer_data, GL_FILL));
                draw3DObject(create3DObject(GL_TRIANGLES, 6 , digitrightbotbar, color_buffer_data, GL_FILL));
                draw3DObject(create3DObject(GL_TRIANGLES, 6 , digitlefttopbar, color_buffer_data, GL_FILL));
                draw3DObject(create3DObject(GL_TRIANGLES, 6 , digitleftbotbar, color_buffer_data, GL_FILL));
                draw3DObject(create3DObject(GL_TRIANGLES, 6 , digitrighttopbar, color_buffer_data, GL_FILL));
                break;
            default:
                break;
        }
        num = num/10;
        x = x - 0.2;
    }
    std::cout << "1234567890" << '\n';
}
void StageBuild(){
    //stage1
    slab_type[0][0][0]=1,slab_type[0][1][0]=1,slab_type[0][2][0]=1;
    slab_type[0][0][1]=1,slab_type[0][1][1]=1,slab_type[0][2][1]=1,slab_type[0][3][1]=1;
    slab_type[0][0][2]=1,slab_type[0][1][2]=1,slab_type[0][2][2]=1,slab_type[0][3][2]=1;
    slab_type[0][1][3]=1,slab_type[0][2][3]=1,slab_type[0][3][3]=1;
    slab_type[0][1][4]=1,slab_type[0][2][4]=1,slab_type[0][3][4]=1;
    slab_type[0][1][5]=1,slab_type[0][2][5]=1,slab_type[0][3][5]=1,slab_type[0][4][5]=1;
    slab_type[0][2][6]=1,slab_type[0][3][6]=1,slab_type[0][4][6]=1,slab_type[0][5][6]=1;
    slab_type[0][2][7]=1,slab_type[0][3][7]=1,slab_type[0][4][7]=5,slab_type[0][5][7]=1;
    slab_type[0][2][8]=1,slab_type[0][3][8]=1,slab_type[0][4][8]=1,slab_type[0][5][8]=1;
    slab_type[0][3][9]=1,slab_type[0][4][9]=1;
    //stage2
    slab_type[1][0][0]=1,slab_type[1][1][0]=1,slab_type[1][2][0]=1;
    slab_type[1][0][1]=1,slab_type[1][1][1]=5,slab_type[1][2][1]=1;
    slab_type[1][0][2]=1,slab_type[1][1][2]=1,slab_type[1][2][2]=1;
    slab_type[1][1][3]=7;
    slab_type[1][1][4]=7,slab_type[1][8][4]=1,slab_type[1][9][4]=1;
    slab_type[1][1][5]=1,slab_type[1][8][5]=1,slab_type[1][9][5]=6;
    slab_type[1][1][6]=7,slab_type[1][8][6]=1;
    slab_type[1][1][7]=7,slab_type[1][8][7]=1;
    slab_type[1][0][8]=1,slab_type[1][1][8]=1,slab_type[1][2][8]=1,slab_type[1][8][8]=1,slab_type[1][9][8]=1;
    slab_type[1][0][9]=1,slab_type[1][1][9]=1,slab_type[1][2][9]=1,slab_type[1][3][9]=1,slab_type[1][9][9]=1;
    slab_type[1][0][10]=1,slab_type[1][1][10]=1,slab_type[1][2][10]=1,slab_type[1][3][10]=1,slab_type[1][9][10]=1;
    slab_type[1][0][11]=1,slab_type[1][1][11]=1,slab_type[1][2][11]=1,slab_type[1][3][11]=1,slab_type[1][4][11]=1,slab_type[1][7][11]=1,slab_type[1][8][11]=1,slab_type[1][9][11]=2;
    slab_type[1][0][12]=1,slab_type[1][1][12]=4,slab_type[1][2][12]=7,slab_type[1][3][12]=7,slab_type[1][4][12]=1,slab_type[1][5][12]=1,slab_type[1][6][12]=1,slab_type[1][7][12]=1,slab_type[1][8][12]=1,slab_type[1][9][12]=1;
    slab_type[1][0][13]=1,slab_type[1][1][13]=1;
    //stage3
    slab_type[2][7][0]=1,slab_type[2][8][0]=1,slab_type[2][9][0]=1;
    slab_type[2][1][1]=1,slab_type[2][2][1]=1,slab_type[2][3][1]=1,slab_type[2][4][1]=3,slab_type[2][7][1]=1,slab_type[2][8][1]=1,slab_type[2][9][1]=1;
    slab_type[2][1][2]=1,slab_type[2][2][2]=1,slab_type[2][3][2]=1,slab_type[2][4][2]=3,slab_type[2][5][2]=3,slab_type[2][6][2]=1,slab_type[2][7][2]=1,slab_type[2][8][2]=2,slab_type[2][9][2]=1;
    slab_type[2][1][3]=1,slab_type[2][2][3]=1,slab_type[2][3][3]=1,slab_type[2][4][3]=3,slab_type[2][7][3]=1,slab_type[2][8][3]=1,slab_type[2][9][3]=1;
    slab_type[2][2][4]=1,slab_type[2][7][4]=7;
    slab_type[2][2][5]=1,slab_type[2][7][5]=7;
    slab_type[2][2][6]=1,slab_type[2][3][6]=1,slab_type[2][5][6]=1,slab_type[2][6][6]=1,slab_type[2][7][6]=1,slab_type[2][8][6]=2;
    slab_type[2][2][7]=1,slab_type[2][3][7]=1,slab_type[2][4][7]=1,slab_type[2][5][7]=1,slab_type[2][6][7]=1;
    slab_type[2][1][8]=2,slab_type[2][2][8]=1,slab_type[2][3][8]=1,slab_type[2][4][8]=1,slab_type[2][5][8]=1,slab_type[2][6][8]=1,slab_type[2][7][8]=1;
    slab_type[2][2][9]=7,slab_type[2][7][9]=1;
    slab_type[2][2][10]=7,slab_type[2][7][10]=1;
    slab_type[2][0][11]=1,slab_type[2][1][11]=1,slab_type[2][2][11]=1,slab_type[2][5][11]=3,slab_type[2][6][11]=1,slab_type[2][7][11]=1,slab_type[2][8][11]=1;
    slab_type[2][0][12]=1,slab_type[2][1][12]=5,slab_type[2][2][12]=1,slab_type[2][3][12]=1,slab_type[2][4][12]=3,slab_type[2][5][12]=3,slab_type[2][6][12]=1,slab_type[2][7][12]=1,slab_type[2][8][12]=1;
    slab_type[2][0][13]=1,slab_type[2][1][13]=1,slab_type[2][2][13]=1,slab_type[2][5][13]=3,slab_type[2][6][13]=1,slab_type[2][7][13]=1,slab_type[2][8][13]=1;
    slab_type[2][0][14]=7,slab_type[2][1][14]=7,slab_type[2][2][14]=7;
    slab_type[2][1][15]=1;
}
void Initialiseblock(){
    selected_block=0;
    block1_xi=Initialblock[Game_Stage][0]-stage_width[Game_Stage]/2 ;
    block1_yi=Initialblock[Game_Stage][1] ;
    block1_zi =-stage_breath[Game_Stage]/2+Initialblock[Game_Stage][2];
    block2_xi=Initialblock[Game_Stage][3]-stage_width[Game_Stage]/2 ;
    block2_yi=Initialblock[Game_Stage][4] ;
    block2_zi =-stage_breath[Game_Stage]/2+Initialblock[Game_Stage][5];
    for(int r=0;r<no_stages;r++){
        for(int i=0;i<20;i++){
            for(int j=0;j<20;j++){
                slab[r][i][j].loc_set(slab_width*(i-stage_width[Game_Stage]/2),-slab_height,slab_breath*(-j+stage_breath[Game_Stage]/2));
            }
        }
    }
}
void CreateAllObjects(){
    for(int r=0;r<no_stages;r++){
        for(int i=0;i<20;i++){
            for(int j=0;j<20;j++){
                COLOR selCol;
                if(slab_type[r][i][j]==2){//bridgecreate2
                    selCol = coingold;
                }
                else if(slab_type[r][i][j]==4){//split
                    selCol = red;
                }
                else if(slab_type[r][i][j]==6){//bridgecreate1
                    selCol = gold;
                }
                else if (slab_type[r][i][j]==7){//bridge
                    selCol = white;
                }
                else if (slab_type[r][i][j]==3){//loosslab
                    selCol = steel;
                }
                else
                {
                    selCol = slab_colors[(i+j)%2];
                }
                slab[r][i][j].createRectangleAll(slab_width,slab_height,slab_breath,darkbrown,darkbrown,selCol);
                slab[r][i][j].loc_set(slab_width*(i-stage_width[Game_Stage]/2),-slab_height,slab_breath*(-j+stage_breath[Game_Stage]/2));
            }
        }
    }
    block1.createRectangleAll(slab_width,slab_width,slab_width,darkbrown,darkbrown,lightbrown);
    block2.createRectangleAll(slab_width,slab_width,slab_width,darkbrown,darkbrown,lightbrown);

    // block2.createRectangleAll(slab_width,slab_width,slab_width,white,white,grey);
    // block1.createRectangleAll(slab_width,slab_width,slab_width,white,white,grey);

}
void ifstpedsolve(){
    if(slab_type[Game_Stage][block1_xi+stage_width[Game_Stage]/2][block1_zi+stage_breath[Game_Stage]/2]==5 && slab_type[Game_Stage][block2_xi+stage_width[Game_Stage]/2][block2_zi+stage_breath[Game_Stage]/2]==5)
    {
        std::cout << "solve" << '\n';
        selected_block = 4;
        // TotalScore = 0;
        TotalScorePre = TotalScore;
    }
}
void ifstpedout(){
    if( block1_xi>stage_width[Game_Stage]/2||block1_zi>stage_breath[Game_Stage]/2|| block2_xi>stage_width[Game_Stage]/2 || block2_zi>stage_breath[Game_Stage]/2 ||
        block1_xi<-stage_width[Game_Stage]/2||block1_zi<-stage_breath[Game_Stage]/2|| block2_xi<-stage_width[Game_Stage]/2 || block2_zi<-stage_breath[Game_Stage]/2 ||
        slab_type[Game_Stage][block1_xi+stage_width[Game_Stage]/2][block1_zi+stage_breath[Game_Stage]/2]==0 ||
        slab_type[Game_Stage][block2_xi+stage_width[Game_Stage]/2][block2_zi+stage_breath[Game_Stage]/2]==0||
        slab_type[Game_Stage][block1_xi+stage_width[Game_Stage]/2][block1_zi+stage_breath[Game_Stage]/2]==7 ||
        slab_type[Game_Stage][block2_xi+stage_width[Game_Stage]/2][block2_zi+stage_breath[Game_Stage]/2]==7)
    {//if stped in space
        std::cout << "fall" << '\n';
        selected_block=3;
        TotalScore = TotalScorePre;
    }
}
void ifstpedsplit(){
    if(slab_type[Game_Stage][block1_xi+stage_width[Game_Stage]/2][block1_zi+stage_breath[Game_Stage]/2]==4 &&
        slab_type[Game_Stage][block2_xi+stage_width[Game_Stage]/2][block2_zi+stage_breath[Game_Stage]/2]==4)
    {
        selected_block = 1;
        block2_yi=0;
        block1_yi=0;
        std::cout << "split" << '\n';
        for(int i=0;i<len_special;i++){
            if(specialData[i][0]==Game_Stage&&((specialData[i][1]==block1_xi+stage_width[Game_Stage]/2 && specialData[i][2]== block1_zi+stage_breath[Game_Stage]/2 )
                &&(specialData[i][1]==block2_xi+stage_width[Game_Stage]/2 && specialData[i][2]== block2_zi+stage_breath[Game_Stage]/2)))
            {
                // std::cout << block1_xi+stage_width/2  << '\n';
                block1_xi=specialData[i][3]-stage_width[Game_Stage]/2;
                block1_zi=specialData[i][4]-stage_breath[Game_Stage]/2;
                block2_xi=specialData[i][5]-stage_width[Game_Stage]/2;
                block2_zi=specialData[i][6]-stage_breath[Game_Stage]/2;
            }
        }
    }
}
void ifCnjoin(){
    if((selected_block==1|| selected_block==2)&& block1_xi==block2_xi && (block1_zi-block2_zi==1 || block1_zi-block2_zi==-1))
    {
        selected_block=0;
    }
    else if((selected_block==1|| selected_block==2) && block1_zi==block2_zi && (block1_xi-block2_xi==1 || block1_xi-block2_xi==-1))
    {
        selected_block=0;
    }
}
void ifstpedbridgeCreate1(){
    if(slab_type[Game_Stage][block1_xi+stage_width[Game_Stage]/2][block1_zi+stage_breath[Game_Stage]/2]==6 ||
        slab_type[Game_Stage][block2_xi+stage_width[Game_Stage]/2][block2_zi+stage_breath[Game_Stage]/2]==6)
    {
        std::cout << "stped on bridge create 1" << '\n';
        for(int i=0;i<len_special;i++){
            if(specialData[i][0]==Game_Stage&&((specialData[i][1]==block1_xi+stage_width[Game_Stage]/2 && specialData[i][2]== block1_zi+stage_breath[Game_Stage]/2)
            ||(specialData[i][1]==block2_xi+stage_width[Game_Stage]/2 && specialData[i][2]== block2_zi+stage_breath[Game_Stage]/2)))
            {
                slab_type[Game_Stage][specialData[i][3]][specialData[i][4]]=1;
                slab_type[Game_Stage][specialData[i][5]][specialData[i][6]]=1;
            }
        }
    }
}
void ifstpedbridgeCreate2(){
    if(slab_type[Game_Stage][block1_xi+stage_width[Game_Stage]/2][block1_zi+stage_breath[Game_Stage]/2]==2 &&
        slab_type[Game_Stage][block2_xi+stage_width[Game_Stage]/2][block2_zi+stage_breath[Game_Stage]/2]==2)
    {
        std::cout << "stped on bridge create 2" << '\n';
        std::cout << "x1 = "<< block1_xi+stage_width[Game_Stage]/2 << "z1 =" << block1_zi+stage_breath[Game_Stage]/2 << '\n';
        for(int i=0;i<len_special;i++){
            if(specialData[i][0] == Game_Stage&&
                ((specialData[i][1]==block1_xi+stage_width[Game_Stage]/2 && specialData[i][2]== block1_zi+stage_breath[Game_Stage]/2)
            &&(specialData[i][1]==block2_xi+stage_width[Game_Stage]/2 && specialData[i][2]== block2_zi+stage_breath[Game_Stage]/2)))
            {
                slab_type[Game_Stage][specialData[i][3]][specialData[i][4]]=1;
                slab_type[Game_Stage][specialData[i][5]][specialData[i][6]]=1;
                // std::cout << specialData[i][0] <<" "<< specialData[i][1] <<" "<< specialData[i][2] <<" "<<specialData[i][3] <<" "<<specialData[i][4] <<" "<<specialData[i][5] <<" "<<specialData[i][6]<< '\n';
                std::cout << "bridgeform2" << '\n';
            }
        }
    }
}
void ifstpedloosSlab(){
    if(slab_type[Game_Stage][block1_xi+stage_width[Game_Stage]/2][block1_zi+stage_breath[Game_Stage]/2]==3 &&
        slab_type[Game_Stage][block2_xi+stage_width[Game_Stage]/2][block2_zi+stage_breath[Game_Stage]/2]==3 &&
        block1_xi+stage_width[Game_Stage]/2 ==block2_xi+stage_width[Game_Stage]/2 &&block1_zi+stage_breath[Game_Stage]/2== block2_zi+stage_breath[Game_Stage]/2  )
    {
        std::cout << "loos brige breking" << '\n';
        slab_type[Game_Stage][block1_xi+stage_width[Game_Stage]/2][block1_zi+stage_breath[Game_Stage]/2]=0;

    }
}
void cameraSetting(GLFWwindow* window){
    double a,b;
    glfwGetCursorPos(window,&a,&b);
    double cur_x=(a-400)/100 ,cur_y= (b-400)/100;
    if(view_ortho==1){
         std::cout << "ortho view" << '\n';
        perspective_projection=0;
        cam_eye_z =5;
        cam_eye_x =5;
        cam_eye_y =5;
        reshapeWindow(window,1000,1000);
    }
    else if (view_top==1){
        std::cout << "top view" << '\n';
        perspective_projection=1;
        cam_eye_x =0.0001;
        cam_eye_y =7;
        cam_eye_z =0.00001;
        reshapeWindow(window,1000,1000);
    }
    else if(view_tower==1){
        std::cout << "tower view" << '\n';
        perspective_projection=1;
        cam_eye_z =5;
        cam_eye_x =6;
        cam_eye_y =6;
        reshapeWindow(window,1000,1000);
    }
    else if(mouse_left_clicked == 1 && view_helicoptor ==1){
        perspective_projection=1;
        std::cout << "helicoptor view" << '\n';
        float x_dif = cur_x - mouse_left_clicked_loc_x,y_dif = cur_y + mouse_left_clicked_loc_y;
        // cam_eye_x = 7*sin(-7*x_dif*M_PI/180.0f);sasb
        // cam_eye_z = 7*cos(-7*x_dif*M_PI/180.0f);casb
        cam_eye_x = -7*sin(7*x_dif*M_PI/180.0f)*sin(7*y_dif*M_PI/180.0f);
        cam_eye_y = -7*cos(-7*x_dif*M_PI/180.0f)*sin(-7*y_dif*M_PI/180.0f);
        cam_eye_z = 7*cos(-7*y_dif*M_PI/180.0f);
        reshapeWindow(window,1000,1000);
    }

    else if(view_follow_block==1)
    {
        std::cout << "blockfollow_view" << '\n';
        perspective_projection=1;
        if(latest_left_clicked==1){
            cam_eye_x = 6;
            cam_eye_y = 6;
            cam_eye_z = 0;
        }
        else if(latest_right_clicked==1){
            cam_eye_x = -6;
            cam_eye_y = 6;
            cam_eye_z = 0;
        }
        else if(latest_up_clicked==1){
            cam_eye_x = 0;
            cam_eye_y = 6;
            cam_eye_z = 6;
        }
        else if(latest_down_clicked==1){
            cam_eye_x = 0;
            cam_eye_y = 6;
            cam_eye_z = -6;
        }
        reshapeWindow(window,1000,1000);
    }
    else if(view_block_view){
        perspective_projection=1;
        float av_x=(block1_xi+block2_xi)/2;
        float av_y=(block1_yi+block2_yi)/2;
        float av_z=(block1_zi+block2_zi)/2;
        if(latest_left_clicked==1){
            cam_eye_x = slab_width*av_x-1;
            cam_eye_y = 1;
            cam_eye_z = slab_breath*(-1*av_z);
            cam_target_x = slab_width*av_x-3;
            cam_target_y = 0;
            cam_target_z = slab_breath*(-1*av_z);
        }
        else if(latest_right_clicked==1){
            cam_eye_x = slab_width*av_x+1;
            cam_eye_y = 1;
            cam_eye_z = slab_breath*(-1*av_z);
            cam_target_x = slab_width*av_x+3;
            cam_target_y = 0;
            cam_target_z = slab_breath*(-1*av_z);
        }
        else if(latest_up_clicked==1){
            cam_eye_x = slab_width*av_x;
            cam_eye_y = 1;
            cam_eye_z = slab_breath*(-1*av_z)-1;
            cam_target_x = slab_width*av_x;
            cam_target_y = 0;
            cam_target_z = slab_breath*(-1*av_z)-3;
        }
        else if(latest_down_clicked==1){
            cam_eye_x = slab_width*av_x;
            cam_eye_y = 1;
            cam_eye_z = slab_breath*(-1*av_z)+1;
            cam_target_x = slab_width*av_x;
            cam_target_y = 0;
            cam_target_z = slab_breath*(-1*av_z)+3;
        }
        reshapeWindow(window,1000,1000);
    }
}
int main (int argc, char** argv)
{
	int width = 1000;int height = 1000;
    GLFWwindow* window = initGLFW(width, height);
	initGL (window, width, height);
    double last_update_time = glfwGetTime(), current_time;
    for(int r =0;r<no_stages;r++){
        for (int i=0;i<20;i++){
            for(int j=0;j<20;j++){
                slab_type[r][i][j]=0;
            }
        }
    }

    StageBuild();
    Initialiseblock();
    CreateAllObjects();
    float depth_fall =0;
    float ang =0;
    while (!glfwWindowShouldClose(window)) {
        glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        cameraSetting(window);
        //<<<<<<<<<<<<<<<<<<<<<<ALL DRAW FUNCTION>>>>>>>>>>>>>>>>>
        for(int i=0;i<20;i++){
            for(int j=0;j<20;j++){
                if(slab_type[Game_Stage][i][j]!=0 && slab_type[Game_Stage][i][j]!=5 && slab_type[Game_Stage][i][j]!=7){
                    slab[Game_Stage][i][j].draw();
                }
            }
        }
        //<<<<<<<<<<<<<<<<TIME FUNCTION>>>>>>>>>>>>>>>>>>>
        glfwPollEvents();
        current_time = glfwGetTime(); // Time in seconds
        if ((current_time - last_update_time) >= 0.05) { // atleast 0.5s elapsed since last frame
            // do something every 0.5 seconds ..
            if(selected_block == 3 || selected_block == 4){//falling
                block1.loc_set(slab_width*block1_xi,slab_width*block1_yi-depth_fall,slab_breath*(-1*block1_zi));
                block2.loc_set(slab_width*block2_xi,slab_width*block2_yi-depth_fall,slab_breath*(-1*block2_zi));
                depth_fall = depth_fall +0.06;
                std::cout << "fall movement" << '\n';
                if(depth_fall > 2 ){
                    if(selected_block == 4){
                        Game_Stage++;
                        system("mpg123 -n 30 -i -q welply.mp3 &");
                    }
                    std::cout << "outuutut" << '\n';
                    depth_fall=0;
                    StageBuild();
                    Initialiseblock();
                }
            }
            else if(rotating_block==1){//rolling
                if(left_clicked==1){
                    std::cout << "left_clicked" << '\n';
                    if(selected_block==1){
                        block1.ang_set(ang,0,0,1);
                        block1.loc_set(slab_width*block1_xi,slab_width*block1_yi,slab_breath*(-1*block1_zi));
                    }
                    else if(selected_block==2){
                        block2.ang_set(ang,0,0,1);
                        block2.loc_set(slab_width*block2_xi,slab_width*block2_yi,slab_breath*(-1*block2_zi));
                    }
                    else if(selected_block==0){
                        if(block1_yi==1){
                            block1.fst_loc_set(0,slab_width,0);
                            block1.ang_set(ang,0,0,1);
                            block1.loc_set(slab_width*block1_xi,slab_width*block1_yi-slab_width,slab_breath*(-1*block1_zi));
                            block2.ang_set(ang,0,0,1);
                            block2.loc_set(slab_width*block2_xi,slab_width*block2_yi,slab_breath*(-1*block2_zi));
                        }
                        else if(block2_yi==1){
                            block1.ang_set(ang,0,0,1);
                            block1.loc_set(slab_width*block1_xi,slab_width*block1_yi,slab_breath*(-1*block1_zi));
                            block2.fst_loc_set(0,slab_width,0);
                            block2.ang_set(ang,0,0,1);
                            block2.loc_set(slab_width*block2_xi,slab_width*block2_yi-slab_width,slab_breath*(-1*block2_zi));
                        }
                        else if(block1_yi==0 && block2_yi==0){
                            if(block1_zi==block2_zi){
                                if(block1_xi>block2_xi){
                                    block1.fst_loc_set(slab_width,0,0);
                                    block1.ang_set(ang,0,0,1);
                                    block1.loc_set(slab_width*block1_xi-slab_width,slab_width*block1_yi,slab_breath*(-1*block1_zi));
                                    block2.ang_set(ang,0,0,1);
                                    block2.loc_set(slab_width*block2_xi,slab_width*block2_yi,slab_breath*(-1*block2_zi));
                                }
                                else if(block1_xi<block2_xi){
                                    block1.ang_set(ang,0,0,1);
                                    block1.loc_set(slab_width*block1_xi,slab_width*block1_yi,slab_breath*(-1*block1_zi));
                                    block2.fst_loc_set(slab_width,0,0);
                                    block2.ang_set(ang,0,0,1);
                                    block2.loc_set(slab_width*block2_xi-slab_width,slab_width*block2_yi,slab_breath*(-1*block2_zi));
                                }
                            }
                            else{
                                block1.ang_set(ang,0,0,1);
                                block1.loc_set(slab_width*block1_xi,slab_width*block1_yi,slab_breath*(-1*block1_zi));
                                block2.ang_set(ang,0,0,1);
                                block2.loc_set(slab_width*block2_xi,slab_width*block2_yi,slab_breath*(-1*block2_zi));
                            }
                        }
                    }
                    ang=ang+10;
                    if(ang==90){
                        std::cout << "rotation left compleated0" << '\n';
                        if(selected_block==1){
                            block1_xi--;
                        }
                        else if(selected_block==2){
                            block2_xi--;
                        }
                        else if(selected_block==0){
                            block1_xi--;
                            block2_xi--;
                            if(block1_yi==1){
                                block1_xi--;

                                block1_yi=0;
                            }
                            else if(block2_yi==1){
                                block2_xi--;
                                block2_yi=0;
                            }
                            else if(block1_yi==0 && block2_yi==0){
                                if(block1_zi==block2_zi){
                                    if(block1_xi>block2_xi){
                                        block2_yi=1;
                                        block1_xi--;
                                    }
                                    else if(block1_xi<block2_xi){
                                        block1_yi=1;
                                        block2_xi--;
                                    }
                                }
                            }
                        }
                        ang=0;
                        block1.ang_set(0,0,0,1);
                        block2.ang_set(0,0,0,1);
                        block1.fst_loc_set(0,0,0);
                        block2.fst_loc_set(0,0,0);
                        block1.loc_set(slab_width*block1_xi,slab_width*block1_yi,slab_breath*(-1*block1_zi));
                        block2.loc_set(slab_width*block2_xi,slab_width*block2_yi,slab_breath*(-1*block2_zi));
                        left_clicked=0;
                        rotating_block=0;
                        TotalScore++;
                        system("mpg123 -n 30 -i -q blockRoll.mp3 &");
                    }
                }
                else if(right_clicked==1){
                    std::cout << "right_clicked" << '\n';
                    if(selected_block==1){
                        block1.fst_loc_set(-slab_width,0,0);
                        block1.ang_set(-ang,0,0,1);
                        block1.loc_set(slab_width*block1_xi+slab_width,slab_width*block1_yi,slab_breath*(-1*block1_zi));
                    }
                    else if(selected_block==2){
                        block2.fst_loc_set(-slab_width,0,0);
                        block2.ang_set(-ang,0,0,1);
                        block2.loc_set(slab_width*block2_xi+slab_width,slab_width*block2_yi,slab_breath*(-1*block2_zi));
                    }
                    else if (selected_block==0){
                        if (block1_yi==1){
                            block1.fst_loc_set(-slab_width,slab_width,0);
                            block1.ang_set(-ang,0,0,1);
                            block1.loc_set(slab_width*block1_xi+slab_width,slab_width*block1_yi-slab_width,slab_breath*(-1*block1_zi));
                            block2.fst_loc_set(-slab_width,0,0);
                            block2.ang_set(-ang,0,0,1);
                            block2.loc_set(slab_width*block2_xi+slab_width,slab_width*block2_yi,slab_breath*(-1*block2_zi));
                        }
                        else if (block2_yi==1){
                            block1.fst_loc_set(-slab_width,0,0);
                            block1.ang_set(-ang,0,0,1);
                            block1.loc_set(slab_width*block1_xi+slab_width,slab_width*block1_yi,slab_breath*(-1*block1_zi));
                            block2.fst_loc_set(-slab_width,slab_width,0);
                            block2.ang_set(-ang,0,0,1);
                            block2.loc_set(slab_width*block2_xi+slab_width,slab_width*block2_yi-slab_width,slab_breath*(-1*block2_zi));
                        }
                        else if(block1_yi==0&&block2_yi==0){
                            if(block1_zi==block2_zi){
                                if(block1_xi>block2_xi){
                                    block2.fst_loc_set(-2*slab_width,0,0);
                                    block2.ang_set(-ang,0,0,1);
                                    block2.loc_set(slab_width*block2_xi+2*slab_width,slab_width*block2_yi,slab_breath*(-1*block2_zi));
                                    block1.fst_loc_set(-slab_width,0,0);
                                    block1.ang_set(-ang,0,0,1);
                                    block1.loc_set(slab_width*block1_xi+slab_width,slab_width*block1_yi,slab_breath*(-1*block1_zi));
                                }
                                else if(block1_xi<block2_xi){
                                    block1.fst_loc_set(-2*slab_width,0,0);
                                    block1.ang_set(-ang,0,0,1);
                                    block1.loc_set(slab_width*block1_xi+2*slab_width,slab_width*block1_yi,slab_breath*(-1*block1_zi));
                                    block2.fst_loc_set(-slab_width,0,0);
                                    block2.ang_set(-ang,0,0,1);
                                    block2.loc_set(slab_width*block2_xi+slab_width,slab_width*block2_yi,slab_breath*(-1*block2_zi));
                                }
                            }
                            else{
                                block1.fst_loc_set(-slab_width,0,0);
                                block1.ang_set(-ang,0,0,1);
                                block1.loc_set(slab_width*block1_xi+slab_width,slab_width*block1_yi,slab_breath*(-1*block1_zi));
                                block2.fst_loc_set(-slab_width,0,0);
                                block2.ang_set(-ang,0,0,1);
                                block2.loc_set(slab_width*block2_xi+slab_width,slab_width*block2_yi,slab_breath*(-1*block2_zi));
                            }
                        }
                    }
                    ang=ang+10;
                    if(ang==90){
                        if(selected_block==1){
                            block1_xi++;
                        }
                        else if(selected_block==2){
                            block2_xi++;
                        }
                        else if (selected_block==0){
                            block1_xi++;
                            block2_xi++;
                            if (block1_yi==1){
                                block1_xi++;
                                block1_yi=0;
                            }
                            else if (block2_yi==1){
                                block2_xi++;
                                block2_yi=0;
                            }
                            else if(block1_yi==0&&block2_yi==0){
                                if(block1_zi==block2_zi){
                                    if(block1_xi>block2_xi){
                                        block1_yi=1;
                                        block2_xi++;
                                    }
                                    else if(block1_xi<block2_xi){
                                        block2_yi=1;
                                        block1_xi++;
                                    }
                                }
                            }
                        }
                        ang=0;
                        block1.ang_set(0,0,0,1);
                        block2.ang_set(0,0,0,1);
                        block1.fst_loc_set(0,0,0);
                        block2.fst_loc_set(0,0,0);
                        block1.loc_set(slab_width*block1_xi,slab_width*block1_yi,slab_breath*(-1*block1_zi));
                        block2.loc_set(slab_width*block2_xi,slab_width*block2_yi,slab_breath*(-1*block2_zi));
                        right_clicked=0;
                        rotating_block=0;
                        TotalScore++;
                        system("mpg123 -n 30 -i -q blockRoll.mp3 &");

                    }

                }
                else if(up_clicked==1){
                    std::cout << "up_clicked" << '\n';
                    if(selected_block==1){
                        block1.fst_loc_set(0,0,slab_width);
                        block1.ang_set(-ang,1,0,0);
                        block1.loc_set(slab_width*block1_xi,slab_width*block1_yi,slab_breath*(-1*block1_zi)-slab_width);
                    }
                    else if(selected_block==2){
                        block2.fst_loc_set(0,0,slab_width);
                        block2.ang_set(-ang,1,0,0);
                        block2.loc_set(slab_width*block2_xi,slab_width*block2_yi,slab_breath*(-1*block2_zi)-slab_width);
                    }
                    else if (selected_block==0){
                        std::cout << "select == 0" << '\n';
                        if (block1_yi==1){
                            std::cout << "1y" << '\n';
                            block2.fst_loc_set(0,0,slab_width);
                            block2.ang_set(-ang,1,0,0);
                            block2.loc_set(slab_width*block2_xi,slab_width*block2_yi,slab_breath*(-1*block2_zi)-slab_width);
                            block1.fst_loc_set(0,slab_width,slab_width);
                            block1.ang_set(-ang,1,0,0);
                            block1.loc_set(slab_width*block1_xi,slab_width*block1_yi-slab_width,slab_breath*(-1*block1_zi)-slab_width);
                        }
                        else if (block2_yi==1){
                            std::cout << "2y" << '\n';
                            block1.fst_loc_set(0,0,slab_width);
                            block1.ang_set(-ang,1,0,0);
                            block1.loc_set(slab_width*block1_xi,slab_width*block1_yi,slab_breath*(-1*block1_zi)-slab_width);
                            block2.fst_loc_set(0,slab_width,slab_width);
                            block2.ang_set(-ang,1,0,0);
                            block2.loc_set(slab_width*block2_xi,slab_width*block2_yi-slab_width,slab_breath*(-1*block2_zi)-slab_width);
                        }
                        else if(block1_yi==0 && block2_yi==0){
                            std::cout << "1y==2y==0" << '\n';
                            if(block1_xi==block2_xi){
                                std::cout << "1x==2x==0" << '\n';
                                if(block1_zi>block2_zi){
                                    std::cout << "1" << '\n';
                                    block2.fst_loc_set(0,0,2*slab_width);
                                    block2.ang_set(-ang,1,0,0);
                                    block2.loc_set(slab_width*block2_xi,slab_width*block2_yi,slab_breath*(-1*block2_zi)-2*slab_width);
                                    block1.fst_loc_set(0,0,slab_width);
                                    block1.ang_set(-ang,1,0,0);
                                    block1.loc_set(slab_width*block1_xi,slab_width*block1_yi,slab_breath*(-1*block1_zi)-slab_width);
                                }
                                else if(block1_zi<block2_zi){
                                    std::cout << "2" << '\n';
                                    block1.fst_loc_set(0,0,2*slab_width);
                                    block1.ang_set(-ang,1,0,0);
                                    block1.loc_set(slab_width*block1_xi,slab_width*block1_yi,slab_breath*(-1*block1_zi)-2*slab_width);
                                    block2.fst_loc_set(0,0,slab_width);
                                    block2.ang_set(-ang,1,0,0);
                                    block2.loc_set(slab_width*block2_xi,slab_width*block2_yi,slab_breath*(-1*block2_zi)-slab_width);
                                }
                            }
                            else {
                                block1.fst_loc_set(0,0,slab_width);
                                block1.ang_set(-ang,1,0,0);
                                block1.loc_set(slab_width*block1_xi,slab_width*block1_yi,slab_breath*(-1*block1_zi)-slab_width);
                                block2.fst_loc_set(0,0,slab_width);
                                block2.ang_set(-ang,1,0,0);
                                block2.loc_set(slab_width*block2_xi,slab_width*block2_yi,slab_breath*(-1*block2_zi)-slab_width);
                            }
                        }
                    }
                    ang=ang+10;
                    if(ang==90){
                        if(selected_block==1){
                            block1_zi++;
                        }
                        else if(selected_block==2){
                            block2_zi++;
                        }
                        else if (selected_block==0){
                            block1_zi++;
                            block2_zi++;
                            if (block1_yi==1){
                                block1_zi++;
                                block1_yi=0;
                            }
                            else if (block2_yi==1){
                                block2_zi++;
                                block2_yi=0;
                            }
                            else if(block1_yi==0 && block2_yi==0){
                                if(block1_xi==block2_xi){
                                    if(block1_zi>block2_zi){
                                        block1_yi=1;
                                        block2_zi++;
                                    }
                                    else if(block1_zi<block2_zi){
                                        block2_yi=1;
                                        block1_zi++;
                                    }
                                }
                            }
                        }
                        ang=0;
                        block1.ang_set(0,0,0,1);
                        block2.ang_set(0,0,0,1);
                        block1.fst_loc_set(0,0,0);
                        block2.fst_loc_set(0,0,0);
                        block1.loc_set(slab_width*block1_xi,slab_width*block1_yi,slab_breath*(-1*block1_zi));
                        block2.loc_set(slab_width*block2_xi,slab_width*block2_yi,slab_breath*(-1*block2_zi));
                        up_clicked=0;
                        rotating_block=0;
                        TotalScore++;
                        system("mpg123 -n 30 -i -q blockRoll.mp3 &");

                    }
                }
                else if(down_clicked==1){
                    std::cout << "down_clicked" << '\n';
                    if(selected_block==1){
                        block1.ang_set(ang,1,0,0);
                        block1.loc_set(slab_width*block1_xi,slab_width*block1_yi,slab_breath*(-1*block1_zi));
                    }
                    else if(selected_block==2){
                        block2.ang_set(ang,1,0,0);
                        block2.loc_set(slab_width*block2_xi,slab_width*block2_yi,slab_breath*(-1*block2_zi));
                    }
                    else if (selected_block==0){
                        std::cout << "select == 0" << '\n';
                        if (block1_yi==1){
                            std::cout << "1y" << '\n';
                            block2.fst_loc_set(0,0,0);
                            block2.ang_set(ang,1,0,0);
                            block2.loc_set(slab_width*block2_xi,slab_width*block2_yi,slab_breath*(-1*block2_zi));
                            block1.ang_set(ang,1,0,0);
                            block1.fst_loc_set(0,slab_width,0);
                            block1.loc_set(slab_width*block1_xi,slab_width*block1_yi-slab_width ,slab_breath*(-1*block1_zi));
                        }
                        else if (block2_yi==1){
                            std::cout << "2y" << '\n';
                            block1.fst_loc_set(0,0,0);
                            block1.ang_set(ang,1,0,0);
                            block1.loc_set(slab_width*block1_xi,slab_width*block1_yi,slab_breath*(-1*block1_zi));
                            block2.ang_set(ang,1,0,0);
                            block2.fst_loc_set(0,slab_width,0);
                            block2.loc_set(slab_width*block2_xi,slab_width*block2_yi-slab_width ,slab_breath*(-1*block2_zi));
                        }
                        else if(block1_yi==0 && block2_yi==0){
                            std::cout << "1y==2y==0" << '\n';
                            if(block1_xi==block2_xi){
                                std::cout << "1x==2x==0" << '\n';
                                if(block1_zi<block2_zi){
                                    std::cout << "1" << '\n';
                                    block2.fst_loc_set(0,slab_width,-slab_width);
                                    block2.ang_set(ang,1,0,0);
                                    block2.loc_set(slab_width*block2_xi,slab_width*block2_yi-slab_width,slab_breath*(-1*block2_zi)+slab_width);
                                    block1.fst_loc_set(0,0,0);
                                    block1.ang_set(ang,1,0,0);
                                    block1.loc_set(slab_width*block1_xi,slab_width*block1_yi,slab_breath*(-1*block1_zi));
                                }
                                else if(block1_zi>block2_zi){
                                    std::cout << "2" << '\n';
                                    block1.fst_loc_set(0,slab_width,-slab_width);
                                    block1.ang_set(ang,1,0,0);
                                    block1.loc_set(slab_width*block1_xi,slab_width*block1_yi-slab_width,slab_breath*(-1*block1_zi)+slab_width);
                                    block2.fst_loc_set(0,0,0);
                                    block2.ang_set(ang,1,0,0);
                                    block2.loc_set(slab_width*block2_xi,slab_width*block2_yi,slab_breath*(-1*block2_zi));
                                }
                            }
                            else {
                                block1.ang_set(ang,1,0,0);
                                block1.loc_set(slab_width*block1_xi,slab_width*block1_yi,slab_breath*(-1*block1_zi));
                                block2.ang_set(ang,1,0,0);
                                block2.loc_set(slab_width*block2_xi,slab_width*block2_yi,slab_breath*(-1*block2_zi));
                            }
                        }
                    }
                    ang=ang+10;
                    if(ang==90){
                        if(selected_block==1){
                            block1_zi--;
                        }
                        else if(selected_block==2){
                            block2_zi--;
                        }
                        else if (selected_block==0){
                            if (block1_yi==1){
                                block1_zi--;
                                block1_zi--;
                                block2_zi--;
                                block1_yi=0;
                            }
                            else if (block2_yi==1){
                                block2_zi--;
                                block1_zi--;
                                block2_zi--;
                                block2_yi=0;
                            }
                            else if(block1_yi==0&&block2_yi==0){
                                block1_zi--;
                                block2_zi--;
                                if(block1_xi==block2_xi){
                                    if(block1_zi>block2_zi){
                                        block2_yi=1;
                                        block1_zi--;
                                    }
                                    else if(block1_zi<block2_zi){
                                        block1_yi=1;
                                        block2_zi--;
                                    }
                                }
                            }
                        }
                        ang=0;
                        block1.ang_set(0,0,0,1);
                        block2.ang_set(0,0,0,1);
                        block1.fst_loc_set(0,0,0);
                        block2.fst_loc_set(0,0,0);
                        block1.loc_set(slab_width*block1_xi,slab_width*block1_yi,slab_breath*(-1*block1_zi));
                        block2.loc_set(slab_width*block2_xi,slab_width*block2_yi,slab_breath*(-1*block2_zi));
                        down_clicked=0;
                        rotating_block=0;
                        TotalScore++;
                        system("mpg123 -n 30 -i -q blockRoll.mp3 &");
                    }

                }
            }
            else{
                block1.loc_set(slab_width*block1_xi,slab_width*block1_yi,slab_breath*(-1*block1_zi));
                block2.loc_set(slab_width*block2_xi,slab_width*block2_yi,slab_breath*(-1*block2_zi));
            }
            last_update_time = current_time;
        }

        numberdraw(TotalScore);

        ifstpedsolve();//if stped in answer

        ifstpedout();//sink the block

        ifstpedsplit();//split the block

        ifCnjoin();//join the block

        ifstpedbridgeCreate1();//one step bridge creater

        ifstpedbridgeCreate2();//two step bridge creater

        ifstpedloosSlab();//stped in loos bridge

        block1.draw();
        block2.draw();
        glfwSwapBuffers(window);
    }

    glfwTerminate();
//    exit(EXIT_SUCCESS);
}
