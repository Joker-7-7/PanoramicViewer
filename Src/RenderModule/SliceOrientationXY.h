#ifndef SLICEORIENTATIONXY_H
#define SLICEORIENTATIONXY_H

#include <QVTKOpenGLNativeWidget.h>
#include <vtkImageReader2.h>
#include <vtkNew.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkResliceImageViewer.h>
#include <vtkSplineWidget.h>
#include <vtkPoints.h>
#include <vtkParametricSpline.h>
#include <vtkParametricFunctionSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkImageData.h>

#include "SceneWidget.h"

class SliceOrientationXY : public QVTKOpenGLNativeWidget {
    Q_OBJECT

public:
    struct PointSpline
    {
        int x;
        int y;
        int z;
    };

    enum class SplineVisibility : bool
    {
        VisibilityOn,
        VisibilityOff
    };

    // window background color
    const double _backgroundColor[3];
    // a renderer is an object that controls the rendering process for slice
    vtkNew<vtkRenderer> _renderer;
    vtkNew<vtkSplineWidget> splineWidget;
    vtkNew<vtkResliceImageViewer> _reslicer;
    // create a window for renderers to draw slice XY into 
    vtkNew<vtkGenericOpenGLRenderWindow> _renderWindow;
    // reader for DICOM 
    vtkSmartPointer<vtkImageReader2> _reader;

    SplineVisibility _visibilitySpline;
    vtkNew<vtkRenderWindowInteractor> _renderWindowInteractor;
    vtkNew<vtkRenderWindowInteractor> _renderInteractorForSpline;

    SceneWidget* _panoramicView;
public:
    explicit SliceOrientationXY(QWidget* parent = nullptr);

    /// <summary>
    /// Add a data set to the scene
    /// </summary>
    /// <param name="param[in] dataSet The data set to add"></param>
    void addDataSet(vtkSmartPointer<vtkImageReader2> dataSet, SceneWidget* panoramicView);

    /// <summary>
    /// Remove the data set from the scene
    /// </summary>
    void removeDataSet();

    /// <summary>
    /// Setup m_ptrRenderer, m_ptrRenderWindow, m_ptrIinteractor
    /// </summary>
    void setupRender();
    void setSplineWidget();
    void CreateSpline();
    void CreateSplineModifiCallback();

};

#endif
