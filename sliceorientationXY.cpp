#include "sliceorientationXY.h"



SliceOrientationXY::SliceOrientationXY(QWidget* parent)
    : QVTKOpenGLNativeWidget(parent), _backgroundColor{ 0.0, 0.0, 0. }
{
    setRenderWindow(_renderWindow.Get());
    setupRender();
}



void SliceOrientationXY::removeDataSet()
{
    vtkProp* actor = _renderer->GetActors()->GetLastProp();
    if (actor != nullptr) {
        _renderer->RemoveVolume(actor);
    }
}


void SliceOrientationXY::addDataSet(vtkSmartPointer<vtkImageReader2> reader)
{
    removeDataSet();
    _reader = reader;

    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    _reslicer->SetupInteractor(renderWindowInteractor);
    _reslicer->SetRenderWindow(_renderWindow);
    _reslicer->SliceScrollOnMouseWheelOff();
    _reslicer->SetSliceOrientationToXY();
    _reslicer->SetRenderer(_renderer);
    _reslicer->SetInputData(_reader->GetOutput());
    _reslicer->SetResliceModeToAxisAligned();
    _reslicer->SetColorLevel(40.0);
    _reslicer->SetSlice(_reslicer->GetSliceMax() / 2);
    _reslicer->SetColorWindow(4000.0);
    _reslicer->SetResliceMode(0);


    int Z = _reslicer->GetSliceMax();

    std::vector<PointSpline> vecPointsForSpline;
//    vecPointsForSpline.emplace_back(PointSpline{ 114, 440, Z });
    vecPointsForSpline.emplace_back(PointSpline{ 142, 353, Z });
    vecPointsForSpline.emplace_back(PointSpline{ 148, 323, Z });
    vecPointsForSpline.emplace_back(PointSpline{ 157, 290, Z });
    vecPointsForSpline.emplace_back(PointSpline{ 165, 250, Z });
    vecPointsForSpline.emplace_back(PointSpline{ 167, 204, Z });
    vecPointsForSpline.emplace_back(PointSpline{ 177, 162, Z });
    vecPointsForSpline.emplace_back(PointSpline{ 187, 130, Z });
    vecPointsForSpline.emplace_back(PointSpline{ 200, 93, Z });
    vecPointsForSpline.emplace_back(PointSpline{ 227, 65, Z });
    vecPointsForSpline.emplace_back(PointSpline{ 263, 56, Z });
    vecPointsForSpline.emplace_back(PointSpline{ 286, 54, Z });
    vecPointsForSpline.emplace_back(PointSpline{ 316, 58, Z });
    vecPointsForSpline.emplace_back(PointSpline{ 347, 79, Z });
    vecPointsForSpline.emplace_back(PointSpline{ 367, 130, Z });
    vecPointsForSpline.emplace_back(PointSpline{ 377, 165, Z });
    vecPointsForSpline.emplace_back(PointSpline{ 384, 203, Z });
    vecPointsForSpline.emplace_back(PointSpline{ 394, 249, Z });
    vecPointsForSpline.emplace_back(PointSpline{ 409, 310, Z });
    vecPointsForSpline.emplace_back(PointSpline{ 420, 344, Z });
 //   vecPointsForSpline.emplace_back(PointSpline{ 424, 380, Z });
  //  vecPointsForSpline.emplace_back(PointSpline{ 432, 408, Z });
  //  vecPointsForSpline.emplace_back(PointSpline{ 442, 429, Z });
    vtkImageData* imageCurrentData_ = _reader->GetOutput();
    vtkNew<vtkPoints> points;
    for (int i = 0; i < vecPointsForSpline.size(); ++i)
    {
        double tmpPoint[3];

        imageCurrentData_->TransformIndexToPhysicalPoint(reinterpret_cast<int*>(&vecPointsForSpline[i]), tmpPoint);
        points->InsertNextPoint(tmpPoint);
    }

    vtkNew<vtkParametricSpline> spline;
    spline->SetPoints(points);

    vtkNew<vtkParametricFunctionSource> functionSource;
    functionSource->SetParametricFunction(spline);
    functionSource->Update();
    vtkNew<vtkPolyDataMapper> splineMapper;
    splineMapper->SetInputConnection(functionSource->GetOutputPort());
    vtkNew<vtkActor> splineActor;
    splineActor->SetMapper(splineMapper);
    splineActor->GetProperty()->SetLineWidth(2.0f);
    splineActor->GetProperty()->SetColor(1.0, 0.0, 0.0);
    _renderer->AddActor(splineActor);

    _renderer->ResetCamera();
    renderWindow()->Render();
}

void SliceOrientationXY::setupRender()
{
    // Setup rendering stuff
    _renderer->SetBackground(_backgroundColor);

    // link render and render window
    _renderWindow->SetSize(_renderWindow->GetScreenSize());

    // Setup interactor style
    renderWindow()->GetInteractor()->SetRenderWindow(_renderWindow);
    renderWindow()->AddRenderer(_renderer);
}
