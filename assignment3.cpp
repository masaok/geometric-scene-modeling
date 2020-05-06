/***
 Assignment-3: Geometric Modeling of a Scene

 Name: Wong, Alex (Please write your name in Last Name, First Name format)

 Collaborators: Doe, John; Doe, Jane
 ** Note: although the assignment should be completed individually
 you may speak with classmates on high level algorithmic concepts. Please
 list their names in this section

 Project Summary: A short paragraph (3-4 sentences) describing the work you
 did for the project.
 ***/

#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

#ifdef __APPLE__
  #include <GLUT/glut.h>
  #include <OpenGL/gl.h>
  #include <OpenGL/glu.h>
#else
  #include <GL/gl.h>
  #include <GL/glu.h>
  #include <GL/glut.h>
#endif

#include <math.h>
#include <stdlib.h>

#include <iostream>
#include <vector>
using namespace std;

// If a float is < EPSILON or > -EPILSON then it should be 0
float EPSILON = 0.000001;
// theta is the angle to rotate the scene
float THETA = 0.0;
// Vector placeholders for the scene and color array
vector<GLfloat> SCENE;
vector<GLfloat> COLOR;

/**************************************************
 *  Rectangular Prisms via Hierarchical Modeling  *
 *                                                *
 *  using planes as building blocks, build a unit *
 *  cube with transformations that will serve as  *
 *  a primitive for modeling objects in the scene *
 *                                                *
 *************************************************/

// Variable debug convenience function
// Usage: debug(__LINE__, "some label", the_variable);
void debug(int line, string label, int value, string end = "" ) {
    cout << line <<  ": " << label << ": " << value << end << endl << flush;
}

// Vector dump convenience function
string dump(vector<GLfloat> points, int columns = 4) {
  string str = "[\n";
  for (int i = 0; i < points.size(); i++) {
    GLfloat v = points[i];
    str += "\t";
    str += to_string(v);
    if ((i + 1) % columns == 0) {
        str += "\n";
    }
  }
  str += "]\n";
  return str;
}

// Initializes a square plane of unit lengths
vector<GLfloat> init_plane() {
  vector<GLfloat> vertices = {
      +0.5, +0.5, +0.0,
      -0.5, +0.5, +0.0,
      -0.5, -0.5, +0.0,
      +0.5, -0.5, +0.0};
  return vertices;
}

// Converts degrees to radians for rotation
float deg2rad(float d) {
  return (d * M_PI) / 180.0;
}

// Converts a vector to an array
GLfloat *vector2array(vector<GLfloat> vec) {
  GLfloat *arr = new GLfloat[vec.size()];
  for (int i = 0; i < vec.size(); i++) {
    arr[i] = vec[i];
  }
  return arr;
}

// Converts Cartesian coordinates to homogeneous coordinates
vector<GLfloat> to_homogeneous_coord(vector<GLfloat> cartesian_coords) {
  vector<GLfloat> result;
  for (int i = 0; i < cartesian_coords.size(); i++) {
    result.push_back(cartesian_coords[i]);
    if ((i + 1) % 3 == 0) {
      result.push_back(1.0);
    }
  }
  return result;
}

// Converts Cartesian coordinates to homogeneous coordinates
vector<GLfloat> to_cartesian_coord(vector<GLfloat> homogeneous_coords) {
  vector<GLfloat> result;
  for (int i = 0; i < homogeneous_coords.size(); i++) {
    if ((i + 1) % 4 == 0) {
      continue;
    } else {
      result.push_back(homogeneous_coords[i]);
    }
  }
  return result;
}

// Definition of a translation matrix
vector<GLfloat> translation_matrix(float dx, float dy, float dz) {
  vector<GLfloat> translate_mat;

  translate_mat.push_back(1.0);
  translate_mat.push_back(0.0);
  translate_mat.push_back(0.0);
  translate_mat.push_back(dx);

  translate_mat.push_back(0.0);
  translate_mat.push_back(1.0);
  translate_mat.push_back(0.0);
  translate_mat.push_back(dy);

  translate_mat.push_back(0.0);
  translate_mat.push_back(0.0);
  translate_mat.push_back(1.0);
  translate_mat.push_back(dz);

  translate_mat.push_back(0.0);
  translate_mat.push_back(0.0);
  translate_mat.push_back(0.0);
  translate_mat.push_back(1.0);

  return translate_mat;
}

