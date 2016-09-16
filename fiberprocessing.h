#ifndef FIBERPROCESSING_H
#define FIBERPROCESSING_H

#include <vtkPolyDataReader.h>
#include <vtkPolyDataWriter.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkCellArray.h>
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <itkSymmetricSecondRankTensor.h>
#include <itkDTITubeSpatialObject.h>
#include <vtkDataArray.h>
#include <vtkPointData.h>
#include <vtkFloatArray.h>
#include <itkGroupSpatialObject.h>
#include <itkExceptionObject.h>
#include <itkSpatialObjectReader.h>

typedef itk::GroupSpatialObject<3> GroupType;
typedef GroupType::ChildrenListType ChildrenListType;
typedef itk::DTITubeSpatialObjectPoint<3> DTIPointType;


GroupType::Pointer read_fiber(std::string filename); // Read the bunch of fibers at the location in parameter and return an array where each cell is a different fiber

void write_fiber(const std::string &filename, GroupType::Pointer fibergroup); // write the bunch of fibers at the location in parameter return an array where each cell is a different fiber

#endif // FIBERPROCESSING_H