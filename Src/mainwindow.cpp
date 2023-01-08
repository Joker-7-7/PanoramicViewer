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

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::showAboutDialog()
{

}

void MainWindow::showOpenFileDialog()
{
    QStringList items;
    items << tr("Single DICOM file") << tr("Multi-file directory");

    bool ok;
    QString item = QInputDialog::getItem(this, tr("Opening mode"),
                                            tr("Select mode:"), items, 0, false, &ok);
    if (ok && !item.isEmpty())
        openFile(item);
}

void MainWindow::openFile(const QString& item)
{
    vtkSmartPointer<vtkImageReader2> dataSet = nullptr;

        if(item ==  "Multi-file directory")
        {
            QString directoryName = QFileDialog::getExistingDirectory(this, tr("Open folder"), "");
            // Open directory
            QFile file(directoryName);
            file.open(QIODevice::ReadOnly);

            // Return on Cancel
            if (!file.exists())
                return;

            vtkSmartPointer<vtkGlobFileNames> globFileNames =
                vtkSmartPointer<vtkGlobFileNames>::New();
            globFileNames->SetDirectory(directoryName.toStdString().c_str());
            globFileNames->AddFileNames("*.dcm");

            vtkSmartPointer<vtkDICOMFileSorter> filesSorter =
                vtkSmartPointer<vtkDICOMFileSorter>::New();
            filesSorter->SetInputFileNames(globFileNames->GetFileNames());
            filesSorter->Update();

            if (filesSorter->GetNumberOfSeries() != 1)
            {
                ShowErrorMassage();
                return;
            }

            vtkNew<vtkDICOMReader> reader;
            reader->SetFileNames(filesSorter->GetFileNamesForSeries(0));
            reader->SetMemoryRowOrderToFileNative();
            reader->Update();
            if (reader->GetErrorCode() == 0)
                dataSet = reader;
            else
                ShowErrorMassage();
        }
        else if(item ==  "Single DICOM file" )
        {
             QString fileName = QFileDialog::getOpenFileName(this, tr("Open file.."), "C://","*.dcm");
             // Open file
             QFile file(fileName);
             file.open(QIODevice::ReadOnly);

             // Return on Cancel
             if (!file.exists())
                 return;

             vtkNew<vtkDICOMReader> fileReader;
             fileReader->SetFileName(fileName.toStdString().c_str());
             fileReader->SetMemoryRowOrderToFileNative();
             fileReader->Update();
             if (fileReader->GetErrorCode() == 0)
                 dataSet = fileReader;
             else
                 ShowErrorMassage();
        }

        // Add data set to 3D view
        if (dataSet != nullptr)
        {
            addDataSet(dataSet);
        }
}


void MainWindow::addDataSet(vtkSmartPointer<vtkImageReader2> dataSet)
{
    ui->openGLSceneWidget->addDataSet(dataSet);
    ui->openGLSliceXYWidget->addDataSet(dataSet, ui->openGLSceneWidget);

   ui->verticalSlider->setMaximum(ui->openGLSliceXYWidget->_reslicer->GetSliceMax());
   ui->verticalSlider->setValue(ui->openGLSliceXYWidget->_reslicer->GetSliceMax() / 2);
}

void MainWindow::on_pushButton_clicked()
{
    ui->openGLSceneWidget->setRentgenEffects();
    ui->openGLSceneWidget->renderWindow()->Render();
}


void MainWindow::on_pushButton_2_clicked()
{
    ui->openGLSceneWidget->setSolidEffects();
    ui->openGLSceneWidget->renderWindow()->Render();
}


void MainWindow::on_pushButton_3_clicked()
{
    ui->openGLSceneWidget->setMaxIntensity();
    ui->openGLSceneWidget->renderWindow()->Render();
}


void MainWindow::on_pushButton_4_clicked()
{
    ui->openGLSceneWidget->setMinIntensity();
    ui->openGLSceneWidget->renderWindow()->Render();
}


void MainWindow::on_verticalSlider_sliderMoved(int position)
{
    if(ui->openGLSliceXYWidget->_reslicer != nullptr)
    {
        ui->openGLSliceXYWidget->_reslicer->SetSlice(position);
        ui->openGLSliceXYWidget->renderWindow()->Render();
    }
}


void MainWindow::on_pushButton_5_clicked()
{
     ui->openGLSliceXYWidget->setSplineWidget();
    
}

