// Source file for the scene viewer program



// Include files 

#include "R3Graphics/R3Graphics.h"
#include "fglut/fglut.h"
#include "unistd.h"
#include "p5d.h"



// Program variables

static const char *input_project_name = NULL;
static const char *input_data_directory = "../..";
static const char *input_trajectory_name = NULL;
static const char *output_color_directory = NULL;
static const char *output_depth_directory = NULL;
static R3Vector initial_camera_towards(-0.57735, -0.57735, -0.57735);
static R3Vector initial_camera_up(-0.57735, 0.57735, 0.5773);
static R3Point initial_camera_origin(0,0,0);
static RNBoolean initial_camera = FALSE;
static int print_verbose = 0;
static int print_debug = 0;



// GLUT variables 

static int GLUTwindow = 0;
static int GLUTwindow_height = 800;
static int GLUTwindow_width = 800;
static int GLUTmouse[2] = { 0, 0 };
static int GLUTbutton[3] = { 0, 0, 0 };
static int GLUTmouse_drag = 0;
static int GLUTmodifiers = 0;



// Application variables

static P5DProject *project = NULL;
static R3Scene *scene = NULL;
static R3Viewer *viewer = NULL;
static RNArray<R3Camera *> trajectory;
static R3SceneNode *selected_node = NULL;
static R3Point center(0, 0, 0);



// Display variables

static int show_faces = 1;
static int show_edges = 0;
static int show_bboxes = 0;
static int show_camera = 0;
static int show_trajectory = 0;
static int show_backfacing = 1;



////////////////////////////////////////////////////////////////////////
// Capture functions
////////////////////////////////////////////////////////////////////////

static void 
CaptureDepth(R2Grid& image)
{
  // Get viewport dimensions
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);

  // Get modelview  matrix
  static GLdouble modelview_matrix[16];
  // glGetDoublev(GL_MODELVIEW_MATRIX, modelview_matrix);
  for (int i = 0; i < 16; i++) modelview_matrix[i] = 0;
  modelview_matrix[0] = 1.0;
  modelview_matrix[5] = 1.0;
  modelview_matrix[10] = 1.0;
  modelview_matrix[15] = 1.0;
  
  // Get projection matrix
  GLdouble projection_matrix[16];
  glGetDoublev(GL_PROJECTION_MATRIX, projection_matrix);

  // Get viewpoint matrix
  GLint viewport_matrix[16];
  glGetIntegerv(GL_VIEWPORT, viewport_matrix);

  // Allocate pixels
  float *pixels = new float [ image.NEntries() ];

  // Read pixels from frame buffer 
  glReadPixels(0, 0, viewport[2], viewport[3], GL_DEPTH_COMPONENT, GL_FLOAT, pixels); 

  // Resize image
  image.Resample(viewport[2], viewport[3]);
  image.Clear(0.0);
  
  // Convert pixels to depths
  int ix, iy;
  double x, y, z;
  for (int i = 0; i < image.NEntries(); i++) {
    if (RNIsEqual(pixels[i], 1.0)) continue;
    if (RNIsNegativeOrZero(pixels[i])) continue;
    image.IndexToIndices(i, ix, iy);
    gluUnProject(ix, iy, pixels[i], modelview_matrix, projection_matrix, viewport_matrix, &x, &y, &z);
    image.SetGridValue(i, -z);
  }

  // Delete pixels
  delete [] pixels;
}



////////////////////////////////////////////////////////////////////////
// Draw functions
////////////////////////////////////////////////////////////////////////

#if 0

static void 
DrawText(const R3Point& p, const char *s)
{
  // Draw text string s and position p
  glRasterPos3d(p[0], p[1], p[2]);
  while (*s) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *(s++));
}
  


static void 
DrawText(const R2Point& p, const char *s)
{
  // Draw text string s and position p
  R3Ray ray = viewer->WorldRay((int) p[0], (int) p[1]);
  R3Point position = ray.Point(2 * viewer->Camera().Near());
  glRasterPos3d(position[0], position[1], position[2]);
  while (*s) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *(s++));
}

#endif



