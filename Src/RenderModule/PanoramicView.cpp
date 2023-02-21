#include "PanoramicView.h"
#include "Src/ThirdParty/vtkSplineDrivenImageSlicer.h"

#include <vtkCamera.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkImageAppend.h>
#include <vtkInteractorStyleRubberBandZoom.h>
#include <vtkImageData.h>
#include <vtkParametricSpline.h>
#include <vtkParametricFunctionSource.h>
#include <vtkDICOMReader.h>

PanoramicView::PanoramicView(QWidget* parent) :
    QVTKOpenGLNativeWidget(parent),
    _backgroundColor{ 0, 0, 0 }
{
    setRenderWindow(_renderWindow.Get());
    SetupRender();
    SetupProperty();
    CreateSpline();
}

void PanoramicView::CreateSpline() {
    constexpr double Z = 225;
    _splineVertices.insert(_splineVertices.cend(), {
            {29, 385, Z},
            {148, 323, Z},
            {157, 290, Z},
            {165, 250, Z},
            {167, 204, Z},
            {177, 162, Z},
            {187, 130, Z},
            {200, 93, Z},
            {227, 65, Z},
            {263, 56, Z},
            {286, 54, Z},
            {316, 58, Z},
            {347, 79, Z},
            {367, 130, Z},
            {377, 165, Z},
            {384, 203, Z},
            {394, 249, Z},
            {409, 310, Z},
            {420, 344, Z},
            {443, 385, Z}
    });
}

void PanoramicView::GeneratePanoramicView(vtkParametricSpline* spline) {
    //vtkImageData* imageCurrentData_ = _reader->GetOutput();
    //vtkNew<vtkPoints> points;
    //for (int i = 0; i < _splineVertices.size(); ++i)
    //{
    //    double tmpPoint[3];

    //    imageCurrentData_->TransformIndexToPhysicalPoint(reinterpret_cast<int*>(&_splineVertices[i]), tmpPoint);
    //    points->InsertNextPoint(tmpPoint);
    //}

    //vtkNew<vtkParametricSpline> spline;
    //spline->SetPoints(points);

    int numberOfPoints = 20;
    vtkNew<vtkParametricFunctionSource> functionSource;
    functionSource->SetParametricFunction(spline);
    functionSource->SetUResolution(10 * numberOfPoints);
    functionSource->SetVResolution(10 * numberOfPoints);
    functionSource->SetWResolution(10 * numberOfPoints);
    functionSource->Update();

    auto imageSlicer = vtkSmartPointer<vtkSplineDrivenImageSlicer>::New();
    imageSlicer->SetInputConnection(0, _reader->GetOutputPort());
    imageSlicer->SetInputConnection(1, functionSource->GetOutputPort());
    imageSlicer->SetIncidence(vtkMath::Pi() / 2);
    //imageSlicer->SetOffsetPoint(10);
    imageSlicer->SetSliceExtent(300, 500);
    imageSlicer->SetSliceSpacing(0.15, 0.15);
    imageSlicer->Update();

    auto imageStitching = vtkSmartPointer<vtkImageAppend>::New();

    int nbPoints = numberOfPoints * 10;
    for (int ptId = 0; ptId < nbPoints; ptId++) {
        imageSlicer->SetOffsetPoint(ptId);
        imageSlicer->Update();

        auto tempSlice = vtkSmartPointer<vtkImageData>::New();
        tempSlice->DeepCopy(imageSlicer->GetOutput(0));
        imageStitching->AddInputData(tempSlice);
    }
    imageStitching->SetAppendAxis(2);
    imageStitching->Update();

    //vtkNew<vtkGPUVolumeRayCastMapper> mapper;;
    //mapper->SetInputConnection(imageStitching->GetOutputPort());
    //mapper->SetMaximumImageSampleDistance(1.0);
    //mapper->UseJitteringOn();

    _volume->GetMapper()->SetInputConnection(imageStitching->GetOutputPort());

    _renderer->AddVolume(_volume);
    _renderer->ResetCamera();
    renderWindow()->Render();
}
void PanoramicView::AddDataSet(vtkSmartPointer<vtkImageReader2> dataSet) {
    RemoveDataSet();
    SetupReader(dataSet);
    SetupProperty();

    vtkNew<vtkGPUVolumeRayCastMapper> mapper;;
    mapper->SetMaximumImageSampleDistance(1.0);
    mapper->UseJitteringOn();

    _volume->SetMapper(mapper);
}

void PanoramicView::RemoveDataSet() {
    vtkProp* volume = _renderer->GetVolumes()->GetLastProp();
    if (volume != nullptr) {
        _renderer->RemoveVolume(volume);
    }
}

void PanoramicView::ZoomToExtent() {
    _renderer->ResetCamera();
    renderWindow()->Render();
}

void PanoramicView::SetupRender() {
    // Setup rendering stuff
    _renderer->SetBackground(_backgroundColor);

    // link render and render window
    _renderWindow->SetSize(_renderWindow->GetScreenSize());

    // Setup interactor style
    renderWindow()->GetInteractor()->SetRenderWindow(_renderWindow);
    renderWindow()->AddRenderer(_renderer);

    auto style = vtkSmartPointer<vtkInteractorStyleRubberBandZoom>::New();
    style->PickingManagedOff();
    renderWindow()->GetInteractor()->SetInteractorStyle(style);

    _renderer->GetActiveCamera()->Azimuth(90);
}

void PanoramicView::SetupProperty() {
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

void PanoramicView::SetRentgenEffects() {
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
    opacityFun->AddPoint(2150, 0.25);

    colFun->AddRGBPoint(1000, 0.0, 0.0, 0.0);
    colFun->AddRGBPoint(1400, 1.0, 0.5, 0.3);
    colFun->AddRGBPoint(1800, 1.0, 0.5, 0.3);
    colFun->AddRGBPoint(2150, 1.0, 1.0, 0.9);

    volumeProperty_->SetAmbient(0.6);
    volumeProperty_->SetDiffuse(0.9);
    volumeProperty_->SetSpecular(0.5);
}

void PanoramicView::SetSolidEffects() {
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

void PanoramicView::SetMaxIntensity() {

    vtkVolumeProperty* volumeProperty_ = _volume->GetProperty();

    vtkColorTransferFunction* colFun = volumeProperty_->GetRGBTransferFunction();
    colFun->RemoveAllPoints();
    vtkPiecewiseFunction* opacityFun = volumeProperty_->GetScalarOpacity();
    opacityFun->RemoveAllPoints();

    vtkPiecewiseFunction* volumeGradientOpacity = volumeProperty_->GetGradientOpacity();
    opacityFun->AddPoint(1000, 0.00);
    opacityFun->AddPoint(1400, 0.05);
    opacityFun->AddPoint(1800, 0.05);
    opacityFun->AddPoint(2150, 0.25);

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

void PanoramicView::SetMinIntensity() {

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

void PanoramicView::SetupReader(vtkSmartPointer<vtkImageReader2> reader) {
    _reader = reader;
}
