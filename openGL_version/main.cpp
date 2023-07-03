#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <cstring>

#include "scene.hpp"

void test_opengl_error(std::string func, std::string file, int line);

#define TEST_OPENGL_ERROR()                                                             \
  do {                                                              		  							\
     test_opengl_error(__func__, __FILE__, __LINE__);                                   \
  } while(0)


GLuint program_id;
GLuint vao_id;

int pixWIDTH = 1024;//glutGet(GLUT_WINDOW_WIDTH);
int pixHEIGHT = 1024;//glutGet(GLUT_WINDOW_HEIGHT);

void window_resize(int width, int height) {
  glViewport(0,0,width,height);TEST_OPENGL_ERROR();
}

void display() {
  glUseProgram(program_id);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);TEST_OPENGL_ERROR();
  glGenVertexArrays(1, &vao_id);TEST_OPENGL_ERROR();
  glBindVertexArray(vao_id);TEST_OPENGL_ERROR();
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);TEST_OPENGL_ERROR();
  glBindVertexArray(0);TEST_OPENGL_ERROR();
  glutSwapBuffers();
}

void init_glut(int &argc, char *argv[]) {
  glutInit(&argc, argv);
  glutInitContextVersion(4,5);
  glutInitContextProfile(GLUT_CORE_PROFILE | GLUT_DEBUG);
  glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);
  glutInitWindowSize(pixWIDTH, pixHEIGHT);
  glutCreateWindow("Shader Programming");
  glutDisplayFunc(display);
  glutReshapeFunc(window_resize);
}

bool init_glew() {
  if (glewInit()) {
    std::cerr << " Error while initializing glew";
    return false;
  }
  return true;
}

void init_GL() {
  glEnable(GL_DEPTH_TEST);TEST_OPENGL_ERROR();
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);TEST_OPENGL_ERROR();
  glEnable(GL_CULL_FACE);TEST_OPENGL_ERROR();
  glClearColor(0.4,0.4,0.4,1.0);TEST_OPENGL_ERROR();
}

std::string load(const std::string &filename) {
  std::ifstream input_src_file(filename, std::ios::in);
  std::string ligne;
  std::string file_content="";
  if (input_src_file.fail()) {
    std::cerr << "FAILURE: can not load " << filename << "\n";
    return "";
  }
  while(getline(input_src_file, ligne)) {
    file_content = file_content + ligne + "\n";
  }
  file_content += '\0';
  input_src_file.close();
  return file_content;
}

bool load_and_compile_shader(const GLenum shader_type,const std::string shader_src_filename, GLuint &shader_id) {
  GLint compile_status = GL_TRUE;
  std::string shader_src = load(shader_src_filename);
  const GLchar *sources[1];
  sources[0] = shader_src.c_str();
  shader_id = glCreateShader(shader_type);TEST_OPENGL_ERROR();
  glShaderSource(shader_id, 1, sources, 0);TEST_OPENGL_ERROR();
  glCompileShader(shader_id);TEST_OPENGL_ERROR();
  glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compile_status);
  if(compile_status != GL_TRUE) {
      GLint log_size;
      char *shader_log;
      glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_size);
      shader_log = (char*)std::malloc(log_size+1); /* +1 pour le caractere de fin de chaine '\0' */
      if(shader_log != 0) {
      	glGetShaderInfoLog(shader_id, log_size, &log_size, shader_log);
	      std::cerr << "FAILURE can not compile shader " << shader_src_filename << ": " << shader_log << std::endl;
    	  std::free(shader_log);
      }
      glDeleteShader(shader_id);
      return false;
  }
  return true;
}

bool attach_and_link_program(const std::vector<GLuint> &shaders_id, GLuint &program_id) {
  GLint link_status=GL_TRUE;
  program_id=glCreateProgram();TEST_OPENGL_ERROR();
  if (program_id==0) return false;
  for(unsigned int i = 0 ; i < shaders_id.size() ; i++) {
    glAttachShader(program_id, shaders_id[i]);TEST_OPENGL_ERROR();
  }
  glLinkProgram(program_id);TEST_OPENGL_ERROR();
  glGetProgramiv(program_id, GL_LINK_STATUS, &link_status);
  if (link_status!=GL_TRUE) {
    GLint log_size;
    char *program_log;
    glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &log_size);
    program_log = (char*)std::malloc(log_size+1); /* +1 pour le caractere de fin de chaine '\0' */
    if(program_log != 0) {
      glGetProgramInfoLog(program_id, log_size, &log_size, program_log);
      std::cerr << "FAILURE: Program can not be linked " << program_log << std::endl;
      std::free(program_log);
    }
    for(unsigned int i = 0 ; i < shaders_id.size() ; i++) {
      glDetachShader(program_id, shaders_id[i]);TEST_OPENGL_ERROR();
    }
    glDeleteProgram(program_id);TEST_OPENGL_ERROR();
    program_id=0;
    return false;
  }
  return true;
}

