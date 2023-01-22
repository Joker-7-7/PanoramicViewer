#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <vtkGlobFileNames.h>
#include <vtkDICOMDirectory.h>
#include <vtkDICOMImageReader.h>
#include <vtkDICOMReader.h>

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
    * File open status
    */
    enum class OpeningStatus
    {
        Error,
        Success,
        Cancel
    };
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();

public slots:
    /**
     * Show the 'About this application' dialog
     */
    void showAboutDialog();

    /**
     * Show the 'Open file...' dialog
     */
    void showOpenFileDialog();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_verticalSlider_sliderMoved(int position);

    void on_pushButton_5_clicked();

private:
    Ui::MainWindow* ui;

    /**
     * Open a file
     *
     * @param fileName The name of the file including the path
     */
    void OpenData(const QString& fileName);

    /**
    * Open single DICOM file
    *
    * @return <code>OpeningStatus::Success</code> file opened successfully
    *         <code>OpeningStatus::Error</code> opening error
    * @see    #OpeningStatus
    */
    [[nodiscard]] OpeningStatus OpenSingleFile();

    /**
    * Directory with DICOM files
    *
    * @return <code>OpeningStatus::Success</code> directory opened successfully
    *         <code>OpeningStatus::Error</code> opening error
    * @see    #OpeningStatus
    */
    [[nodiscard]] OpeningStatus OpenDirectory();

    /**
    * Check the validity of the reader and add data to the scene
    *
    * @param  reader  reader with selected file
    * @param  dataSet dataset for render
    * @return         <code>OpeningStatus::Success</code> successful opening
    *                 <code>OpeningStatus::Error</code> opening error
    * @see            #OpeningStatus
    */
    [[nodiscard]] OpeningStatus CheckReader(vtkSmartPointer<vtkDICOMReader> reader, vtkSmartPointer<vtkImageReader2> dataSet);

    /**
    * Add a data set to the scene
    *
    * @param dataSet reader with dats
    */
    void AddDataSet(vtkSmartPointer<vtkImageReader2> dataSet);
};

#endif
