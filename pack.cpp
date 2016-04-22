#include "pack.h"
#include "shape.h"


Pack::Pack(){}

Pack::~Pack(){}

void Pack::AddShape(Shape *shape)
{
  shapes.push_back(shape);
}

void Pack::MapShapes()
{
  for (int i=0; i<shapes.size(); i++)
  {
    MapShape(shapes[i]);
  }
}