static void 
DrawCamera(R3Scene *scene)
{
  // Draw view frustum
  const R3Camera& camera = scene->Camera();
  R3Point eye = camera.Origin();
  R3Vector towards = camera.Towards();
  R3Vector up = camera.Up();
  R3Vector right = camera.Right();
  RNAngle xfov = camera.XFOV();
  RNAngle yfov = camera.YFOV();
  double radius = scene->BBox().DiagonalRadius();
  R3Point org = eye + towards * radius;
  R3Vector dx = right * radius * tan(xfov);
  R3Vector dy = up * radius * tan(yfov);
  R3Point ur = org + dx + dy;
  R3Point lr = org + dx - dy;
  R3Point ul = org - dx + dy;
  R3Point ll = org - dx - dy;
  glBegin(GL_LINE_LOOP);
  glVertex3d(ur[0], ur[1], ur[2]);
  glVertex3d(ul[0], ul[1], ul[2]);
  glVertex3d(ll[0], ll[1], ll[2]);
  glVertex3d(lr[0], lr[1], lr[2]);
  glVertex3d(ur[0], ur[1], ur[2]);
  glVertex3d(eye[0], eye[1], eye[2]);
  glVertex3d(lr[0], lr[1], lr[2]);
  glVertex3d(ll[0], ll[1], ll[2]);
  glVertex3d(eye[0], eye[1], eye[2]);
  glVertex3d(ul[0], ul[1], ul[2]);
  glEnd();
}



static void 
DrawTrajectory(const RNArray<R3Camera *>& trajectory)
{
  // Check trajectory
  if (trajectory.IsEmpty()) return;
  
  // Draw viewpoints
  glPointSize(3);
  glBegin(GL_POINTS);
  for (int i = 0; i < trajectory.NEntries(); i++) 
    R3LoadPoint(trajectory[i]->Origin());
  glEnd();
  glPointSize(1);

  // Draw curve
  glBegin(GL_LINE_STRIP);
  for (int i = 0; i < trajectory.NEntries(); i++) 
    R3LoadPoint(trajectory[i]->Origin());
  glEnd();
}



static void 
DrawShapes(R3Scene *scene, R3SceneNode *node, RNFlags draw_flags = R3_DEFAULT_DRAW_FLAGS)
{
  // Push transformation
  node->Transformation().Push();

  // Draw elements
  if (node == selected_node) {
    // Draw elements with selected material
    for (int i = 0; i < node->NElements(); i++) {
      R3SceneElement *element = node->Element(i);
      R3Material *material = element->Material();
      static R3Material *selection_material = new R3Material(&R3red_brdf, "Selection");
      element->SetMaterial(selection_material);
      element->Draw();
      element->SetMaterial(material);
    }
  }
  else {
    // Draw elements with original materials
    for (int i = 0; i < node->NElements(); i++) {
      R3SceneElement *element = node->Element(i);
      element->Draw(draw_flags);
    }
  }

  // Draw children
  for (int i = 0; i < node->NChildren(); i++) {
    R3SceneNode *child = node->Child(i);
    DrawShapes(scene, child, draw_flags);
  }

  // Pop transformation
  node->Transformation().Pop();
}



static void 
DrawBBoxes(R3Scene *scene, R3SceneNode *node)
{
  // Draw node bounding box
  node->BBox().Outline();

  // Push transformation
  node->Transformation().Push();

  // Draw children bboxes
  for (int i = 0; i < node->NChildren(); i++) {
    R3SceneNode *child = node->Child(i);
    DrawBBoxes(scene, child);
  }

  // Pop transformation
  node->Transformation().Pop();
}



////////////////////////////////////////////////////////////////////////
// Glut user interface functions
////////////////////////////////////////////////////////////////////////

void GLUTStop(void)
{
  // Destroy window 
  glutDestroyWindow(GLUTwindow);

  // Exit
  exit(0);
}



