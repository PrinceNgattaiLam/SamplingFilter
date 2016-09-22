#include "fiberprocessing.h"

GroupType::Pointer readFiberFile(std::string filename)
{


    if (filename.rfind(".vt")!= std::string::npos)
    {
        // Build up the principal data structure for fiber tracts
        GroupType::Pointer fibergroup = GroupType::New();

        vtkSmartPointer<vtkPolyData> fibdata=vtkSmartPointer<vtkPolyData>::New();

        /** We read the vtk data */
        if (filename.rfind(".vtk")!=std::string::npos)
        {
            vtkSmartPointer<vtkPolyDataReader> reader = vtkPolyDataReader::New();
            reader->SetFileName(filename.c_str());
            reader->Update();
            fibdata = reader->GetOutput();

        }
        else if (filename.rfind(".vtp")!=std::string::npos)
        {
            vtkSmartPointer<vtkXMLPolyDataReader> reader= vtkXMLPolyDataReader::New();
            reader->SetFileName(filename.c_str());
            reader->Update();
            fibdata = reader->GetOutput();
        }
        else
        {
            throw itk::ExceptionObject("Unknown file format for fibers");
        }

        // Definition of some useful types
        typedef  itk::SymmetricSecondRankTensor<double,3> ITKTensorType;
        typedef  ITKTensorType::EigenValuesArrayType LambdaArrayType;

        // Iterate over VTK data
        const int nfib = fibdata->GetNumberOfCells(); // "nfib" = number of fibers in the input fiber bundle
        int pindex = -1;
        for(int i = 0; i < nfib; ++i)
        {
            itk::DTITubeSpatialObject<3>::Pointer dtiTube = itk::DTITubeSpatialObject<3>::New();
            vtkSmartPointer<vtkCell> fib = fibdata->GetCell(i); // "fib" = i th fiber in fibdata

            vtkSmartPointer<vtkPoints> points = fib->GetPoints(); // "points" contains all the points of one fiber fib
            std::vector<DTIPointType> pointsToAdd;

            vtkSmartPointer<vtkDataArray> fibtensordata = fibdata->GetPointData()->GetTensors(); // WHY IN THE LOOP?????????????????????????

            for(int j = 0; j < points->GetNumberOfPoints(); ++j) // For each point in the fiber "fib"
            {
                ++pindex;
                double * coordinates = points->GetPoint(j);

                DTIPointType pt;
                // Convert from RAS to LPS for vtk
                pt.SetPosition(coordinates[0], coordinates[1], coordinates[2]);
                pt.SetRadius(0.5);
                pt.SetColor(0.0, 0.0, 1.0);

                if (fibtensordata != NULL)
                {

                    double * vtktensor = fibtensordata->GetTuple9(pindex);

                    float floattensor[6];
                    ITKTensorType itktensor;

                    floattensor[0] = itktensor[0] = vtktensor[0];
                    floattensor[1] = itktensor[1] = vtktensor[1];
                    floattensor[2] = itktensor[2] = vtktensor[2];
                    floattensor[3] = itktensor[3] = vtktensor[4];
                    floattensor[4] = itktensor[4] = vtktensor[5];
                    floattensor[5] = itktensor[5] = vtktensor[8];

                    pt.SetTensorMatrix(floattensor);

                    LambdaArrayType lambdas;

                    // Need to do eigenanalysis of the tensor
                    itktensor.ComputeEigenValues(lambdas);

                    float md = (lambdas[0] + lambdas[1] + lambdas[2])/3;
                    float fa = sqrt(1.5) * sqrt((lambdas[0] - md)*(lambdas[0] - md) +
                            (lambdas[1] - md)*(lambdas[1] - md) +
                            (lambdas[2] - md)*(lambdas[2] - md))
                            / sqrt(lambdas[0]*lambdas[0] + lambdas[1]*lambdas[1] + lambdas[2]*lambdas[2]);

                    float logavg = (log(lambdas[0])+log(lambdas[1])+log(lambdas[2]))/3;

                    //                    float ga =  sqrt( SQ2(log(lambdas[0])-logavg)
                    //                            + SQ2(log(lambdas[1])-logavg)
                    //                            + SQ2(log(lambdas[2])-logavg) );
                    //                  float fro = sqrt(lambdas[0]*lambdas[0] + lambdas[1]*lambdas[1] + lambdas[2]*lambdas[2]);
                    float ad = lambdas[2];
                    float rd = (lambdas[0] + lambdas[1])/2;

                    pt.AddField("FA",fa);
                    pt.AddField("MD",md);
                    //pt.AddField("FRO",fro);
                    pt.AddField("l2",lambdas[1]);
                    pt.AddField("l3",lambdas[0]);
                    pt.AddField("l1",ad);
                    pt.AddField("RD",rd);
                    //pt.AddField("GA",ga);
                }
                pointsToAdd.push_back(pt);
            }

            dtiTube->SetPoints(pointsToAdd);
            fibergroup->AddSpatialObject(dtiTube);
        }
        //       std::cout << "# of points with scalar values: " << scalarCount << "; # of points without scalar values: " << nonscalarCount << "\n" << std::endl;


        return fibergroup;
    } // end process .vtk .vtp
    else
    {
        throw itk::ExceptionObject("Unknown fiber file");
    }
}

