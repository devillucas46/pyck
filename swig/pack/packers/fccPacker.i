%module FccPacker

%{
#include "../pack/packers/fccPacker.h"
%}

class FccPacker : public Packer {

  public:
    FccPacker(double *xyz, double h);
};