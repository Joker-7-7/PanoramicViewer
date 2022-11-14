#include "scenewidget.h"

#include <vtkCamera.h>
#include <vtkDataSetMapper.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkProperty.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSplineDrivenImageSlicer.h>
#include <vtkImageAppend.h>
#include <vtkInteractorStyleRubberBandZoom.h>

#include <vtkImageEllipsoidSource.h>
#include <vtkImageData.h>
#include <vtkLineSource.h>
#include <vtkParametricSpline.h>
#include <vtkParametricFunctionSource.h>
#include <vtkDICOMReader.h>
#include <vtkGlobFileNames.h>
#include <vtkDICOMFileSorter.h>
#include <vtkCamera.h>
#include <vtkCommand.h>

SceneWidget::SceneWidget(QWidget* parent)
    : QVTKOpenGLNativeWidget(parent), _desiredUpdateRate(30.0), _backgroundColor{ 0, 0, 0 }
{
    setRenderWindow(_renderWindow.Get());
    setupRender();
    setupProperty();


    int Z = 225;
    vecPointsForSpline.emplace_back(Point{ 129, 385, Z });
    vecPointsForSpline.emplace_back(Point{ 148, 323, Z });
    vecPointsForSpline.emplace_back(Point{ 157, 290, Z });
    vecPointsForSpline.emplace_back(Point{ 165, 250, Z });
    vecPointsForSpline.emplace_back(Point{ 167, 204, Z });
    vecPointsForSpline.emplace_back(Point{ 177, 162, Z });
    vecPointsForSpline.emplace_back(Point{ 187, 130, Z });
    vecPointsForSpline.emplace_back(Point{ 200, 93, Z });
    vecPointsForSpline.emplace_back(Point{ 227, 65, Z });
    vecPointsForSpline.emplace_back(Point{ 263, 56, Z });
    vecPointsForSpline.emplace_back(Point{ 286, 54, Z });
    vecPointsForSpline.emplace_back(Point{ 316, 58, Z });
    vecPointsForSpline.emplace_back(Point{ 347, 79, Z });
    vecPointsForSpline.emplace_back(Point{ 367, 130, Z });
    vecPointsForSpline.emplace_back(Point{ 377, 165, Z });
    vecPointsForSpline.emplace_back(Point{ 384, 203, Z });
    vecPointsForSpline.emplace_back(Point{ 394, 249, Z });
    vecPointsForSpline.emplace_back(Point{ 409, 310, Z });
    vecPointsForSpline.emplace_back(Point{ 420, 344, Z });
}


void SceneWidget::generateSpline()
{
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

    int numberOfPoints = 20;
    vtkNew<vtkParametricFunctionSource> functionSource;
    functionSource->SetParametricFunction(spline);
    functionSource->SetUResolution(10 * numberOfPoints);
    functionSource->SetVResolution(10 * numberOfPoints);
    functionSource->SetWResolution(10 * numberOfPoints);
    functionSource->Update();

    vtkSmartPointer<vtkSplineDrivenImageSlicer> sdis;
    sdis = vtkSmartPointer<vtkSplineDrivenImageSlicer>::New();
    sdis->SetInputConnection(0, _reader->GetOutputPort());
    sdis->SetInputConnection(1, functionSource->GetOutputPort());
    sdis->SetIncidence(vtkMath::Pi() / 2);
    //sdis->SetOffsetPoint(10);
    sdis->SetSliceExtent(300, 500);
    sdis->SetSliceSpacing(0.15, 0.15);
    sdis->Update();

    vtkSmartPointer<vtkImageAppend> append =
        vtkSmartPointer<vtkImageAppend>::New();

    int nbPoints = numberOfPoints * 10;
    for (int ptId = 0; ptId < nbPoints; ptId++)
    {
        sdis->SetOffsetPoint(ptId);

        sdis->Update();

        vtkSmartPointer<vtkImageData> tempSlice =
            vtkSmartPointer<vtkImageData>::New();
        tempSlice->DeepCopy(sdis->GetOutput(0));
        //tempSlice->SetDirectionMatrix(matrix);
        append->AddInputData(tempSlice);

    }
    append->SetAppendAxis(2);
    append->Update();



    vtkNew<vtkGPUVolumeRayCastMapper> mapper;;
    mapper->SetInputConnection(append->GetOutputPort());
    mapper->SetMaximumImageSampleDistance(1.0);
    mapper->UseJitteringOn();


    _volume->SetMapper(mapper);

    _renderer->AddVolume(_volume);
    _renderer->ResetCamera();
}
void SceneWidget::addDataSet(vtkSmartPointer<vtkImageReader2> reader)
{
    removeDataSet();

    setupReader(reader);

    generateSpline();

    setupProperty();

    renderWindow()->Render();
}

void SceneWidget::removeDataSet()
{
    vtkProp* volume = _renderer->GetVolumes()->GetLastProp();
    if (volume != nullptr) {
        _renderer->RemoveVolume(volume);
    }
}

