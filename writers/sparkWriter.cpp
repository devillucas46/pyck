#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <math.h>
#include <cstdlib>

#include "sparkWriter.h"
#include "../intField.h"
#include "../doubleField.h"
#include "../progressBar.h"

#include <zlib.h>
#include "base64.h"

SparkWriter::SparkWriter(){}

SparkWriter::~SparkWriter(){}

void SparkWriter::Write(std::string fname,
  std::map<std::string, std::string> parameters,
  double *positions,
  std::vector<IntField*> intFields,
  std::vector<DoubleField*> doubleFields,
  int dim,
  long numParticles
)
{
  if(numParticles == 0)
  {
    std::cout << "Error: Number of particles is 0, exiting" << std::endl;
    std::exit(1);
  }

  int progress = 1;
  ProgressBar pb(intFields.size()+doubleFields.size()+parameters.size()+1,"Writing output");

  std::ofstream outfile((char*)fname.c_str(), std::ios::out);
  if(outfile.is_open()) {
    std::cout << "Writing to output file..." << std::flush;

    outfile << "<?xml version='1.0' encoding='UTF-8'?>" << std::endl;
    outfile << "<VTKFile type=\"PolyData\" byte_order=\"LittleEndian\" compressor=\"vtkZLibDataCompressor\">" << std::endl;
    outfile << "<SimulationParameters>" << std::endl;

    std::map<std::string, std::string>::iterator it;
    for ( it = parameters.begin(); it != parameters.end(); it++ )
    {
      outfile << "<" << it->first << ">" << it->second << "</" << it->first << ">" << std::endl;
      pb.UpdateProgress(progress);
      progress++;
    }

    outfile << "</SimulationParameters>" << std::endl;
    outfile << "<PolyData>" << std::endl;
    outfile << "<Piece NumberOfPoints=\""<< numParticles  << "\" NumberOfVerts=\"0\" NumberOfLines=\"0\" NumberOfStrips=\"0\" NumberOfPolys=\"0\">" << std::endl;
    outfile << "<Points>" << std::endl;
    outfile << "<DataArray type=\"Float64\" NumberOfComponents=\"3\" format=\"binary\" Name=\"Position\">";
    WriteEncodedString((void *)positions,numParticles,3,8,&outfile);
    outfile << "</DataArray>" << std::endl;
    outfile << "</Points>" << std::endl;
    outfile << "<PointData>" << std::endl;
    pb.UpdateProgress(progress);
    progress++;

    for (long intf=0; intf < intFields.size(); intf++)
    {
      IntField *thisField = intFields[intf];
      outfile << "<DataArray type=\"Int32\" NumberOfComponents=\""<< thisField->dim <<"\" format=\"binary\" Name=\"" << thisField->name << "\">";
      WriteEncodedString((void *)thisField->data,numParticles,thisField->dim,4,&outfile);
      outfile << "</DataArray>" << std::endl;
      pb.UpdateProgress(progress);
      progress++;
    }

    for (long intf=0; intf < doubleFields.size(); intf++)
    {
      DoubleField *thisField = doubleFields[intf];
      outfile << "<DataArray type=\"Float64\" NumberOfComponents=\""<< thisField->dim <<"\" format=\"binary\" Name=\"" << thisField->name << "\">";
      WriteEncodedString((void *)thisField->data,numParticles,thisField->dim,8,&outfile);
      outfile << "</DataArray>" << std::endl;
      pb.UpdateProgress(progress);
      progress++;
    }

    outfile << "</PointData>" << std::endl;
    outfile << "</Piece>" << std::endl;
    outfile << "</PolyData>" << std::endl;
    outfile << "</VTKFile>" << std::endl;
    outfile.close();
    pb.Finish();
  }
}

void SparkWriter::WriteEncodedString(void *data, long numParticles, int dim, int numBytes, std::ofstream *outfile)
{
  uLongf inputBytes = numParticles * dim * numBytes;
  uLongf numberOfBlocks = ceil(inputBytes / (double) VTP_BINARY_BLOCK_SIZE);
  uLongf lastBlockSize  = inputBytes % VTP_BINARY_BLOCK_SIZE;


  int *headerData = new int[3+numberOfBlocks];
  headerData[0] = numberOfBlocks;
  headerData[1] = (numberOfBlocks>1) ? VTP_BINARY_BLOCK_SIZE : lastBlockSize;
  headerData[2] = lastBlockSize;

  uLongf compressedDataSize = compressBound((uLongf) lastBlockSize) + (numberOfBlocks-1)*compressBound((uLongf) VTP_BINARY_BLOCK_SIZE);
  unsigned char *compressBuffer = new unsigned char[compressedDataSize];
  int totalCompressedDataSize = 0;

  uLongf currentSize = compressedDataSize;

  unsigned char *compressBufferPtr = compressBuffer;
  for (int i=0; i<numberOfBlocks; i++)
  {
    int blockSize = VTP_BINARY_BLOCK_SIZE;
    if (numberOfBlocks == 1 || i==numberOfBlocks-1) // last block
      blockSize = lastBlockSize;
    int blockStart = i*VTP_BINARY_BLOCK_SIZE;

    compress(compressBufferPtr,
              &currentSize,
              (unsigned char *) data+blockStart,
              blockSize);

    compressBufferPtr += currentSize;
    totalCompressedDataSize += currentSize;

    headerData[3+i] = (int) currentSize;
    currentSize = compressedDataSize - (compressBufferPtr-compressBuffer);
  }

  uLongf b64HeaderSize = ((4*(numberOfBlocks+3)+2)/3)*4;
  uLongf b64DataSize   = ((totalCompressedDataSize+2)/3)*4;

  char *outputBuffer = new char[b64HeaderSize + b64DataSize+1];
  base64encode(headerData, 4*(numberOfBlocks+3), outputBuffer, b64HeaderSize+1);
  base64encode(compressBuffer, totalCompressedDataSize, outputBuffer+b64HeaderSize, b64DataSize+1);

  delete [] compressBuffer;
  delete [] headerData;

  outfile->write(outputBuffer, b64HeaderSize + b64DataSize);
  delete [] outputBuffer;
}
