#include "SliceOrientationXY.h"
#include <QCoreApplication.h>
#include <vtkCallbackCommand.h>

namespace
{
    void SplinePanoramicUpdateCallback(vtkObject* caller, long unsigned int vtkNotUsed(eventId),
                                       void* clientData, void* vtkNotUsed(callData))
    {
        vtkSplineWidget* splineWidget = static_cast<vtkSplineWidget*>(caller);
        SceneWidget* panoramicView = static_cast<SceneWidget*>(clientData);

        panoramicView->generatePanoramicView(splineWidget->GetParametricSpline());
    }
}

SliceOrientationXY::SliceOrientationXY(QWidget* parent)
    : QVTKOpenGLNativeWidget(parent), _backgroundColor{ 0.0, 0.0, 0. }, _visibilitySpline(SplineVisibility::VisibilityOn)
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


void SliceOrientationXY::addDataSet(vtkSmartPointer<vtkImageReader2> reader, SceneWidget* panoramicView)
{
    _panoramicView = panoramicView;
    removeDataSet();
    _reader = reader;

    _reslicer->SetupInteractor(_renderWindowInteractor);
    _reslicer->SetRenderWindow(_renderWindow);
    _reslicer->SliceScrollOnMouseWheelOff();
    _reslicer->SetSliceOrientationToXY();
    _reslicer->SetRenderer(_renderer);
    _reslicer->SetInputData(_reader->GetOutput());
    _reslicer->SetResliceModeToAxisAligned();
    _reslicer->SetColorLevel(40.0);
    _reslicer->SetSlice(_reslicer->GetSliceMax() / 2);
    _reslicer->SetColorWindow(10000.0);
    _reslicer->SetResliceMode(0);


    CreateSpline();
    CreateSplineModifiCallback();
    
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


void SliceOrientationXY::CreateSplineModifiCallback()
{
    splineWidget->RemoveAllObservers();
    vtkNew<vtkCallbackCommand> splineCallback;
    splineCallback->SetClientData(_panoramicView);
    splineCallback->SetCallback(::SplinePanoramicUpdateCallback);
    splineWidget->AddObserver(vtkCommand::EndInteractionEvent, splineCallback);
}



void SliceOrientationXY::CreateSpline()
{
    int Z = _reslicer->GetSliceMax()/2;;// _reslicer->GetSliceMax();

    std::vector<PointSpline> vecPointsForSpline;
    vecPointsForSpline.emplace_back(PointSpline{ 129, 385, Z });
   // vecPointsForSpline.emplace_back(PointSpline{ 148, 323, Z });
    vecPointsForSpline.emplace_back(PointSpline{ 157, 290, Z });
   // vecPointsForSpline.emplace_back(PointSpline{ 165, 250, Z });
    vecPointsForSpline.emplace_back(PointSpline{ 167, 204, Z });
   // vecPointsForSpline.emplace_back(PointSpline{ 177, 162, Z });
    vecPointsForSpline.emplace_back(PointSpline{ 187, 130, Z });
    //vecPointsForSpline.emplace_back(PointSpline{ 200, 93, Z });
    vecPointsForSpline.emplace_back(PointSpline{ 227, 65, Z });
    //vecPointsForSpline.emplace_back(PointSpline{ 263, 56, Z });
    vecPointsForSpline.emplace_back(PointSpline{ 286, 54, Z });
    //vecPointsForSpline.emplace_back(PointSpline{ 316, 58, Z });
    vecPointsForSpline.emplace_back(PointSpline{ 347, 79, Z });
    vecPointsForSpline.emplace_back(PointSpline{ 367, 130, Z });
    //vecPointsForSpline.emplace_back(PointSpline{ 377, 165, Z });
    vecPointsForSpline.emplace_back(PointSpline{ 384, 203, Z });
  //  vecPointsForSpline.emplace_back(PointSpline{ 394, 249, Z });
    vecPointsForSpline.emplace_back(PointSpline{ 409, 310, Z });
  //  vecPointsForSpline.emplace_back(PointSpline{ 420, 344, Z });
    vecPointsForSpline.emplace_back(PointSpline{ 443, 385, Z });

    vtkImageData* imageCurrentData_ = _reader->GetOutput();
    vtkNew<vtkPoints> points;
    for (int i = 0; i < vecPointsForSpline.size(); ++i)
    {
        double tmpPoint[3];

        imageCurrentData_->TransformIndexToPhysicalPoint(reinterpret_cast<int*>(&vecPointsForSpline[i]), tmpPoint);
        points->InsertNextPoint(tmpPoint);
    }

    _renderInteractorForSpline->SetRenderWindow(_renderWindow);
    splineWidget->SetInteractor(_renderInteractorForSpline);
    splineWidget->InitializeHandles(points);
    splineWidget->SetCurrentRenderer(_renderer);
    splineWidget->SetHandleSize(0.008);
    splineWidget->On();

    _renderer->ResetCamera();
    renderWindow()->Render();
}


void SliceOrientationXY::setSplineWidget()
{
    if (_visibilitySpline == SplineVisibility::VisibilityOff)
    {
        splineWidget->On();
        _visibilitySpline = SplineVisibility::VisibilityOn;
    }
    else
    {
        splineWidget->Off();
        _visibilitySpline = SplineVisibility::VisibilityOff;
    }
    _renderer->ResetCamera();
    renderWindow()->Render();
}