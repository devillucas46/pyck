import pyck

L = [10.0, 10.0, 10.0]
r = 0.1

# Create a packer, see packers directory for options
cubic = pyck.CubicPacker(L, r)
# do not create the cubic packer in this function call as it will be
# destroyed, blame SWIG developers
pack = pyck.StructuredPack(cubic)

# Create some shapes, see shapes directory for options and reference
# First argument is always a tag for these particles
# Mapping operations are applied sequentially
cube = pyck.Cuboid(1, [2, 2, 2], [6, 6, 6])
sphere = pyck.Sphere(2, [2, 2, 2], 5)

# Map the shapes and generate the pack
# As with creating the cubic packer, do not create the shapes within the
# function call here
pack.AddShape(cube)
pack.AddShape(sphere)
pack.Process()

# Create a new model from the pack
# model = pyck.Model() # Create an empty model
# model = pyck.Model(pack) # Create a model from a pack
model = pyck.Model([10.0, 10.0, 10.0, 11.0, 11.0, 11.0, 12.0, 12.0, 12.0], [
                   3, 3, 3], 3, 3)  # Create a model from points generated in python
model.AddPack(pack)  # Add a pyck pack to an existing model
model.AddPack([13.0, 13.0, 13.0, 14.0, 14.0, 14.0, 15.0, 15.0, 15.0], [
              3, 3, 3], 3, 3)  # Add points generated in python to an existing pack

# Create a new field of n-dimensional integers
# Arguments are CreateIntField(label,dimensions)
# label - label for this field in the vtp file
# dimensions - dimensionality of this field, doesnt have to correspond to model dimensions
# Create field of doubles in the same way with CreateDoubleField
stateField = model.CreateIntField("State", 1)

# Arguments are SetIntField(field,tag,value(s))
# field - returned from CreateIntField
# tag - tag applied to particles during shape Mapping
# value(s) - singular value or array of values [v1, v2,...vn] to set of
# particles with a matching tag
model.SetIntField(stateField, 1, 10)
model.SetIntField(stateField, 2, 20)

# Set default simulation parameters for the VTP file
# SetDefaultParameters computes geometric simulation parameters from pack parameters
# pyck_utils.SetDefaultParameters(model,L,r)
# Overwrite some parameters
# Arguments are SetParameter(Label,Value)
model.SetParameter("MaxSteps", "100")
model.SetParameter("Mass", "0.5")

# Or use a python dictionary to overwrite parameters
parameters = pyck.Parameters({'ViscAlpha': '0.1', 'ViscBeta': '0.2'})
model.SetParameters(parameters)

# Create a file writer, in this case VTP according to spark format
writer = pyck.SparkWriter()

# Write the VTP file
model.Serialize("hello.vtp", writer)
