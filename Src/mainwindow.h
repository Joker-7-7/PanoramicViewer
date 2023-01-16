#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <vtkDataSetReader.h>
#include <vtkGlobFileNames.h>
#include <vtkDICOMFileSorter.h>
#include <vtkImageReader2.h>
#include <vtkDICOMReader.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    void addDataSet(vtkSmartPointer<vtkImageReader2> dataSet);
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();

public slots:
    //! Show the 'About this application' dialog
    void showAboutDialog();

    //! Show the 'Open file...' dialog
    void showOpenFileDialog();

protected:
    //! Open a file
    /*!
    \param[in] fileName The name of the file including the path
  */
    void openFile(const QString& fileName);

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_verticalSlider_sliderMoved(int position);

    void on_pushButton_5_clicked();

private:
    Ui::MainWindow* ui;
};

#endif