bool init_shaders() {
  GLuint vertex_shader_id, fragment_shader_id;
  if (!load_and_compile_shader(GL_VERTEX_SHADER, "vertex.glsl", vertex_shader_id)) {
    return false;
  }
  if (!load_and_compile_shader(GL_FRAGMENT_SHADER, "fragment.glsl", fragment_shader_id)) {
    return false;
  }

  std::vector<GLuint> shaders_id;
  shaders_id.push_back(vertex_shader_id);
  shaders_id.push_back(fragment_shader_id);


  if (!attach_and_link_program(shaders_id, program_id)) {
    for(unsigned int i = 0 ; i < shaders_id.size() ; i++) {
      glDeleteShader(shaders_id[i]);TEST_OPENGL_ERROR();
    }
    return false;
  }

  for(unsigned int i = 0 ; i < shaders_id.size() ; i++) {
    glDetachShader(program_id, shaders_id[i]);TEST_OPENGL_ERROR();
  }

  for(unsigned int i = 0 ; i < shaders_id.size() ; i++) {
    glDeleteShader(shaders_id[i]);TEST_OPENGL_ERROR();
  }
  return true;
}


void test_opengl_error(std::string func, std::string file, int line) {
    GLenum err = glGetError();
    switch (err) {
      case GL_NO_ERROR: return;
      case GL_INVALID_ENUM: std::cerr << file << ":" << line << "(" << func << ") ";
                       std::cerr << "GL_INVALID_ENUM\n";
		       break;
      case GL_INVALID_VALUE: std::cerr << file << ":" << line << "(" << func << ") ";
                       std::cerr << "GL_INVALID_VALUE\n";
		       break;
      case GL_INVALID_OPERATION: std::cerr << file << ":" << line << "(" << func << ") ";
                       std::cerr << "GL_INVALID_OPERATION\n";
		       break;
      case GL_INVALID_FRAMEBUFFER_OPERATION: std::cerr << file << ":" << line << "(" << func << ") ";
                       std::cerr << "GL_INVALID_FRAMEBUFFER_OPERATION\n";
		       break;
      case GL_OUT_OF_MEMORY: std::cerr << file << ":" << line << "(" << func << ") ";
                       std::cerr << "GL_OUT_OF_MEMORY\n";
		       break;
      case GL_STACK_UNDERFLOW: std::cerr << file << ":" << line << "(" << func << ") ";
                       std::cerr << "GL_STACK_UNDERFLOW\n";
		       break;
      case GL_STACK_OVERFLOW: std::cerr << file << ":" << line << "(" << func << ") ";
                       std::cerr << "GL_STACK_OVERFLOW\n";
		       break;
      default:std::cerr << file << ":" << line << "(" << func << ") ";
                       std::cerr << "UNKONWN ERROR\n";
		       break;
    }

  }
struct shader_data
{
    int obj_hit = 0;
};

GLint cam_pos_id;
GLint cam_rot_id;
GLint time_id;
GLint canon_time_id;
GLint cam_resolution_id;
GLint mouse_pos_id;
GLint obj_selected_id;
GLint obj_1_id;
GLint obj_2_id;
GLint obj_3_id;
cam cam_;
obj obj_1;
obj obj_2;
obj obj_3;

void obj_set(obj& obj_, float x, float y, float z, float id)
{
    obj_.x = x;
    obj_.y = y;
    obj_.z = z;
    obj_.id = id;
}

