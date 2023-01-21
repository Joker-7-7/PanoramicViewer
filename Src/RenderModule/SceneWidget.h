#ifndef SCENEWIDGET_H
#define SCENEWIDGET_H

#include "Src/GraphicPrimitives/Point3D.hpp"

#include <QVTKOpenGLNativeWidget.h>

#include <vtkDataSet.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <QVTKOpenGLNativeWidget.h>
#include <vtkDICOMImageReader.h>
#include <vtkDICOMReader.h>
#include <vtkColorTransferFunction.h>
#include <vtkPiecewiseFunction.h>
#include <vtkRenderer.h>
#include <vtkVolume.h>
#include <vtkVolumeProperty.h>
#include <vtkNew.h>
#include <vtkBoxWidget2.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkCamera.h>
#include <vtkDataSetMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkGPUVolumeRayCastMapper.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkBoxRepresentation.h>
#include <vtkImageData.h>
#include <vtkParametricSpline.h>

class SceneWidget : public QVTKOpenGLNativeWidget {
    Q_OBJECT
public:
    // window refresh rate when rotating the model
    const double _desiredUpdateRate;
    // window background color
    const double _backgroundColor[3];
    // a renderer is an object that controls the rendering process for objects
    vtkNew<vtkRenderer> _renderer;
    // create a window for renderers to draw into
    vtkNew<vtkGenericOpenGLRenderWindow> _renderWindow;
    // represents a volume (data & properties) in a rendered scene
    vtkNew<vtkVolume> _volume;
    // represents the common properties for rendering a volume
    vtkNew<vtkVolumeProperty> _volumeProperty;
    // reader for first buffer
    vtkSmartPointer<vtkImageReader2> _reader;

    std::vector<GraphicPrimitives::Point3D> vecPointsForSpline;
    explicit SceneWidget(QWidget* parent = nullptr);

    /// <summary>
    /// Add a data set to the scene
    /// </summary>
    /// <param name="param[in] dataSet The data set to add"></param>
    void AddDataSet(vtkSmartPointer<vtkImageReader2> dataSet);
    /// <summary>
    /// Setup readers parameters for m_ptrReader and m_ptrPreReader
    /// </summary>
    /// <param name="reader"></param>
    void SetupReader(vtkSmartPointer<vtkImageReader2> reader);
    /// <summary>
    /// Setup m_ptrRenderer, m_ptrRenderWindow, m_ptrIinteractor
    /// </summary>
    void SetupRender();
    /// <summary>
    /// Setup vtkGPUVolumeRayCastMapper, vtkColorTransferFunction, vtkPiecewiseFunction
    /// </summary>
    void SetupProperty();
    /// <summary>
    /// Remove the data set from the scene
    /// </summary>
    void RemoveDataSet();

    void GeneratePanoramicView(vtkParametricSpline* spline);

    void CreateSpline();

    void SetRentgenEffects();

    void SetMaxIntensity();

    void SetMinIntensity();

    void SetSolidEffects();
public slots:
    /**
     * Zoom to the extent of the data set in the scene
     */
    void ZoomToExtent();
};

#endif