void GLUTRedraw(void)
{
  // Check scene
  if (!scene) return;

  // Set viewing transformation
  viewer->Camera().Load();

  // Clear window 
  RNRgb background = scene->Background();
  glClearColor(background.R(), background.G(), background.B(), 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Set backface culling
  if (show_backfacing) glDisable(GL_CULL_FACE);
  else glEnable(GL_CULL_FACE);

  // Set lights
  static GLfloat light0_position[] = { 3.0, 4.0, 5.0, 0.0 };
  glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
  static GLfloat light1_position[] = { -3.0, -2.0, -3.0, 0.0 };
  glLightfv(GL_LIGHT1, GL_POSITION, light1_position);

  // Draw camera
  if (show_camera) {
    glDisable(GL_LIGHTING);
    glColor3d(1.0, 1.0, 1.0);
    glLineWidth(5);
    DrawCamera(scene);
    glLineWidth(1);
  }

  // Draw trajectory
  if (show_trajectory) {
    glDisable(GL_LIGHTING);
    glColor3d(0.0, 1.0, 1.0);
    DrawTrajectory(trajectory);
  }

  // Draw faces
  if (show_faces) {
    glEnable(GL_LIGHTING);
    R3null_material.Draw();
    DrawShapes(scene, scene->Root());
    R3null_material.Draw();
  }

  // Draw edges
  if (show_edges) {
    glDisable(GL_LIGHTING);
    R3null_material.Draw();
    glColor3d(0.0, 1.0, 0.0);
    R3null_material.Draw();
    DrawShapes(scene, scene->Root(), R3_EDGES_DRAW_FLAG);
    R3null_material.Draw();
  }

  // Draw bboxes
  if (show_bboxes) {
    glDisable(GL_LIGHTING);
    glColor3d(1.0, 0.0, 0.0);
    DrawBBoxes(scene, scene->Root());
  }

  // Capture depth image
  if (output_depth_directory) {
    static int counter = 0;
    R2Grid image(GLUTwindow_width, GLUTwindow_height);
    CaptureDepth(image);
    char output_depth_filename[1024];
    sprintf(output_depth_filename, "%s/d%06d.raw", output_depth_directory, counter++);
    image.WriteFile(output_depth_filename);
  }

  // Capture color image
  if (output_color_directory) {
    static int counter = 0;
    R2Image image(GLUTwindow_width, GLUTwindow_height, 3);
    image.Capture();
    char output_color_filename[1024];
    sprintf(output_color_filename, "%s/c%06d.jpg", output_color_directory, counter++);
    image.Write(output_color_filename);
  }

#if 0
  // Print current camera in format for trajectory file
  R3Camera c = viewer->Camera();
  R3Point v = c.Origin();
  R3Vector t = c.Towards();
  R3Vector u = c.Up();
  printf("%g %g %g %g %g %g %g %g %g\n", v[0], v[1], v[2], t[0], t[1], t[2], u[0], u[1], u[2]);
#endif
  
  // Swap buffers 
  glutSwapBuffers();
}    



void GLUTResize(int w, int h)
{
  // Resize window
  glViewport(0, 0, w, h);

  // Resize viewer viewport
  viewer->ResizeViewport(0, 0, w, h);

  // Resize scene viewport
  scene->SetViewport(viewer->Viewport());

  // Remember window size 
  GLUTwindow_width = w;
  GLUTwindow_height = h;

  // Redraw
  glutPostRedisplay();
}



void GLUTMotion(int x, int y)
{
  // Invert y coordinate
  y = GLUTwindow_height - y;

  // Compute mouse movement
  int dx = x - GLUTmouse[0];
  int dy = y - GLUTmouse[1];
  
  // Update mouse drag
  GLUTmouse_drag += dx*dx + dy*dy;

  // World in hand navigation 
  if (GLUTbutton[0]) viewer->RotateWorld(1.0, center, x, y, dx, dy);
  else if (GLUTbutton[1]) viewer->ScaleWorld(1.0, center, x, y, dx, dy);
  else if (GLUTbutton[2]) viewer->TranslateWorld(1.0, center, x, y, dx, dy);
  if (GLUTbutton[0] || GLUTbutton[1] || GLUTbutton[2]) glutPostRedisplay();

  // Remember mouse position 
  GLUTmouse[0] = x;
  GLUTmouse[1] = y;
}



void GLUTMouse(int button, int state, int x, int y)
{
  // Invert y coordinate
  y = GLUTwindow_height - y;

  // Mouse is going down
  if (state == GLUT_DOWN) {
    // Reset mouse drag
    GLUTmouse_drag = 0;
  }
  else {
    // Check for double click  
    static RNBoolean double_click = FALSE;
    static RNTime last_mouse_up_time;
    double_click = (!double_click) && (last_mouse_up_time.Elapsed() < 0.4);
    last_mouse_up_time.Read();

    // Check for click (rather than drag)
    if (GLUTmouse_drag < 100) {
      // Check for double click
      if (double_click) {
        // Set viewing center point 
        R3Ray ray = viewer->WorldRay(x, y);
        R3Point intersection_point;
        if (scene->Intersects(ray, NULL, NULL, NULL, &intersection_point)) {
          center = intersection_point;
        }
      }
      else {
        // Select node 
        selected_node = NULL;;
        R3Ray ray = viewer->WorldRay(x, y);
        R3Point selected_position;
        if (scene->Intersects(ray, &selected_node, NULL, NULL, &selected_position)) {
          printf("Selected (%g,%g,%g): ", selected_position.X(), selected_position.Y(), selected_position.Z());
          R3SceneNode *node = selected_node;
          while (node) { printf("%s, ", node->Name()); node = node->Parent(); }
          printf("\n");
        }
      }
    }
  }

  // Remember button state 
  int b = (button == GLUT_LEFT_BUTTON) ? 0 : ((button == GLUT_MIDDLE_BUTTON) ? 1 : 2);
  GLUTbutton[b] = (state == GLUT_DOWN) ? 1 : 0;

  // Remember modifiers 
  GLUTmodifiers = glutGetModifiers();

   // Remember mouse position 
  GLUTmouse[0] = x;
  GLUTmouse[1] = y;

  // Redraw
  glutPostRedisplay();
}



void GLUTSpecial(int key, int x, int y)
{
  // Invert y coordinate
  y = GLUTwindow_height - y;

  // Process keyboard button event 

  // Remember mouse position 
  GLUTmouse[0] = x;
  GLUTmouse[1] = y;

  // Remember modifiers 
  GLUTmodifiers = glutGetModifiers();

  // Redraw
  glutPostRedisplay();
}



void GLUTKeyboard(unsigned char key, int x, int y)
{
  // Process keyboard button event 
  switch (key) {
  case 'B':
  case 'b':
    show_backfacing = !show_backfacing;
    break;

  case 'C':
  case 'c':
    show_camera = !show_camera;
    break;

  case 'E':
  case 'e':
    show_edges = !show_edges;
    break;

  case 'F':
  case 'f':
    show_faces = !show_faces;
    break;

  case 'H':
  case 'h':
    show_bboxes = !show_bboxes;
    break;

  case 'T':
  case 't':
    show_trajectory = !show_trajectory;
    break;

  case 27: // ESCAPE
    GLUTStop();
    break;
  }

  // Remember mouse position 
  GLUTmouse[0] = x;
  GLUTmouse[1] = GLUTwindow_height - y;

  // Remember modifiers 
  GLUTmodifiers = glutGetModifiers();

  // Redraw
  glutPostRedisplay();  
}



void GLUTIdle(void)
{
  // Check trajectory
  if (trajectory.IsEmpty()) return;

  // Check trajectory index
  static int trajectory_index = 0;
  if (trajectory_index >= trajectory.NEntries()) {
    trajectory_index = 0; 
    glutIdleFunc(NULL);
    return;
  }
  
  // Update viewer
  viewer->SetCamera(*trajectory[trajectory_index]);
  
  // Redraw
  glutPostRedisplay();

  // Go to next trajectory index
  trajectory_index++;
}



void GLUTInit(int *argc, char **argv)
{
  // Open window 
  glutInit(argc, argv);
  glutInitWindowPosition(100, 100);
  glutInitWindowSize(GLUTwindow_width, GLUTwindow_height);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); // | GLUT_STENCIL
  GLUTwindow = glutCreateWindow("Property Viewer");

  // Initialize lighting
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  static GLfloat lmodel_ambient[] = { 0.2, 0.2, 0.2, 1.0 };
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
  glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
  glEnable(GL_NORMALIZE);
  glEnable(GL_LIGHTING); 

  // Initialize stationary lights
  static GLfloat light0_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
  static GLfloat light1_diffuse[] = { 0.5, 0.5, 0.5, 1.0 };
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHT1);

  // Initialize headlight
  static GLfloat light2_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
  static GLfloat light2_position[] = { 0.0, 0.0, 1.0, 0.0 };
  glLightfv(GL_LIGHT2, GL_DIFFUSE, light2_diffuse);
  glLightfv(GL_LIGHT2, GL_POSITION, light2_position);
  glEnable(GL_LIGHT2);

  // Initialize graphics modes  
  glEnable(GL_DEPTH_TEST);

  // Initialize GLUT callback functions 
  glutDisplayFunc(GLUTRedraw);
  glutReshapeFunc(GLUTResize);
  glutKeyboardFunc(GLUTKeyboard);
  glutSpecialFunc(GLUTSpecial);
  glutMouseFunc(GLUTMouse);
  glutMotionFunc(GLUTMotion);
}