// Definition of a scaling matrix
vector<GLfloat> scaling_matrix(float sx, float sy, float sz) {
  vector<GLfloat> scale_mat;

  scale_mat.push_back(sx);
  scale_mat.push_back(0.0);
  scale_mat.push_back(0.0);
  scale_mat.push_back(0.0);

  scale_mat.push_back(0.0);
  scale_mat.push_back(sy);
  scale_mat.push_back(0.0);
  scale_mat.push_back(0.0);

  scale_mat.push_back(0.0);
  scale_mat.push_back(0.0);
  scale_mat.push_back(sz);
  scale_mat.push_back(0.0);

  scale_mat.push_back(0.0);
  scale_mat.push_back(0.0);
  scale_mat.push_back(0.0);
  scale_mat.push_back(1.0);

  return scale_mat;
}

// Definition of a rotation matrix about the x-axis theta degrees
vector<GLfloat> rotation_matrix_x (float theta) {
    vector<GLfloat> rotate_mat_x;

    GLfloat cos_theta = cos(deg2rad(theta));
    GLfloat sin_theta = sin(deg2rad(theta));

    rotate_mat_x.push_back(1.0);
    rotate_mat_x.push_back(0.0);
    rotate_mat_x.push_back(0.0);
    rotate_mat_x.push_back(0.0);

    rotate_mat_x.push_back(0.0);
    rotate_mat_x.push_back(cos_theta);
    rotate_mat_x.push_back(sin_theta * -1.0);
    rotate_mat_x.push_back(0.0);

    rotate_mat_x.push_back(0.0);
    rotate_mat_x.push_back(sin_theta);
    rotate_mat_x.push_back(cos_theta);
    rotate_mat_x.push_back(0.0);

    rotate_mat_x.push_back(0.0);
    rotate_mat_x.push_back(0.0);
    rotate_mat_x.push_back(0.0);
    rotate_mat_x.push_back(1.0);

    return rotate_mat_x;
}

// Definition of a rotation matrix along the y-axis by theta degrees
vector<GLfloat> rotation_matrix_y (float theta) {
    vector<GLfloat> rotate_mat_y;

    GLfloat cos_theta = cos(deg2rad(theta));
    GLfloat sin_theta = sin(deg2rad(theta));

    // Define the rotation matrix about the y-axis
    rotate_mat_y.push_back(cos_theta);
    rotate_mat_y.push_back(0.0);
    rotate_mat_y.push_back(sin_theta);
    rotate_mat_y.push_back(0.0);

    rotate_mat_y.push_back(0.0);
    rotate_mat_y.push_back(1.0);
    rotate_mat_y.push_back(0.0);
    rotate_mat_y.push_back(0.0);

    rotate_mat_y.push_back(sin_theta * -1.0);
    rotate_mat_y.push_back(0.0);
    rotate_mat_y.push_back(cos_theta);
    rotate_mat_y.push_back(0.0);

    rotate_mat_y.push_back(0.0);
    rotate_mat_y.push_back(0.0);
    rotate_mat_y.push_back(0.0);
    rotate_mat_y.push_back(1.0);

    return rotate_mat_y;
}

// Definition of a rotation matrix along the z-axis by theta degrees
vector<GLfloat> rotation_matrix_z (float theta) {
    vector<GLfloat> rotate_mat_z;

    GLfloat cos_theta = cos(deg2rad(theta));
    GLfloat sin_theta = sin(deg2rad(theta));

    // Define the rotation matrix about the z-axis
    rotate_mat_z.push_back(cos_theta);
    rotate_mat_z.push_back(sin_theta * -1.0);
    rotate_mat_z.push_back(0.0);
    rotate_mat_z.push_back(0.0);

    rotate_mat_z.push_back(sin_theta);
    rotate_mat_z.push_back(cos_theta);
    rotate_mat_z.push_back(0.0);
    rotate_mat_z.push_back(0.0);

    rotate_mat_z.push_back(0.0);
    rotate_mat_z.push_back(0.0);
    rotate_mat_z.push_back(1.0);
    rotate_mat_z.push_back(0.0);

    rotate_mat_z.push_back(0.0);
    rotate_mat_z.push_back(0.0);
    rotate_mat_z.push_back(0.0);
    rotate_mat_z.push_back(1.0);

    return rotate_mat_z;
}