void SceneWidget::zoomToExtent()
{
    _renderer->ResetCamera();
    renderWindow()->Render();
}
void SceneWidget::setupRender()
{
    // Setup rendering stuff
    _renderer->SetBackground(_backgroundColor);

    // link render and render window
    _renderWindow->SetSize(_renderWindow->GetScreenSize());
    //l_render_windows->SetSize(900, 900);

    // Setup interactor style
    renderWindow()->GetInteractor()->SetRenderWindow(_renderWindow);
    //renderWindow()->GetInteractor()->GetInteractorStyle()->SetDefaultRenderer(m_ptrRenderer);
    renderWindow()->AddRenderer(_renderer);
    renderWindow()->GetInteractor()->SetDesiredUpdateRate(_desiredUpdateRate);

    vtkSmartPointer<vtkInteractorStyleRubberBandZoom> style =
        vtkSmartPointer<vtkInteractorStyleRubberBandZoom>::New();
    style->PickingManagedOff();
    renderWindow()->GetInteractor()->SetInteractorStyle(style);

    _renderer->GetActiveCamera()->Azimuth(90);
}

void SceneWidget::setupProperty()
{
    double wl_ = 1830.0;
    double ww_ = 140.0;
    // Setup Volume property
    vtkNew<vtkColorTransferFunction> m_ptrColorFunction;
    vtkNew<vtkPiecewiseFunction> m_ptrOpacityFunction;
    m_ptrColorFunction->SetColorSpaceToRGB();
    m_ptrColorFunction->AddRGBPoint(wl_ - ww_ / 2.0, 0.0, 0.0, 0.0);
    m_ptrColorFunction->AddRGBPoint(wl_ - ww_ / 2.0 + 94.0 * (ww_ / 255.0), 1.0, 21.0 / 255.0, 27.0 / 255.0);
    m_ptrColorFunction->AddRGBPoint(wl_ - ww_ / 2.0 + 147.0 * (ww_ / 255.0), 1.0, 176.0 / 255.0, 9.0 / 255.0);
    m_ptrColorFunction->AddRGBPoint(wl_ - ww_ / 2.0 + 201.0 * (ww_ / 255.0), 1.0, 241.0 / 255.0, 39.0 / 255.0);
    m_ptrColorFunction->AddRGBPoint(wl_ - ww_ / 2.0 + 255.0 * (ww_ / 255.0), 1.0, 1.0, 1.0);

    m_ptrOpacityFunction->AddPoint(wl_ - ww_ / 2.0, 0.0);
    m_ptrOpacityFunction->AddPoint(wl_ + ww_ / 2.0, 1.0);
    _volumeProperty->SetColor(m_ptrColorFunction);
    _volumeProperty->SetScalarOpacity(m_ptrOpacityFunction);
    _volumeProperty->SetInterpolationTypeToLinear();
    _volumeProperty->ShadeOn();
    _volumeProperty->SetAmbient(0.15);
    _volumeProperty->SetDiffuse(0.8);
    _volumeProperty->SetSpecular(0.25);
    _volumeProperty->SetSpecularPower(40);

    _volume->SetProperty(_volumeProperty);
}

void SceneWidget::setRentgenEffects()
{
    vtkVolumeProperty* volumeProperty_ = _volume->GetProperty();

    vtkColorTransferFunction* colFun = volumeProperty_->GetRGBTransferFunction();
    colFun->RemoveAllPoints();
    vtkPiecewiseFunction* opacityFun = volumeProperty_->GetScalarOpacity();
    opacityFun->RemoveAllPoints();

    vtkPiecewiseFunction* volumeGradientOpacity = volumeProperty_->GetGradientOpacity();

    volumeGradientOpacity->AddPoint(0, 0.0);
    volumeGradientOpacity->AddPoint(90, 0.5);
    volumeGradientOpacity->AddPoint(100, 0.25);

    opacityFun->AddPoint(1000, 0.00);
    opacityFun->AddPoint(1400, 0.05);
    opacityFun->AddPoint(1800, 0.05);
    opacityFun->AddPoint(2150, 0.25);//don't want it to be

    colFun->AddRGBPoint(1000, 0.0, 0.0, 0.0);
    colFun->AddRGBPoint(1400, 1.0, 0.5, 0.3);
    colFun->AddRGBPoint(1800, 1.0, 0.5, 0.3);
    colFun->AddRGBPoint(2150, 1.0, 1.0, 0.9);

    volumeProperty_->SetAmbient(0.6);
    volumeProperty_->SetDiffuse(0.9);
    volumeProperty_->SetSpecular(0.5);
}