void GLUTMainLoop(void)
{
  // Initialize viewing center
  if (scene) center = scene->BBox().Centroid();

  // Create output depth directory
  if (output_depth_directory) {
    char cmd[1024];
    sprintf(cmd, "mkdir -p %s", output_depth_directory);
    system(cmd);
  }

  // Create output color directory
  if (output_color_directory) {
    char cmd[1024];
    sprintf(cmd, "mkdir -p %s", output_color_directory);
    system(cmd);
  }

  // Initiate trajectory
  if (!trajectory.IsEmpty()) glutIdleFunc(GLUTIdle);

  // Run main loop -- never returns 
  glutMainLoop();
}


 
static P5DProject *
ReadProject(const char *project_name)
{
  // Start statistics
  RNTime start_time;
  start_time.Read();

  // Allocate scene
  P5DProject *project = new P5DProject();
  if (!project) {
    fprintf(stderr, "Unable to allocate project for %s\n", project_name);
    return NULL;
  }

  // Set project name
  project->name = strdup(project_name);

  // Read project from file
  char filename[4096];
  sprintf(filename, "%s/projects/%s/project.json", input_data_directory, project_name);
  if (!project->ReadFile(filename)) {
    delete project;
    return NULL;
  }

  // Print statistics
  if (print_verbose) {
    printf("Read project from %s ...\n", filename);
    printf("  Time = %.2f seconds\n", start_time.Elapsed());
    fflush(stdout);
  }

  // Temporary
  if (print_debug) project->Print();
  
  // Return project
  return project;
}