// Perform matrix multiplication for A B
vector<GLfloat> mat_mult(vector<GLfloat> A, vector<GLfloat> B) {
  vector<GLfloat> result;
  for (int i = 0; i < B.size(); i += 4) {
    for (int j = 0; j < A.size(); j += 4) {
      GLfloat dot_product = 0.0;
      for (int k = 0; k < 4; k++) {
        dot_product += A[j + k] * B[i + k];
      }
      result.push_back(dot_product);
    }
  }
  return result;
}

// Builds a unit cube centered at the origin
vector<GLfloat> build_cube() {
  vector<GLfloat> result;

  // Front plane
  vector<GLfloat> front_plane = init_plane();

  // Convert to homogeneous coordinates
  vector<GLfloat> front_hom = to_homogeneous_coord(front_plane);

  // Create the translation matrix (move it forward)
  vector<GLfloat> trans_mat = translation_matrix(0.0, 0.0, 0.5);

  // Matrix mutiply by the translation matrix
  vector<GLfloat> front_final = mat_mult(trans_mat, front_hom);

  // Back plane
  vector<GLfloat> back_plane = init_plane();
  vector<GLfloat> back_hom = to_homogeneous_coord(back_plane);

  // Back plane rotate
  vector<GLfloat> rot_y = rotation_matrix_y(180);
  vector<GLfloat> back_rotated = mat_mult(rot_y, back_hom);

  // Back plane move backward
  vector<GLfloat> back_trans = translation_matrix(0.0, 0.0, -0.5);
  vector<GLfloat> back_final = mat_mult(back_trans, back_rotated);

  // West plane
  vector<GLfloat> west_plane = init_plane();
  vector<GLfloat> west_hom = to_homogeneous_coord(west_plane);

  // West plane rotate
  vector<GLfloat> west_rot_y = rotation_matrix_y(-90);
  vector<GLfloat> west_rotated = mat_mult(west_rot_y, west_hom);

  // West plane move westward
  vector<GLfloat> west_trans = translation_matrix(-0.5, 0.0, 0.0);
  vector<GLfloat> west_final = mat_mult(west_trans, west_rotated);

  // East plane
  vector<GLfloat> east_plane = init_plane();
  vector<GLfloat> east_hom = to_homogeneous_coord(east_plane);

  // East plane rotate
  vector<GLfloat> east_rot_y = rotation_matrix_y(90);
  vector<GLfloat> east_rotated = mat_mult(east_rot_y, east_hom);

  // East plane move eastward
  vector<GLfloat> east_trans = translation_matrix(0.5, 0.0, 0.0);
  vector<GLfloat> east_final = mat_mult(east_trans, east_rotated);

  // South plane
  vector<GLfloat> south_plane = init_plane();
  vector<GLfloat> south_hom = to_homogeneous_coord(south_plane);

  // South plane rotate
  vector<GLfloat> south_rot_y = rotation_matrix_x(90);
  vector<GLfloat> south_rotated = mat_mult(south_rot_y, south_hom);

  // South plane move southward
  vector<GLfloat> south_trans = translation_matrix(0.0, -0.5, 0.0);
  vector<GLfloat> south_final = mat_mult(south_trans, south_rotated);

  // North plane
  vector<GLfloat> north_plane = init_plane();
  vector<GLfloat> north_hom = to_homogeneous_coord(north_plane);

  // North plane rotate
  vector<GLfloat> north_rot_y = rotation_matrix_x(-90);
  vector<GLfloat> north_rotated = mat_mult(north_rot_y, north_hom);

  // North plane move northward
  vector<GLfloat> north_trans = translation_matrix(0.0, 0.5, 0.0);
  vector<GLfloat> north_final = mat_mult(north_trans, north_rotated);

  // Concatenate all planes
  vector<GLfloat> concat;
  concat.insert( concat.end(), front_final.begin(), front_final.end() );
  concat.insert( concat.end(), back_final.begin(), back_final.end() );
  concat.insert( concat.end(), west_final.begin(), west_final.end() );
  concat.insert( concat.end(), east_final.begin(), east_final.end() );
  concat.insert( concat.end(), south_final.begin(), south_final.end() );
  concat.insert( concat.end(), north_final.begin(), north_final.end() );

  // Return in cartesian coordinates
  return to_cartesian_coord(concat);
}