void init_scene(int scene_type)
{
    cam_.pos_x = 0;
    cam_.pos_y = 0;
    cam_.pos_z = 0;
    cam_.look_at_x = 0;
    cam_.look_at_y = 0;
    cam_.look_at_z = 1;
    cam_pos_id = glGetUniformLocation(program_id, "cam_pos");TEST_OPENGL_ERROR();
    glProgramUniform3f(program_id, cam_pos_id, cam_.pos_x, cam_.pos_y, cam_.pos_z);TEST_OPENGL_ERROR();

    cam_rot_id = glGetUniformLocation(program_id, "cam_rot");TEST_OPENGL_ERROR();
    glProgramUniform3f(program_id, cam_rot_id, cam_.look_at_x, cam_.look_at_y, cam_.look_at_z);TEST_OPENGL_ERROR();

    GLint scene_type_id = glGetUniformLocation(program_id, "sceneType");TEST_OPENGL_ERROR();
    glProgramUniform1f(program_id, scene_type_id, scene_type);TEST_OPENGL_ERROR();

    time_id = glGetUniformLocation(program_id, "time");TEST_OPENGL_ERROR();
    glProgramUniform1f(program_id, time_id, glutGet(GLUT_ELAPSED_TIME));TEST_OPENGL_ERROR();

    pixWIDTH = glutGet(GLUT_WINDOW_WIDTH);
    pixHEIGHT = glutGet(GLUT_WINDOW_HEIGHT);

    cam_resolution_id = glGetUniformLocation(program_id, "u_resolution");TEST_OPENGL_ERROR();
    glProgramUniform2f(program_id, cam_resolution_id, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));TEST_OPENGL_ERROR();

    canon_time_id = glGetUniformLocation(program_id, "canon_time");TEST_OPENGL_ERROR();

    mouse_pos_id = glGetUniformLocation(program_id, "mouse_pos");TEST_OPENGL_ERROR();

    obj_selected_id = glGetUniformLocation(program_id, "obj_selected");TEST_OPENGL_ERROR();
    glProgramUniform1i(program_id, obj_selected_id, 0);TEST_OPENGL_ERROR();

    obj_1_id = glGetUniformLocation(program_id, "obj_1");TEST_OPENGL_ERROR();
    switch (scene_type)
    {
        case 0:
            obj_set(obj_1, 0.2, 0.2, 1.5, 1.0);
            break;
        case 1:
            obj_set(obj_1, 0.0, -5.0, 20.0, 1.0);
            break;
        case 2:
            obj_set(obj_1, 5.0, 5.0, 5.0, 1.0);
            break;
        case 3:
            obj_set(obj_1, 0.2, 0.2, 1.5, 1.0);
            obj_2_id = glGetUniformLocation(program_id, "obj_2");TEST_OPENGL_ERROR();
            obj_3_id = glGetUniformLocation(program_id, "obj_3");TEST_OPENGL_ERROR();
            obj_set(obj_2, 2.0, 0.2, 1.5, 2.0);
            obj_set(obj_3, -2.2, 0.0, 1.5, 3.0);
            glProgramUniform4f(program_id, obj_2_id, obj_2.x, obj_2.y, obj_2.z, obj_2.id);TEST_OPENGL_ERROR();
            glProgramUniform4f(program_id, obj_3_id, obj_3.x, obj_3.y, obj_3.z, obj_3.id);TEST_OPENGL_ERROR();
            break;
        case 4:
            obj_2_id = glGetUniformLocation(program_id, "obj_2");TEST_OPENGL_ERROR();
            obj_set(obj_1, 0., 0., 1., 1.0);
            obj_set(obj_2, -100., 50., 100., 2.0);
            glProgramUniform4f(program_id, obj_2_id, obj_2.x, obj_2.y, obj_2.z, obj_2.id);TEST_OPENGL_ERROR();
            break;
    }
    glProgramUniform4f(program_id, obj_1_id, obj_1.x, obj_1.y, obj_1.z, obj_1.id);TEST_OPENGL_ERROR();
}
void update_cam_pos()
{
    glUniform3f(cam_pos_id, cam_.pos_x, cam_.pos_y, cam_.pos_z);TEST_OPENGL_ERROR();
    glutPostRedisplay();
}
void update_cam_rot()
{
    glUniform3f(cam_rot_id, cam_.look_at_x, cam_.look_at_y, cam_.look_at_z);TEST_OPENGL_ERROR();
    glutPostRedisplay();
}
void idle()
{
    glUniform1f(time_id, glutGet(GLUT_ELAPSED_TIME));TEST_OPENGL_ERROR();
    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) {
    if (state == GLUT_DOWN)
    {
        if (button == GLUT_LEFT_BUTTON && !cam_.is_rotating)
        {
            cam_.is_rotating = true;

            cam_.last_mouse_pos_x = x;
            cam_.last_mouse_pos_y = y;
        }
        if (button == GLUT_RIGHT_BUTTON && !cam_.is_selecting)
        {
            cam_.is_selecting = true;

            cam_.last_mouse_pos_x = x;
            cam_.last_mouse_pos_y = y;
        }
    }
    if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
    {
        cam_.is_rotating = false;
    }
    if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP)
    {
        cam_.is_selecting = false;
    }
}