static int
ReadTrajectory(const char *filename)
{
  // Start statistics
  RNTime start_time;
  start_time.Read();

  // Open file
  FILE *fp = fopen(filename, "r");
  if (!fp) {
    fprintf(stderr, "Unable to read camera trajectory file %s\n", filename);
    return 0;
  }

  // Read file
  RNScalar vx, vy, vz, tx, ty, tz, ux, uy, uz;
  while (fscanf(fp, "%lf%lf%lf%lf%lf%lf%lf%lf%lf", &vx, &vy, &vz, &tx, &ty, &tz, &ux, &uy, &uz) == (unsigned int) 9) {
    R3Camera *camera = new R3Camera(viewer->Camera());
    R3Point viewpoint(vx, vy, vz);
    R3Vector towards(tx, ty, tz);
    R3Vector up(ux, uy, uz);
    R3Vector right = towards % up;
    towards.Normalize();
    up = right % towards;
    up.Normalize();
    camera->Reposition(viewpoint);
    camera->Reorient(towards, up);
    trajectory.Insert(camera);
  }

  // Close file
  fclose(fp);

  // Print statistics
  if (print_verbose) {
    printf("Read camera trajectory from %s ...\n", filename);
    printf("  Time = %.2f seconds\n", start_time.Elapsed());
    printf("  # Cameras = %d\n", trajectory.NEntries());
    fflush(stdout);
  }

  // Return success
  return 1;
}



