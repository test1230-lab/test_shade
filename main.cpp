#pragma comment (lib, "glew32s.lib")
#define GLEW_STATIC

#include <GL\glew.h> 
#include <GL\freeglut.h> 
#include <iostream>

#include <string> 

#include <cmath>


/*
std::string vertexShader = "#version 430\n"
"in vec3 pos;"
"void main() {"
"gl_Position = vec4(pos, 1.0);"
"}";
*/


std::string vertexShader = "#version 430\n"
"in vec3 pos;"
"void main() {"
"gl_Position = vec4(pos, 1.0);"
"}";




std::string fragmentShader = "#version 430\n"
"#define r(v,t) v *= mat2( C = cos((t)*time), S = sin((t)*time), -S, C )\n"
"in vec3 gl_FragCoord;"
"uniform float time;"
"vec2 res;"
"out vec4 f;"
"void main(){"
"res.x = 640.0;"
"res.y = 480.0;"
"float C,S,r,x;"
"vec4 p = vec4(gl_FragCoord,1)/res.yyxy-0.5, d; p.x-=0.4;"
"r(p.xz,0.13); r(p.yz,0.2); r(p.xy,0.1); "
"d = p;"
"p.z += 5.0*time;"
"for(float i=1.0; i>0.0; i-=0.01){"
"vec4 u = floor(p/8.0), t = mod(p, 8.0)-4.0, M,R;"
"u = sin(78.0*(u+u.yzxw));"
"r = 1.2; t = abs(t);"
"M=max(t,t.yzxw);"
"x = max(t.x,M.y)-r;"
"R = r-u/2.0-M;"
"if(x<.01){ f = i*i*(1.7+0.02*t); break; }"
"p -= d*x;"
"}"
"}";



/*
std::string fragmentShader = "#version 430\n"
"in vec3 gl_FragCoord;"
"uniform float time;"
"void main() {"
"vec3 uv = gl_FragCoord/(640.0, 480.0);"
"vec3 col = 0.5 + 0.5*cos(time+uv.xyx+vec3(0,2,4));"
"gl_FragColor = vec4(col,1.0);"
"}";
*/


// Compile and create shader object and returns its id 
GLuint compileShaders(std::string shader, GLenum type)
{
    const char* shaderCode = shader.c_str();
    GLuint shaderId = glCreateShader(type);

    if (shaderId == 0) { // Error: Cannot create shader object 
        std::cout << "Error creating shaders";
        return 0;
    }

    // Attach source code to this object 
    glShaderSource(shaderId, 1, &shaderCode, NULL);
    glCompileShader(shaderId); // compile the shader object 

    GLint compileStatus;

    // check for compilation status 
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compileStatus);

    if (!compileStatus) { // If compilation was not successful 
        int length;
        glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &length);
        char* cMessage = new char[length];

        // Get additional information 
        glGetShaderInfoLog(shaderId, length, &length, cMessage);
        std::cout << "Cannot Compile Shader: " << cMessage;
        delete[] cMessage;
        glDeleteShader(shaderId);
        return 0;
    }

    return shaderId;
}

// Creates a program containg vertex and fragment shader 
// links it and returns its ID 
GLuint linkProgram(GLuint vertexShaderId, GLuint fragmentShaderId)
{
    GLuint programId = glCreateProgram(); // create a program 

    if (programId == 0) {
        std::cout << "Error Creating Shader Program";
        return 0;
    }

    // Attach both the shaders to it 
    glAttachShader(programId, vertexShaderId);
    glAttachShader(programId, fragmentShaderId);

    // Create executable of this program 
    glLinkProgram(programId);

    GLint linkStatus;

    // Get the link status for this program 
    glGetProgramiv(programId, GL_LINK_STATUS, &linkStatus);

    if (!linkStatus) { // If the linking failed 
        std::cout << "Error Linking program";
        glDetachShader(programId, vertexShaderId);
        glDetachShader(programId, fragmentShaderId);
        glDeleteProgram(programId);

        return 0;
    }

    return programId;
}

// Load data in VBO and return the vbo's id 
GLuint loadDataInBuffers()
{
    GLfloat vertices[] = { // vertex coordinates                            
                           -1, -1, 0,
                           1, -1, 0,
                           -1, 1, 0,
                           1, 1, 0

    };

    GLuint vboId;

    // allocate buffer sapce and pass data to it 
    glGenBuffers(1, &vboId);
    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // unbind the active buffer 
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return vboId;
}

// Initialize and put everything together 
void init()
{
    // clear the framebuffer each frame with black color 
    glClearColor(0, 0, 0, 0);

    GLuint vboId = loadDataInBuffers();

    GLuint vShaderId = compileShaders(vertexShader, GL_VERTEX_SHADER);
    GLuint fShaderId = compileShaders(fragmentShader, GL_FRAGMENT_SHADER);

    GLuint programId = linkProgram(vShaderId, fShaderId);

    // Get the 'pos' variable location inside this program 
    GLuint posAttributePosition = glGetAttribLocation(programId, "pos");

    GLuint vaoId;
    glGenVertexArrays(1, &vaoId); // Generate VAO 

    // Bind it so that rest of vao operations affect this vao 
    glBindVertexArray(vaoId);

    // buffer from which 'pos' will recive its data and the format of that data 
    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    glVertexAttribPointer(posAttributePosition, 3, GL_FLOAT, false, 0, 0);

    // Enable this attribute array linked to 'pos' 
    glEnableVertexAttribArray(posAttributePosition);

    // Use this program for rendering. 
    glUseProgram(programId);

}

// Function that does the drawing 
// glut calls this function whenever it needs to redraw 
void display()
{
    //update shader with new values
    GLint id;

    glGetIntegerv(GL_CURRENT_PROGRAM, &id);


    float etime_glut = glutGet(GLUT_ELAPSED_TIME);
    etime_glut /= 1000;

    GLint time = glGetUniformLocation(id, "time");
    if (time != -1)
    {
        std::cout << "time: " << etime_glut << "\r";
        glUniform1f(time, etime_glut);
    }

    // clear the color buffer before each drawing 
    glClear(GL_COLOR_BUFFER_BIT);

    // draw triangles starting from index 0 and 
    // using 3 indices 
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // swap the buffers and hence show the buffers  
    // content to the screen 
    glutSwapBuffers();

    //update glut window continuously
    glutPostRedisplay();
}

// main function 
int main(int argc, char** argv)
{
    std::cout << "running . . .\n";
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    //glutInitWindowSize(1920, 1080);
    glutInitWindowSize(640, 480);
    glutCreateWindow("OpenGl Window");
    //glutFullScreen();
    glewInit();
    init();
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}