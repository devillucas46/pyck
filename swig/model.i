%module model

%{
#include "../model.h"
#include "../intField.h"
#include "../doubleField.h"
%}

%include "std_string.i"
%include "std_map.i"
namespace std {
%template(Parameters) map<string, string>;
}

class Model {
  public:

    Model();

    Model(Pack *pack);

    Model(double *dprop, int *iprop, long numParticles, int dim);

    ~Model();

    void AddPack(Pack *pack);

    void AddPack(double *dprop, int *iprop, long numParticles, int dim);

    int CreateIntField(std::string name, int dim);

    int CreateDoubleField(std::string name, int dim);

    void SetIntField(int handle, int state, int *iprop);

    %rename SetIntField SetIntFieldCallback;
    void SetIntField(int handle, int state, PyObject *PyFunc);

    IntField *GetIntField(int handle);

    void SetDoubleField(int handle, int state, double *dprop);

    %rename SetDoubleField SetDoubleFieldCallback;
    void SetDoubleField(int handle, int state, PyObject *PyFunc);

    DoubleField *GetDoubleField(int handle);

    void SetParameter(std::string key, std::string value);

    void SetParameters(std::map<std::string, std::string> &parameters);

    int ReadSingleIntegerParameter(std::string key);

    std::vector<double> Model::ReadMultipleDoubleParameter(std::string key, int n);

    void Serialize(std::string fname, Writer *writer);

};