static R3Scene *
CreateScene(P5DProject *project)
{
  // Start statistics
  RNTime start_time;
  start_time.Read();

  // Allocate scene
  R3Scene *scene = new R3Scene();
  if (!scene) {
    fprintf(stderr, "Unable to allocate scene\n");
    return NULL;
  }

  // Get node for project
  R3SceneNode *root_node = scene->Root();
  root_node->SetName("Project");

  // Create nodes for floors
  RNScalar floor_z = 0;
  for (int i = 0; i < project->NFloors(); i++) {
    P5DFloor *floor = project->Floor(i);

    // Create floor node
    char floor_name[1024];
    sprintf(floor_name, "Floor_%d", i);
    R3SceneNode *floor_node = new R3SceneNode(scene);
    floor_node->SetName(floor_name);
    floor_node->SetData(floor);
    floor->data = floor_node;
    root_node->InsertChild(floor_node);

    // Set floor transformation
    R3Affine floor_transformation(R3identity_affine);
    floor_transformation.Translate(R3Vector(0, 0, floor_z));
    floor_node->SetTransformation(floor_transformation);
    floor_z += floor->h;

    // Create nodes for objects
    for (int j = 0; j < floor->NObjects(); j++) {
      P5DObject *object = floor->Object(j);
      if (!object->id || !(*(object->id))) continue;

      // Create object node
      char object_name[1024];
      sprintf(object_name, "Object_%d_%s", j, object->id);
      R3SceneNode *object_node = new R3SceneNode(scene);
      object_node->SetName(object_name);
      object_node->SetData(object);
      object->data = object_node;
      floor_node->InsertChild(object_node);

      // Set object transformation
      R3Affine object_transformation(R3identity_affine);
      object_transformation.Translate(R3Vector(object->x, object->y, object->z));
      object_transformation.ZRotate(object->a);
      object_transformation.Scale(R3Vector(object->sX, object->sY, object->sZ));
      if (!strcmp(object->className, "Door")) object_transformation.ZRotate(RN_PI_OVER_TWO);
      else if (!strcmp(object->className, "Window")) object_transformation.ZRotate(RN_PI_OVER_TWO);
      if (object->fX) object_transformation.XMirror();
      if (object->fY) object_transformation.YMirror();
      object_node->SetTransformation(object_transformation);

      // Read obj file
      char obj_name[4096];
      sprintf(obj_name, "%s/objects/%s/%s.obj", input_data_directory, object->id, object->id);
      if (!ReadObj(scene, object_node, obj_name)) return 0;
    }
    
    // Create nodes for rooms
    for (int j = 0; j < floor->NRooms(); j++) {
      P5DRoom *room = floor->Room(j);

      // Create room node
      char room_name[1024];
      sprintf(room_name, "Room_%d", j);
      R3SceneNode *room_node = new R3SceneNode(scene);
      room_node->SetName(room_name);
      room_node->SetData(room);
      room->data = room_node;
      floor_node->InsertChild(room_node);

      // Set room transformation
      // R3Affine room_transformation(R3identity_affine);
      // room_transformation.Translate(R3Vector(room->x, room->y, 0));
      // room_node->SetTransformation(room_transformation);

      // Create node for floor
      // ???
      
      // Check if enclosed room
      if (!strcmp(room->className, "Room")) {
        // Create nodes for ceiling
        // ???

#if 0        
        // Create nodes for walls
        for (int k = 0; k < room->NWalls(); k++) {
          P5DWall *wall = room->Wall(k);

          // Check if wall is hidden
          if (wall->hidden) continue;

          // Get vector for adding wall depth
          R3Point p1(wall->x1, wall->y1, 0);
          R3Point p2(wall->x2, wall->y2, 0);
          R3Vector d = p2 - p1;
          d.ZRotate(RN_PI_OVER_TWO);
          d.Normalize();
          d *= 0.5 * wall->w;

          // Get vector for adding wall height
          R3Vector h = room->h * R3posz_vector;

          // Get wall width
          RNScalar w = R3Distance(p1, p2);

          // Create wall vertices
          RNArray<R3TriangleVertex *> wall_vertices;
          wall_vertices.Insert(new R3TriangleVertex(p1 - d, R2Point(0,0)));
          wall_vertices.Insert(new R3TriangleVertex(p1 - d + h, R2Point(0,room->h)));
          wall_vertices.Insert(new R3TriangleVertex(p1 + d, R2Point(w,0)));
          wall_vertices.Insert(new R3TriangleVertex(p1 + d + h, R2Point(w,room->h)));
          wall_vertices.Insert(new R3TriangleVertex(p2 - d, R2Point(w,0)));
          wall_vertices.Insert(new R3TriangleVertex(p2 - d + h, R2Point(w,room->h)));
          wall_vertices.Insert(new R3TriangleVertex(p2 + d, R2Point(0,0)));
          wall_vertices.Insert(new R3TriangleVertex(p2 + d + h, R2Point(0,room->h)));

          // Create wall triangles
          RNArray<R3Triangle *> wall_triangles;
          wall_triangles.Insert(new R3Triangle(wall_vertices[0], wall_vertices[4], wall_vertices[5]));
          wall_triangles.Insert(new R3Triangle(wall_vertices[0], wall_vertices[5], wall_vertices[1]));
          wall_triangles.Insert(new R3Triangle(wall_vertices[2], wall_vertices[7], wall_vertices[6]));
          wall_triangles.Insert(new R3Triangle(wall_vertices[2], wall_vertices[3], wall_vertices[7]));
          R3Shape *wall_shape = new R3TriangleArray(wall_vertices, wall_triangles);

          // Create wall material

          // Create wall element
          R3SceneElement *wall_element = new R3SceneElement();
          wall_element->SetMaterial(&R3default_material);
          wall_element->InsertShape(wall_shape);         
        
          // Create wall node
          char wall_name[1024];
          sprintf(wall_name, "Wall_%d", k);
          R3SceneNode *wall_node = new R3SceneNode(scene);
          wall_node->InsertElement(wall_element);
          wall_node->SetName(wall_name);
          wall_node->SetData(wall);
          wall->data = wall_node;
          room_node->InsertChild(wall_node);
        }
#else
        // Read walls
        char rm_name[4096], node_name[4096];
        sprintf(rm_name, "%s/roomfiles/%s/fr_%drm_%d.obj", input_data_directory, project->name, i+1, room->idx_index+1); //fr_1rm_1.obj
        R3SceneNode *wall_node = new R3SceneNode(scene);
        sprintf(node_name, "WALL_%d", room->idx_index);
        wall_node->SetName(node_name);
        if (!ReadObj(scene, wall_node, rm_name)) return 0;
        room_node->InsertChild(wall_node);

        // Read floor
        sprintf(rm_name, "%s/roomfiles/%s/fr_%drm_%df.obj", input_data_directory, project->name, i+1, room->idx_index+1); //fr_1rm_1.obj
        R3SceneNode *rmfloor_node = new R3SceneNode(scene);
        sprintf(node_name, "FLOOR_%d", room->idx_index);
        rmfloor_node->SetName(node_name);
        if (!ReadObj(scene, rmfloor_node, rm_name)) return 0;
        room_node->InsertChild(rmfloor_node);
        
        // Read ceiling
        // sprintf(rm_name, "%s/roomfiles/%s/fr_%drm_%dc.obj", input_data_directory, project->name, i+1, room->idx_index+1); //fr_1rm_1.obj
        // R3SceneNode *rmceil_node = new R3SceneNode(scene);
        // sprintf(node_name, "CEILING_%d", room->idx_index);
        // rmceil_node->SetName(node_name);
        // if (!ReadObj(scene, rmceil_node, rm_name)) return 0;
        // room_node->InsertChild(rmceil_node);
#endif
      }
    }
  }

  // Print statistics
  if (print_verbose) {
    printf("Created scene ...\n");
    printf("  Time = %.2f seconds\n", start_time.Elapsed());
    printf("  # Nodes = %d\n", scene->NNodes());
    fflush(stdout);
  }

  // Return scene
  return scene;
}



