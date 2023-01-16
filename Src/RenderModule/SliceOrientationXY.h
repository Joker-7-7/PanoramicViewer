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
    /**
     * 3D point for spline
     */
    struct PointSpline
    {
        int x;
        int y;
        int z;
    };

    /**
     * Spline visibility
     */
    enum class SplineVisibility : bool
    {
        VisibilityOn,
        VisibilityOff
    };

    /**
     * window background color
     */
    const double _backgroundColor[3];

    /**
     * A renderer is an object that controls the rendering process for slice
     */
    vtkNew<vtkRenderer> _renderer;

    /**
     * Spline along which a panoramic reconstruction is built
     */
    vtkNew<vtkSplineWidget> splineWidget;

    /**
     * Slicer for moving between DICOM layers
     */
    vtkNew<vtkResliceImageViewer> _reslicer;

    /**
     * Create a window for renderers to draw slice XY into
     */
    vtkNew<vtkGenericOpenGLRenderWindow> _renderWindow;

    /**
     * Reader for DICOM
     */
    vtkSmartPointer<vtkImageReader2> _reader;

    /**
     * Spline visibility
     */
    SplineVisibility _visibilitySpline;

    /**
     * Render window interaction for slicer
     */
    vtkNew<vtkRenderWindowInteractor> _renderWindowInteractor;

    /**
     * Render window interaction for spline
     */
    vtkNew<vtkRenderWindowInteractor> _renderInteractorForSpline;

    /**
     * Panoramic reconstruction
     */
    SceneWidget* _panoramicView;
public:
    explicit SliceOrientationXY(QWidget* parent = nullptr);

    /**
     * Add a data set to the scene
     *
     * @param dataSet data
     * @param panoramicView panoramic reconstruction
     */
    void addDataSet(vtkSmartPointer<vtkImageReader2> dataSet, SceneWidget* panoramicView);

    /**
     * Remove the data set from the scene
     */
    void removeDataSet();

    /**
     * Setup render scene
     */
    void setupRender();

    /**
     * Change spline visibility
     */
    void setSplineWidget();

    /**
     * Build spline on DICOM layer
     */
    void CreateSpline();

    /**
     * Rebuilding the spline after changing the position of the vertex
     *
     * @note the method for callback
     */
    void CreateSplineModifiCallback();
};

#endif
