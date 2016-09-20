#ifndef FIBERTRACTSAMPLING_H
#define FIBERTRACTSAMPLING_H
#include <vtkPolyDataReader.h>
#include <itkGroupSpatialObject.h>
#include <vtkCellArray.h>
#include <vtkSmartPointer.h>
#include <itkDTITubeSpatialObject.h>

typedef itk::GroupSpatialObject<3> GroupType;
typedef GroupType::ChildrenListType ChildrenListType;
typedef itk::DTITubeSpatialObjectPoint<3> DTIPointType;

class SamplingFilter{
public:
    /** Constructor & Destructor */
    SamplingFilter(int NbSamples, std::string inputfilename);
    ~SamplingFilter();

    /** Main Functions */
    void SetInput(std::string inputFibers);
    void SetNbSamples(int NbSamples);
    GroupType::Pointer GetOutput();


private:
    /** Internal Functions*/
 void sampling_unit(ChildrenListType::const_iterator it); // Create a new fiber with the good number of samples

    /** Variables */
    GroupType::Pointer inputFibers;
    GroupType::Pointer outputFibers;
    int nbSamples;

};

#endif // FIBERTRACTSAMPLING_H