static R3Viewer *
CreateBirdsEyeViewer(const R3Scene *scene)
{
  // Setup camera view looking down the Z axis
  R3Box bbox = scene->BBox();
  assert(!bbox.IsEmpty());
  RNLength r = bbox.DiagonalRadius();
  assert((r > 0.0) && RNIsFinite(r));
  if (!initial_camera) initial_camera_origin = scene->Centroid() - initial_camera_towards * (2.5 * r);
  R3Camera camera(initial_camera_origin, initial_camera_towards, initial_camera_up, 0.4, 0.4, 0.01 * r, 10.0 * r);
  R2Viewport viewport(0, 0, GLUTwindow_width, GLUTwindow_height);
  return new R3Viewer(camera, viewport);
}



static int 
ParseArgs(int argc, char **argv)
{
  // Parse arguments
  argc--; argv++;
  while (argc > 0) {
    if ((*argv)[0] == '-') {
      if (!strcmp(*argv, "-v")) print_verbose = 1; 
      else if (!strcmp(*argv, "-debug")) print_debug = 1; 
      else if (!strcmp(*argv, "-trajectory")) { argc--; argv++; input_trajectory_name = *argv; }
      else if (!strcmp(*argv, "-data_directory")) { argc--; argv++; input_data_directory = *argv; }
      else if (!strcmp(*argv, "-depth_directory")) { argc--; argv++; output_depth_directory = *argv; }
      else if (!strcmp(*argv, "-color_directory")) { argc--; argv++; output_color_directory = *argv; }
      else if (!strcmp(*argv, "-camera")) {
        RNCoord x, y, z, tx, ty, tz, ux, uy, uz;
        argv++; argc--; x = atof(*argv);
        argv++; argc--; y = atof(*argv);
        argv++; argc--; z = atof(*argv);
        argv++; argc--; tx = atof(*argv);
        argv++; argc--; ty = atof(*argv);
        argv++; argc--; tz = atof(*argv);
        argv++; argc--; ux = atof(*argv);
        argv++; argc--; uy = atof(*argv);
        argv++; argc--; uz = atof(*argv);
        initial_camera_origin = R3Point(x, y, z);
        initial_camera_towards.Reset(tx, ty, tz);
        initial_camera_up.Reset(ux, uy, uz);
        initial_camera = TRUE;
      }
      else { 
        fprintf(stderr, "Invalid program argument: %s", *argv); 
        exit(1); 
      }
      argv++; argc--;
    }
    else {
      if (!input_project_name) input_project_name = *argv;
      else { fprintf(stderr, "Invalid program argument: %s", *argv); exit(1); }
      argv++; argc--;
    }
  }

  // Check scene filename
  if (!input_project_name) {
#if 0
    fprintf(stderr, "Usage: p5dview [inputprojectfile]\n");
    return 0;
#else
    char buffer[4096];
    getcwd(buffer, 4096);
    char *bufferp = strrchr(buffer, '/');
    if (bufferp) input_project_name = strdup(bufferp+1);
#endif
  }

  // Return OK status 
  return 1;
}



int main(int argc, char **argv)
{
  // Parse program arguments
  if (!ParseArgs(argc, argv)) exit(-1);

  // Initialize GLUT
  GLUTInit(&argc, argv);

  // Read project
  project = ReadProject(input_project_name);
  if (!project) exit(-1);

  // Create scene
  scene = CreateScene(project);
  if (!scene) exit(-1);

  // Create viewer
  viewer = CreateBirdsEyeViewer(scene);
  if (!viewer) exit(-1);

  // Read camera trajectory
  if (input_trajectory_name) {
    if (!ReadTrajectory(input_trajectory_name)) exit(-1);
  }

  // Run GLUT interface
  GLUTMainLoop();

  // Return success 
  return 0;
}

