void SceneWidget::setSolidEffects()
{
    double wl_ = 2100;
    double ww_ = 1000;

    vtkVolumeProperty* volumeProperty_ = _volume->GetProperty();

    vtkColorTransferFunction* colFun = volumeProperty_->GetRGBTransferFunction();
    colFun->RemoveAllPoints();
    vtkPiecewiseFunction* opacityFun = volumeProperty_->GetScalarOpacity();
    opacityFun->RemoveAllPoints();

    vtkPiecewiseFunction* volumeGradientOpacity = volumeProperty_->GetGradientOpacity();

    opacityFun->AddPoint(wl_ - ww_ / 2.0, 0.0);
    opacityFun->AddPoint(wl_ + ww_ / 2.0, 1.0);

    volumeGradientOpacity->AddPoint(0, 0.0);
    volumeGradientOpacity->AddPoint(90, 0.5);
    volumeGradientOpacity->AddPoint(100, 1.0);

    colFun->AddRGBPoint(wl_ - ww_ / 2.0, 0.0, 0.0, 0.0);
   // colorTransferFunction->AddRGBPoint(wl_ - ww_ / 2.0 + 94.0 * (ww_ / 255.0), 1.0, 21.0 / 255.0, 27.0 / 255.0);
  //  colorTransferFunction->AddRGBPoint(wl_ - ww_ / 2.0 + 147.0 * (ww_ / 255.0), 1.0, 176.0 / 255.0, 9.0 / 255.0);
   // colorTransferFunction->AddRGBPoint(wl_ - ww_ / 2.0 + 201.0 * (ww_ / 255.0), 1.0, 241.0 / 255.0, 39.0 / 255.0);
    colFun->AddRGBPoint(wl_ - ww_ / 2.0 + 255.0 * (ww_ / 255.0), 1.0, 1.0, 1.0);

    volumeProperty_->SetAmbient(0.6);
    volumeProperty_->SetDiffuse(0.9);
    volumeProperty_->SetSpecular(0.5);
}

void SceneWidget::setMaxIntensity()
{


    vtkVolumeProperty* volumeProperty_ = _volume->GetProperty();

    vtkColorTransferFunction* colFun = volumeProperty_->GetRGBTransferFunction();
    colFun->RemoveAllPoints();
    vtkPiecewiseFunction* opacityFun = volumeProperty_->GetScalarOpacity();
    opacityFun->RemoveAllPoints();

    vtkPiecewiseFunction* volumeGradientOpacity = volumeProperty_->GetGradientOpacity();
    opacityFun->AddPoint(1000, 0.00);
    opacityFun->AddPoint(1400, 0.05);
    opacityFun->AddPoint(1800, 0.05);
    opacityFun->AddPoint(2150, 0.25);//don't want it to be

    volumeGradientOpacity->AddPoint(0, 0.0);
    volumeGradientOpacity->AddPoint(90, 0.5);
    volumeGradientOpacity->AddPoint(100, 1.0);

    colFun->AddRGBPoint(1000, 0.0, 0.0, 0.0);
    //colorTransferFunction->AddRGBPoint(1400, 1.0, 0.5, 0.3);
  //  colorTransferFunction->AddRGBPoint(1800, 1.0, 0.5, 0.3);
    colFun->AddRGBPoint(2150, 1.0, 1.0, 0.9);

    volumeProperty_->SetAmbient(0.9);
    volumeProperty_->SetDiffuse(0.9);
    volumeProperty_->SetSpecular(0.1);
    volumeProperty_->SetSpecularPower(1);
}

void SceneWidget::setMinIntensity()
{


    vtkVolumeProperty* volumeProperty_ = _volume->GetProperty();

    vtkColorTransferFunction* colFun = volumeProperty_->GetRGBTransferFunction();
    colFun->RemoveAllPoints();
    vtkPiecewiseFunction* opacityFun = volumeProperty_->GetScalarOpacity();
    opacityFun->RemoveAllPoints();

    vtkPiecewiseFunction* volumeGradientOpacity = volumeProperty_->GetGradientOpacity();
    opacityFun->AddPoint(1000, 0.00);
    opacityFun->AddPoint(1400, 0.05);

    volumeGradientOpacity->AddPoint(0, 0.0);
    volumeGradientOpacity->AddPoint(90, 0.5);
    volumeGradientOpacity->AddPoint(100, 1.0);

    colFun->AddRGBPoint(1000, 0.0, 0.0, 0.0);
    //colorTransferFunction->AddRGBPoint(1400, 1.0, 0.5, 0.3);
  //  colorTransferFunction->AddRGBPoint(1800, 1.0, 0.5, 0.3);
    colFun->AddRGBPoint(2150, 1.0, 1.0, 0.9);

    volumeProperty_->SetAmbient(0.9);
    volumeProperty_->SetDiffuse(0.4);
    volumeProperty_->SetSpecular(0.05);
    volumeProperty_->SetSpecularPower(100);
}

void SceneWidget::setupReader(vtkSmartPointer<vtkImageReader2> reader)
{
    // save reader in first buffer
    _reader = reader;
    // create new reader for second buffer

}
