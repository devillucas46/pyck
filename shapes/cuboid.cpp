#include "cuboid.h"

Cuboid::Cuboid(int state, double *p1, double *p2) : Shape(state)
{
  this->p1 = p1;
  this->p2 = p2;

  this->boundingBox = new BoundingBox(p1,p2);
}

Cuboid::~Cuboid(){}

bool Cuboid::IsInside(double *pt)
{
  bool isInside = true;
  for(int i=0; i<3; i++){
    if(pt[i]<p1[i]) isInside = false;
    if(pt[i]>p2[i]) isInside = false;
  }

  return isInside;
}