GLubyte *data = (GLubyte*)malloc(1);
void passiv_mouse_motion(int x, int y)
{
    int w_h = glutGet(GLUT_WINDOW_HEIGHT);
    float x_ = (float)x / (float)glutGet(GLUT_WINDOW_WIDTH);
    float y_ = (float)y / (float)w_h;
    glProgramUniform2f(program_id, mouse_pos_id, x_, y_);TEST_OPENGL_ERROR();
    if( data ) {
        glReadPixels(x, w_h - y, 1, 1, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, data);
        uint d = (uint)data[0];
        if (d == 25)
        {
            obj_1.is_selected = true;
            obj_2.is_selected = false;
            obj_3.is_selected = false;
            glProgramUniform1i(program_id, obj_selected_id, 1);TEST_OPENGL_ERROR();
        }
        else if (d == 51)
        {
            obj_1.is_selected = false;
            obj_3.is_selected = false;
            obj_2.is_selected = true;
            glProgramUniform1i(program_id, obj_selected_id, 2);TEST_OPENGL_ERROR();
        }
        else if (d == 76)
        {
            obj_1.is_selected = false;
            obj_2.is_selected = false;
            obj_3.is_selected = true;
            glProgramUniform1i(program_id, obj_selected_id, 3);TEST_OPENGL_ERROR();
        }
        else
        {
            obj_1.is_selected = false;
            obj_2.is_selected = false;
            obj_3.is_selected = false;
            glProgramUniform1i(program_id, obj_selected_id, 0);TEST_OPENGL_ERROR();
        }
    }

}

vec3 rotate_x(vec3 p, float sin, float cos)
{
    vec3 ret;
    ret.x = p.x;
    ret.y = p.y * cos + p.z * -sin;
    ret.z = p.y * sin + p.z * cos;
    return ret;
}
vec3 rotate_y(vec3 p, float sin, float cos)
{
    vec3 ret;
    ret.x = p.x * cos + p.z * sin;
    ret.y = p.y;
    ret.z = p.x * -sin + p.z * cos;
    return ret;
}
vec3 rotate_z(vec3 p, float sin, float cos)
{
    vec3 ret;
    ret.x = p.x * cos + p.y * -sin;
    ret.y = p.x * sin + p.y * cos;
    ret.z = p.z;
    return ret;
}

void move_obj(obj& obj_, GLint id, float x_diff, float y_diff)
{
            float cos_px = 0;
            float sin_px = 1;
            float cos_py = 0;
            float sin_py = 1;
            float cos_pz = 1;
            float sin_pz = 0;

            cos_px = cam_.look_at_x / 1; //cos(angle_pass_x);
            sin_px = sqrt(1 - (cam_.look_at_x * cam_.look_at_x)) / 1; //sin(angle_pass_x);

            cos_py = cam_.look_at_y / 1;//cos(angle_pass_y);
            sin_py = sqrt(1 - (cam_.look_at_y * cam_.look_at_y)) / 1;//sin(angle_pass_y);

            cos_pz = cam_.look_at_z / 1;//cos(angle_pass_z);
            sin_pz = sqrt(1 - (cam_.look_at_z * cam_.look_at_z)) / 1;//sin(angle_pass_z);

            vec3 obj;
            obj.x = obj_.x - cam_.pos_x;
            obj.y = obj_.y - cam_.pos_y;
            obj.z = obj_.z - cam_.pos_z;
            vec3 obj_r = rotate_x(rotate_y(rotate_z(obj, sin_pz, cos_pz), sin_py, cos_py), sin_px, cos_px);

            float sinx = sin(x_diff * 3.14159/2 * ((float)pixWIDTH/(float)pixHEIGHT));
            float cosx = cos(x_diff * 3.14159/2 * ((float)pixWIDTH/(float)pixHEIGHT));
            float siny = sin(y_diff * 3.14159/2);
            float cosy = cos(y_diff * 3.14159/2);

            if (obj.z < 0)
                siny = -siny;
            obj_r = rotate_z(obj_r, sinx, cosx);
            obj_r = rotate_y(obj_r, siny, cosy);
            obj_r = rotate_z(rotate_y(rotate_x(obj_r, -sin_px, cos_px), -sin_py, cos_py), -sin_pz, cos_pz);

            obj_.x = obj_r.x + cam_.pos_x;
            obj_.y = obj_r.y + cam_.pos_y;
            obj_.z = obj_r.z + cam_.pos_z;

            glProgramUniform4f(program_id, id, obj_.x, obj_.y, obj_.z, obj_.id);TEST_OPENGL_ERROR();
}