// Builds a unit cube centered at the origin
vector<GLfloat> build_table() {
  vector<GLfloat> result;

  vector<GLfloat> top = build_cube();
  vector<GLfloat> top_hom = to_homogeneous_coord(top);
  vector<GLfloat> scale_mat = scaling_matrix(1, 0.1, 1);
  vector<GLfloat> top_scaled = mat_mult(scale_mat, top_hom);

  // Southwest Leg
  vector<GLfloat> sw_leg = build_cube();
  vector<GLfloat> sw_leg_hom = to_homogeneous_coord(sw_leg);
  vector<GLfloat> leg_scale_mat = scaling_matrix(0.1, 2, 0.1);
  vector<GLfloat> sw_leg_scaled = mat_mult(leg_scale_mat, sw_leg_hom);

  // Southwest Leg Translate
  vector<GLfloat> sw_leg_trans = translation_matrix(-0.4, -1, 0.4);
  vector<GLfloat> sw_leg_moved = mat_mult(sw_leg_trans, sw_leg_scaled);
  sw_leg = sw_leg_moved;

  // Southeast Leg
  vector<GLfloat> se_leg = build_cube();
  vector<GLfloat> se_leg_hom = to_homogeneous_coord(se_leg);
  vector<GLfloat> se_leg_scaled = mat_mult(leg_scale_mat, se_leg_hom);

  // Southeast Leg Translate
  vector<GLfloat> se_leg_trans = translation_matrix(0.4, -1, 0.4);
  vector<GLfloat> se_leg_moved = mat_mult(se_leg_trans, se_leg_scaled);
  se_leg = se_leg_moved;

  // Northwest Leg
  vector<GLfloat> nw_leg = build_cube();
  vector<GLfloat> nw_leg_hom = to_homogeneous_coord(nw_leg);
  vector<GLfloat> nw_leg_scaled = mat_mult(leg_scale_mat, nw_leg_hom);
  vector<GLfloat> nw_leg_trans = translation_matrix(-0.4, -1, -0.4);
  vector<GLfloat> nw_leg_moved = mat_mult(nw_leg_trans, nw_leg_scaled);
  nw_leg = nw_leg_moved;

  // Northeast Leg
  vector<GLfloat> ne_leg = build_cube();
  vector<GLfloat> ne_leg_hom = to_homogeneous_coord(ne_leg);
  vector<GLfloat> ne_leg_scaled = mat_mult(leg_scale_mat, ne_leg_hom);
  vector<GLfloat> ne_leg_trans = translation_matrix(0.4, -1, -0.4);
  vector<GLfloat> ne_leg_moved = mat_mult(ne_leg_trans, ne_leg_scaled);
  ne_leg = ne_leg_moved;

  // Concatenate all parts
  vector<GLfloat> concat;
  concat.insert( concat.end(), top_scaled.begin(), top_scaled.end() );
  concat.insert( concat.end(), sw_leg.begin(), sw_leg.end() );
  concat.insert( concat.end(), se_leg.begin(), se_leg.end() );
  concat.insert( concat.end(), nw_leg.begin(), nw_leg.end() );
  concat.insert( concat.end(), ne_leg.begin(), ne_leg.end() );

  // result = to_cartesian_coord(concat);
  return concat;
}

/**************************************************
 *            Camera and World Modeling           *
 *                                                *
 *  create a scene by applying transformations to *
 *  the objects built from planes and position    *
 *  the camera to view the scene by setting       *
 *  the projection/viewing matrices               *
 *                                                *
 *************************************************/

