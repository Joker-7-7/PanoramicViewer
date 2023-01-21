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

/**
 * Multi-Planar Reconstruction
 *
 * <p>
 * Used to build the MPR along the curve from the DICOM file
 * <p>
 *
* @author Joker-7-7
* @since  0.1.0
 */
class PanoramicView : public QVTKOpenGLNativeWidget {
    Q_OBJECT
public:
    /**
    * Window refresh rate when rotating the model
    */
    const double _desiredUpdateRate;

    /**
     * Window background color
     */
    const double _backgroundColor[3];

    /**
     * A renderer is an object that controls the rendering process for objects
     */
    vtkNew<vtkRenderer> _renderer;

    /**
     * Create a window for renderers to draw into
     */
    vtkNew<vtkGenericOpenGLRenderWindow> _renderWindow;

    /**
     * Represents a volume (data & properties) in a rendered scene
     */
    vtkNew<vtkVolume> _volume;

    /**
     * Represents the common properties for rendering a volume
     */
    vtkNew<vtkVolumeProperty> _volumeProperty;

    /**
     * Reader for DICOM file
     */
    vtkSmartPointer<vtkImageReader2> _reader;

    /**
     * Array for spline vertices
     */
    std::vector<GraphicPrimitives::Point3D> vecPointsForSpline;

    /**
     * Constructs Panoramic View
     *
     * @param parent Qt widget
     */
    explicit PanoramicView(QWidget* parent = nullptr);

    /**
     * Add a data set to the scene
     *
     * @param dataSet the data set to add
     */
    void AddDataSet(vtkSmartPointer<vtkImageReader2> dataSet);

    /**
     * Setup reader parameters
     *
     * @param reader installable reader
     */
    void SetupReader(vtkSmartPointer<vtkImageReader2> reader);

    /**
     * Setup renderer, renderWindow, interactor
     */
    void SetupRender();

    /**
     * Setting initial properties
     */
    void SetupProperty();

    /**
     * Remove the data set from the scene
     */
    void RemoveDataSet();

    /**
    * Generate MPR
    *
    * @param spline spline along which the MPR is generated
    */
    void GeneratePanoramicView(vtkParametricSpline* spline);

    /**
     * Create default spline for initial build
     */
    void CreateSpline();

    /**
     * Set rentgen effects for MPR
     */
    void SetRentgenEffects();

    /**
     * Set max intensity effects for MPR
     */
    void SetMaxIntensity();

    /**
     * Set min intensity effects for MPR
     */
    void SetMinIntensity();

    /**
     * Set solid effects for MPR
     */
    void SetSolidEffects();

public slots:
    /**
     * Zoom to the extent of the data set in the scene
     */
    void ZoomToExtent();
};

#endif
