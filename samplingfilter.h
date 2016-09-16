#ifndef FIBERTRACTSAMPLING_H
#define FIBERTRACTSAMPLING_H
#include <vtkPolyDataReader.h>
#include <itkGroupSpatialObject.h>
#include <vtkCellArray.h>
#include <vtkSmartPointer.h>

typedef itk::GroupSpatialObject<3> GroupType;

class SamplingFilter{
public:
    /** Constructor & Destructor */
    SamplingFilter(int NbSamples, std::string inputFibers);

    ~SamplingFilter();

    /** Main Functions */
    void SetInput(std::string inputFibers);
    void SetNbSamples(int NbSamples);
    GroupType::Pointer GetOuput();


private:
    /** Internal Functions*/
 GroupType::Pointer Sampling(); // Create a new fiber with the good number of samples

    /** Variables */
    vtkSmartPointer<vtkCellArray> inputFibers;
    int nbSamples;




};

#endif // FIBERTRACTSAMPLING_H
