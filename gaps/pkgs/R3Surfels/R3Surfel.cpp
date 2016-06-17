/* Source file for the R3 surfel class */



/* Include files */

#include "R3Surfels/R3Surfels.h"



/* Public functions */

R3Surfel::
R3Surfel(void)
  : radius(0),
    flags(0)
{
  // Set everything
  position[0] = 0;
  position[1] = 0;
  position[2] = 0;
  normal[0] = 0;
  normal[1] = 0;
  normal[2] = 0;
  color[0] = 0;
  color[1] = 0;
  color[2] = 0;
}



R3Surfel::
R3Surfel(float x, float y, float z, 
  unsigned char r, unsigned char g, unsigned char b, 
  RNBoolean aerial)
  : radius(0),
    flags(0)
{
  // Set everything
  position[0] = x;
  position[1] = y;
  position[2] = z;
  normal[0] = 0;
  normal[1] = 0;
  normal[2] = 0;
  color[0] = r;
  color[1] = g;
  color[2] = b;
  SetAerial(aerial);
}



R3Surfel::
R3Surfel(float x, float y, float z, float nx, float ny, float nz,
  float radius, unsigned char r, unsigned char g, unsigned char b, 
  unsigned char flags)
  : radius(radius),
    flags(flags)
{
  // Set everything
  position[0] = x;
  position[1] = y;
  position[2] = z;
  normal[0] = nx;
  normal[1] = ny;
  normal[2] = nz;
  color[0] = r;
  color[1] = g;
  color[2] = b;
}