void mouse_motion(int x, int y)
{
    if (cam_.is_selecting)
    {
        float x_diff = -(cam_.last_mouse_pos_x - x) / pixWIDTH;
        float y_diff = -(cam_.last_mouse_pos_y - y) / pixHEIGHT;
        if (std::abs(x_diff) + std::abs(y_diff) > 1.0/(float)pixHEIGHT)
        {
            if (obj_1.is_selected)
                move_obj(obj_1, obj_1_id, x_diff, y_diff);
            if (obj_2.is_selected)
            {
                move_obj(obj_2, obj_2_id, x_diff, y_diff);
            }
            if (obj_3.is_selected)
                move_obj(obj_3, obj_3_id, x_diff, y_diff);
            cam_.last_mouse_pos_x = x;
            cam_.last_mouse_pos_y = y;
        }
        return;
    }
    passiv_mouse_motion(x, y);
    if (cam_.is_rotating)
    {
        float x_diff = (cam_.last_mouse_pos_x - x) / pixWIDTH;
        float y_diff = (cam_.last_mouse_pos_y - y) / pixHEIGHT;
        if (std::abs(x_diff) + std::abs(y_diff) > 1.0/(float)pixWIDTH)
        {
            float sinx = sin(x_diff * 3.14159/2 * ((float)pixWIDTH/(float)pixHEIGHT));
            float cosx = cos(x_diff * 3.14159/2 * ((float)pixWIDTH/(float)pixHEIGHT));
            float siny = sin(y_diff * 3.14159/2);
            float cosy = cos(y_diff * 3.14159/2);
            if (cam_.look_at_z < 0)
            {
                siny = -siny;
            }
            float n_look_at_x, n_look_at_y, n_look_at_z;

            //sideways rotation
            n_look_at_x = cam_.look_at_x * cosx + cam_.look_at_z * sinx;
            n_look_at_z = cam_.look_at_x * -sinx + cam_.look_at_z * cosx;

            cam_.look_at_z = n_look_at_z;
            cam_.look_at_x = n_look_at_x;

            //up down rotation
            n_look_at_y = cam_.look_at_y * cosy + cam_.look_at_z * -siny;
            n_look_at_z = cam_.look_at_y * siny + cam_.look_at_z * cosy;

            cam_.look_at_y = n_look_at_y;
            cam_.look_at_z = n_look_at_z;

            cam_.last_mouse_pos_x = x;
            cam_.last_mouse_pos_y = y;
            update_cam_rot();
        }
    }
}


float cam_mov = 0.1;
void keyboard(unsigned char key, int x, int y)
{
    (void)x;
    (void)y;
    if (key == 32) //space
    {
        cam_.pos_z += cam_mov;
        update_cam_pos();
    }
    if (key == 9) //tab
    {
        cam_.pos_z -= cam_mov;
        update_cam_pos();
    }
    if (key == 97) //a
    {
        cam_.pos_x -= cam_mov;
        update_cam_pos();
    }
    if (key == 115) //s
    {
        cam_.pos_y -= cam_mov;
        update_cam_pos();
    }
    if (key == 100) //d
    {
        cam_.pos_x += cam_mov;
        update_cam_pos();
    }
    if (key == 119) //w
    {
        cam_.pos_y += cam_mov;
        update_cam_pos();
    }
    if (key == 114) //r
    {
        pixWIDTH = glutGet(GLUT_WINDOW_WIDTH);
        pixHEIGHT = glutGet(GLUT_WINDOW_HEIGHT);

        glProgramUniform2f(program_id, cam_resolution_id, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));TEST_OPENGL_ERROR();
        glutPostRedisplay();
    }
    if (key == 102) //f
    {
        glUniform1f(canon_time_id, glutGet(GLUT_ELAPSED_TIME));TEST_OPENGL_ERROR();
    }
}

int main(int argc, char *argv[]) {
    int scene_type = 0;
    if (argc > 1)
    {
        if (strcmp(argv[1], "1") == 0)
            scene_type = 1;
        if (strcmp(argv[1], "2") == 0)
            scene_type = 2;
        if (strcmp(argv[1], "3") == 0)
            scene_type = 3;
        if (strcmp(argv[1], "4") == 0)
            scene_type = 4;
    }
  init_glut(argc, argv);
  if (!init_glew())
    std::exit(-1);
  init_GL();
  init_shaders();
  init_scene(scene_type);
  glutIdleFunc(idle);
  glutMouseFunc(mouse);
  glutMotionFunc(mouse_motion);
  glutPassiveMotionFunc(passiv_mouse_motion);
  glutKeyboardFunc(keyboard);
  glutMainLoop();
}