void writeFiberFile(const std::string & filename, GroupType::Pointer fibergroup){

    // Make sure origins are updated
    fibergroup->ComputeObjectToWorldTransform();

    // VTK Poly Data
    if (filename.rfind(".vt") != std::string::npos)
    {
        // Build VTK data structure
        vtkSmartPointer<vtkPolyData> polydata(vtkPolyData::New());
        vtkSmartPointer<vtkFloatArray> tensorsdata(vtkFloatArray::New());
        vtkSmartPointer<vtkIdList> ids(vtkIdList::New());
        vtkSmartPointer<vtkPoints> pts(vtkPoints::New());

        tensorsdata->SetNumberOfComponents(9);
        polydata->SetPoints (pts);

        ids->SetNumberOfIds(0);
        pts->SetNumberOfPoints(0);
        polydata->Allocate();

        std::auto_ptr<ChildrenListType> children(fibergroup->GetChildren(0));
        typedef ChildrenListType::const_iterator IteratorType;

        for (IteratorType it = children->begin(); it != children->end(); it++)
        {
            itk::SpatialObject<3>* tmp = (*it).GetPointer();
            itk::DTITubeSpatialObject<3>* tube = dynamic_cast<itk::DTITubeSpatialObject<3>* >(tmp);
            unsigned int nPointsOnFiber = tube->GetNumberOfPoints();
            vtkIdType currentId = ids->GetNumberOfIds();

            for (unsigned int k = 0; k < nPointsOnFiber; k++)
            {
                itk::Point<double, 3> v(tube->GetPoint(k)->GetPosition());
                itk::Vector<double, 3> spacing(tube->GetSpacing());
                itk::Vector<double, 3> origin(tube->GetObjectToWorldTransform()->GetOffset());

                // convert origin from LPS -> RAS
                /*       origin[0] = -origin[0];
           origin[1] = -origin[1];*/

                vtkIdType id;
                // Need to multiply v by spacing and origin
                // Also negate the first to convert from LPS -> RAS
                // for slicer 3
                /*        id = pts->InsertNextPoint(-v[0] * spacing[0] + origin[0],
                                     -v[1] * spacing[1] + origin[1],
                                     v[2] * spacing[2] + origin[2]);*/

                id = pts->InsertNextPoint(v[0] * spacing[0] + origin[0],
                        v[1] * spacing[1] + origin[1],
                        v[2] * spacing[2] + origin[2]);

                ids->InsertNextId(id);

                itk::DTITubeSpatialObjectPoint<3>* sopt = dynamic_cast<itk::DTITubeSpatialObjectPoint<3>* >(tube->GetPoint(k));
                float vtktensor[9];
                vtktensor[0] = sopt->GetTensorMatrix()[0];
                vtktensor[1] = sopt->GetTensorMatrix()[1];
                vtktensor[2] = sopt->GetTensorMatrix()[2];
                vtktensor[3] = sopt->GetTensorMatrix()[1];
                vtktensor[4] = sopt->GetTensorMatrix()[3];
                vtktensor[5] = sopt->GetTensorMatrix()[4];
                vtktensor[6] = sopt->GetTensorMatrix()[2];
                vtktensor[7] = sopt->GetTensorMatrix()[4];
                vtktensor[8] = sopt->GetTensorMatrix()[5];

                tensorsdata->InsertNextTupleValue(vtktensor);

            }
            
            polydata->InsertNextCell(VTK_POLY_LINE, nPointsOnFiber, ids->GetPointer(currentId));
        }

        polydata->GetPointData()->SetTensors(tensorsdata);

        // Legacy
        if (filename.rfind(".vtk") != std::string::npos)
        {
            vtkSmartPointer<vtkPolyDataWriter> fiberwriter = vtkPolyDataWriter::New();
            //       fiberwriter->SetFileTypeToBinary();
            fiberwriter->SetFileName(filename.c_str());
#if (VTK_MAJOR_VERSION < 6)
            fiberwriter->SetInput(polydata);
#else
            fiberwriter->SetInputData(polydata);
#endif
            fiberwriter->Update();
        }
        // XML
        else if (filename.rfind(".vtp") != std::string::npos)
        {
            vtkSmartPointer<vtkXMLPolyDataWriter> fiberwriter = vtkXMLPolyDataWriter::New();
            fiberwriter->SetFileName(filename.c_str());
#if (VTK_MAJOR_VERSION < 6)
            fiberwriter->SetInput(polydata);
#else
            fiberwriter->SetInputData(polydata);
#endif
            fiberwriter->Update();
        }
        else
        {
            throw itk::ExceptionObject("Unknown file format for fibers");
        }
    }
    else
    {
        throw itk::ExceptionObject("Unknown file format for fibers");
    }
}

