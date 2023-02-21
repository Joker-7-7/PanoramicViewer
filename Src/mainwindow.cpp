#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>

namespace
{
    void ShowErrorMassage()
    {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle("Error!");
        msgBox.setText("There are no suitable files in the directory or the files are corrupted.");
        msgBox.exec();
    }
}

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showAboutDialog() {

}

void MainWindow::showOpenFileDialog() {
    QStringList items;
    items << tr("Single DICOM file") << tr("Multi-file directory");

    bool ok;
    QString item = QInputDialog::getItem(this, tr("Opening mode"),
                                            tr("Select mode:"), items, 0, false, &ok);
    if (ok && !item.isEmpty()) {
        OpenData(item);
    }
}

void MainWindow::OpenData(const QString& fileName) {
    if(fileName == "Multi-file directory") {
        if(OpenDirectory() == OpeningStatus::Error) {
            ShowErrorMassage();
        }
    }
    else if(fileName == "Single DICOM file" ) {
        if(OpenSingleFile() == OpeningStatus::Error) {
            ShowErrorMassage();
        }
    }
}

MainWindow::OpeningStatus MainWindow::OpenSingleFile() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open file.."), "C://","*.dcm");
    // Open file
    QFile file(fileName);
    file.open(QIODevice::ReadOnly);

    // Return on Cancel
    if (!file.exists()) {
        return OpeningStatus::Cancel;
    }

    vtkSmartPointer<vtkImageReader2> dataSet = nullptr;

    vtkNew<vtkDICOMReader> fileReader;
    fileReader->SetFileName(fileName.toStdString().c_str());
    fileReader->SetMemoryRowOrderToFileNative();
    fileReader->Update();

    return CheckReader(std::move(fileReader), dataSet);
}

MainWindow::OpeningStatus MainWindow::OpenDirectory() {
    QString directoryName = QFileDialog::getExistingDirectory(this, tr("Open folder"), "");
    // Open directory
    QFile file(directoryName);
    file.open(QIODevice::ReadOnly);

    // Return on Cancel
    if (!file.exists()) {
        return OpeningStatus::Cancel;
    }

    vtkSmartPointer<vtkImageReader2> dataSet = nullptr;
    // Open directory
    auto globFileNames = vtkSmartPointer<vtkGlobFileNames>::New();
    globFileNames->SetDirectory(directoryName.toStdString().c_str());
    globFileNames->AddFileNames("*.dcm");

    auto directoryReader = vtkSmartPointer<vtkDICOMDirectory>::New();
    directoryReader->RequirePixelDataOn();
    directoryReader->SetInputFileNames(globFileNames->GetFileNames());
    directoryReader->Update();

    if (directoryReader->GetNumberOfSeries() == 0) {
        return OpeningStatus::Error;;
    }

    vtkNew<vtkDICOMReader> reader;
    reader->SetFileNames(directoryReader->GetFileNamesForSeries(0));
    reader->SetMemoryRowOrderToFileNative();
    reader->Update();

    return CheckReader(std::move(reader), dataSet);
}

MainWindow::OpeningStatus MainWindow::CheckReader(vtkSmartPointer<vtkDICOMReader> reader, vtkSmartPointer<vtkImageReader2> dataSet) {
    if (reader->GetErrorCode() == 0) {
        dataSet = reader;
    }

    // Add data set to 3D view
    if (dataSet) {
        AddDataSet(dataSet);
        return OpeningStatus::Success;
    }

    return OpeningStatus::Error;
}

void MainWindow::AddDataSet(vtkSmartPointer<vtkImageReader2> dataSet) {
    ui->openGLPanoramicWidget->AddDataSet(dataSet);
    ui->openGLSliceXYWidget->AddDataSet(dataSet, ui->openGLPanoramicWidget);

   ui->verticalSlider->setMaximum(ui->openGLSliceXYWidget->reslicer->GetSliceMax());
   ui->verticalSlider->setValue(ui->openGLSliceXYWidget->reslicer->GetSliceMax() / 2);
}

void MainWindow::on_pushButton_clicked() {
    ui->openGLPanoramicWidget->SetRentgenEffects();
    ui->openGLPanoramicWidget->renderWindow()->Render();
}

void MainWindow::on_pushButton_2_clicked() {
    ui->openGLPanoramicWidget->SetSolidEffects();
    ui->openGLPanoramicWidget->renderWindow()->Render();
}

void MainWindow::on_pushButton_3_clicked() {
    ui->openGLPanoramicWidget->SetMaxIntensity();
    ui->openGLPanoramicWidget->renderWindow()->Render();
}

void MainWindow::on_pushButton_4_clicked() {
    ui->openGLPanoramicWidget->SetMinIntensity();
    ui->openGLPanoramicWidget->renderWindow()->Render();
}

void MainWindow::on_verticalSlider_sliderMoved(int position) {
    if(ui->openGLSliceXYWidget->reslicer != nullptr)
    {
        ui->openGLSliceXYWidget->reslicer->SetSlice(position);
        ui->openGLSliceXYWidget->renderWindow()->Render();
    }
}

void MainWindow::on_pushButton_5_clicked() {
    ui->openGLSliceXYWidget->SetSplineWidget();
}
