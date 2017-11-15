%module HcpPacker

%{
#include "../pack/packers/hcpPacker.h"
%}

class HcpPacker : public Packer {

  public:
    HcpPacker(double *xyz, double h, bool rotate90=false, double *xyz=NULL);
    std::vector<double> GetPeriodicExtent();
    double GetParticleVolume();
};
