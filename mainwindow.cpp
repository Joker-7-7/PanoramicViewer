#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>

#include <vtkDataSetReader.h>
#include <vtkGlobFileNames.h>
#include <vtkDICOMFileSorter.h>
#include <vtkImageReader2.h>
#include <vtkDICOMReader.h>

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
            ui->openGLWidgetScene->addDataSet(dataSet);
        }
}

void MainWindow::on_pushButton_clicked()
{
    ui->openGLWidgetScene->setRentgenEffects();
    ui->openGLWidgetScene->renderWindow()->Render();
}


void MainWindow::on_pushButton_2_clicked()
{
    ui->openGLWidgetScene->setSolidEffects();
    ui->openGLWidgetScene->renderWindow()->Render();
}


void MainWindow::on_pushButton_3_clicked()
{
    ui->openGLWidgetScene->setMaxIntensity();
    ui->openGLWidgetScene->renderWindow()->Render();
}


void MainWindow::on_pushButton_4_clicked()
{
    ui->openGLWidgetScene->setMinIntensity();
    ui->openGLWidgetScene->renderWindow()->Render();
}

