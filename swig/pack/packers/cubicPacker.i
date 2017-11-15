%module CubicPacker

%{
#include "../pack/packers/cubicPacker.h"
%}

class CubicPacker : public Packer {

  public:
    CubicPacker(double *xyz, double h, double *xyz=NULL);
    std::vector<double> GetPeriodicExtent();
    double GetParticleVolume();
};