void setup() {
  // Enable the vertex array functionality
  glEnableClientState(GL_VERTEX_ARRAY);
  // Enable the color array functionality (so we can specify a color for each vertex)
  glEnableClientState(GL_COLOR_ARRAY);
  // Enable depth test
  glEnable(GL_DEPTH_TEST);
  // Accept fragment if it closer to the camera than the former one
  glDepthFunc(GL_LESS);
  // Set up some default base color
  glColor3f(0.5, 0.5, 0.5);
  // Set up white background
  glClearColor(1.0, 1.0, 1.0, 0.0);
}

void init_camera() {
  // Camera parameters
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  // TODO: Setup your camera here
  // Define a 50 degree field of view, 1:1 aspect ratio, near and far planes at 3 and 7
  gluPerspective(50.0, 1.0, 2.0, 10.0);

  // Position camera at (2, 3, 5), attention at (0, 0, 0), up at (0, 1, 0)
  gluLookAt(2.0, 6.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

  // TODO: doubling the position does NOT work
  // gluLookAt(4.0, 12.0, 10.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

// Construct the scene using objects built from cubes/prisms
vector<GLfloat> init_scene() {
  vector<GLfloat> scene;

  // TODO: Build your scene here
  vector<GLfloat> cube = build_cube();
  vector<GLfloat> cube_hom = to_homogeneous_coord(cube);
  vector<GLfloat> scale_mat = scaling_matrix(2, 1, 1);
  vector<GLfloat> cube_scaled = mat_mult(scale_mat, cube_hom);

  scene = to_cartesian_coord(cube_scaled);

  vector<GLfloat> table_orig = build_table();
  table_orig = to_cartesian_coord(table_orig);

  vector<GLfloat> table = build_table();
  vector<GLfloat> table_trans = translation_matrix(-1.5, 0, 1.5);
  table = mat_mult(table_trans, table);
  table = to_cartesian_coord(table);

  // Concatenate all objects
  vector<GLfloat> concat;
  concat.insert( concat.end(), table_orig.begin(), table_orig.end() );
  concat.insert( concat.end(), table.begin(), table.end() );

  return concat;
}

// Construct the color mapping of the scene
vector<GLfloat> init_color(vector<GLfloat> scene) {
  vector<GLfloat> colors;
  for (int i = 0; i < scene.size(); i++) {
    colors.push_back(static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
  }
  return colors;
}

void display_func() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  // TODO: Rotate the scene using the scene vector
  vector<GLfloat> scene = SCENE;
  GLfloat* scene_vertices = vector2array(scene);
  GLfloat* color_vertices = vector2array(COLOR);
  // Pass the scene vertex pointer
  glVertexPointer(3,                // 3 components (x, y, z)
                  GL_FLOAT,         // Vertex type is GL_FLOAT
                  0,                // Start position in referenced memory
                  scene_vertices);  // Pointer to memory location to read from
  // Pass the color vertex pointer
  glColorPointer(3,                   // 3 components (r, g, b)
                  GL_FLOAT,            // Vertex type is GL_FLOAT
                  0,                   // Start position in referenced memory
                  color_vertices);     // Pointer to memory location to read from
  // Draw quad point planes: each 4 vertices
  glDrawArrays(GL_QUADS, 0, scene.size()/3.0);
  glFlush();			//Finish rendering
  glutSwapBuffers();
}

void idle_func() {
  THETA = THETA + 0.3;
  display_func();
}

int main(int argc, char **argv) {
  vector<GLfloat> west_rot_y = rotation_matrix_y(45);
  // cout << dump(west_rot_y);

  // debug(__LINE__, "blah", 3);


  // Initialize GLUT
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowSize(800, 600);
  // Create a window with rendering context and everything else we need
  glutCreateWindow("Assignment 3");

  setup();
  init_camera();
  // Setting global variables SCENE and COLOR with actual values
  SCENE = init_scene();
  COLOR = init_color(SCENE);

  // Set up our display function
  glutDisplayFunc(display_func);
  glutIdleFunc(idle_func);
  // Render our world
  glutMainLoop();

  // Remember to call "delete" on your dynmically allocated arrays
  // such that you don't suffer from memory leaks. e.g.
  // delete arr;

  return 0;
}
