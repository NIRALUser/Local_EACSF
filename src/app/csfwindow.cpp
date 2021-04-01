#include "csfwindow.h"

#include<stdlib.h>
#include <string>
#include <iostream>
#include <chrono>
#include <thread>


#include <QtGui>
#include <QFileDialog>
#include <QBoxLayout>
#include <QTextStream>

#ifndef Local_EACSF_TITLE
#define Local_EACSF_TITLE "Local_EACSF"
#endif

#ifndef Local_EACSF_CONTRIBUTORS
#define Local_EACSF_CONTRIBUTORS "Tahya Deddah, Martin Styner, Juan Prieto"
#endif

const QString CSFWindow::m_github_url = "https://github.com/tahya-deddah/Local_EACSF";

CSFWindow::CSFWindow(QWidget *m_parent)
    :QMainWindow(m_parent)
{
    
    this->setupUi(this);
    QJsonObject root_obj = readConfig(QString(":/config/default_config.json"));
    
    QJsonArray exe_array = root_obj["executables"].toArray();
    QMap<QString, QString> executables;

    foreach (const QJsonValue exe_val, exe_array)
    {
        QJsonObject exe_obj = exe_val.toObject();
        executables[exe_obj["name"].toString()] = exe_obj["path"].toString();
    }

    QBoxLayout* exe_layout = new QBoxLayout(QBoxLayout::LeftToRight, tab_executables);
    m_exeWidget = new ExtExecutablesWidget();

    if (!executables.keys().isEmpty())
    {
        m_exeWidget->buildInterface(executables);
        m_exeWidget->setExeDir(QDir::currentPath());
        exe_layout->addWidget(m_exeWidget,Qt::AlignCenter);
        connect(m_exeWidget, SIGNAL(newExePath(QString,QString)), this, SLOT(updateExecutables(QString,QString)));
    }


    slurm->setChecked(true);
    slurm->setChecked(false);
    smooth->setChecked(true);
    smooth->setChecked(false);      
   
    QJsonObject param_obj = root_obj["parameters"].toObject();
    ClosingRadius->setValue(param_obj["Closing_radius"].toInt());
    DilationRadius->setValue(param_obj["Dilation_radius"].toInt());
    IterationsNumber->setValue(param_obj["Iterations_number"].toInt());
    ImageDimension->setText(param_obj["Image_dimension"].toString());
    tab->removeTab(3); 

}

CSFWindow::~CSFWindow(){}


QJsonObject CSFWindow::readConfig(QString filename)
{
    QString config_qstr;
    QFile config_qfile;
    config_qfile.setFileName(filename);
    config_qfile.open(QIODevice::ReadOnly | QIODevice::Text);
    config_qstr = config_qfile.readAll();
    config_qfile.close();

    QJsonDocument config_doc = QJsonDocument::fromJson(config_qstr.toUtf8()); 
    QJsonObject root_obj = config_doc.object();
    return root_obj;
}

bool CSFWindow::writeConfig(QString filename)
{
    QFile saveFile(filename);
    if (!saveFile.open(QIODevice::WriteOnly)) {
        std::cout<<"Couldn't open save file."<<std::endl;
        return false;
    }

    QJsonDocument saveDoc(getConfig());
    saveFile.write(saveDoc.toJson());

    std::cout<<"Saved configuration : "<<filename.toStdString()<<endl;
    return true;
}


void CSFWindow::setConfig(QJsonObject root_obj)
{
    QJsonObject data_obj = root_obj["data"].toObject();

    lineEdit_T1img->setText(data_obj["T1"].toString()); 
    lineEdit_Segmentation->setText(data_obj["Tissu_Segmentation"].toString());
    lineEdit_CSF_Probability_Map->setText(data_obj["CSF_Probability_Map"].toString());
    lineEdit_LH_MID_Surface->setText(data_obj["LH_MID_surface"].toString());
    lineEdit_LH_GM_Surface->setText(data_obj["LH_GM_surface"].toString());
    lineEdit_RH_MID_Surface->setText(data_obj["RH_MID_surface"].toString());
    lineEdit_RH_GM_Surface->setText(data_obj["RH_GM_surface"].toString());
    lineEdit_Output_Directory->setText(data_obj["Output_Directory"].toString());

    QJsonObject param_obj = root_obj["parameters"].toObject();

    lineEdit_Core->setText(param_obj["Slurm_cores"].toString());
    lineEdit_Node->setText(param_obj["Slurm_nodes"].toString());
    lineEdit_Time->setText(param_obj["Slurm_time"].toString());
    lineEdit_Memory->setText(param_obj["Slurm_memory"].toString());

    smooth->setChecked(param_obj["Smooth"].toBool());
    NumberIter_lineEdit->setText(param_obj["Smoothing_numberIter"].toString());
    Bandwith_lineEdit->setText(param_obj["Smoothing_bandwith"].toString());

    clean->setChecked(param_obj["Clean"].toBool());

    ClosingRadius->setValue(param_obj["Closing_radius"].toInt());
    DilationRadius->setValue(param_obj["Dilation_radius"].toInt());
    IterationsNumber->setValue(param_obj["Iterations_number"].toInt());
    ImageDimension->setText(param_obj["Image_dimension"].toString());

    QJsonArray exe_array = root_obj["executables"].toArray();
    foreach (const QJsonValue exe_val, exe_array)
    {
        QJsonObject exe_obj = exe_val.toObject();
        cout<<exe_obj["path"].toString().toStdString()<<endl;

        this->updateExecutables(exe_obj["name"].toString(), exe_obj["path"].toString());
    }

}

QJsonObject CSFWindow::getConfig(){

    QJsonObject data_obj; 
    data_obj["T1"]=lineEdit_T1img->text();
    data_obj["Tissu_Segmentation"]=lineEdit_Segmentation->text();
    data_obj["CSF_Probability_Map"]=lineEdit_CSF_Probability_Map->text();
    data_obj["LH_MID_surface"]=lineEdit_LH_MID_Surface->text();
    data_obj["LH_GM_surface"]=lineEdit_LH_GM_Surface->text();
    data_obj["RH_MID_surface"]=lineEdit_RH_MID_Surface->text();
    data_obj["RH_GM_surface"]=lineEdit_RH_GM_Surface->text();
    data_obj["Output_Directory"]=lineEdit_Output_Directory->text();

    QJsonObject param_obj;
    if(slurm->isChecked())
    {
        param_obj["Slurm_nodes"] = lineEdit_Node->text();    
        param_obj["Slurm_cores"] = lineEdit_Core->text();
        param_obj["Slurm_time"] = lineEdit_Time->text();
        param_obj["Slurm_memory"] = lineEdit_Memory->text();
    }
    param_obj["Smooth"] = smooth->isChecked();
    if(smooth->isChecked())
    {
        param_obj["Smoothing_numberIter"] = NumberIter_lineEdit->text();    
        param_obj["Smoothing_bandwith"] = Bandwith_lineEdit->text();
    }
    param_obj["Clean"] = clean->isChecked();

    param_obj["Closing_radius"] = ClosingRadius->value();    
    param_obj["Dilation_radius"] = DilationRadius->value();
    param_obj["Iterations_number"] = IterationsNumber->value();
    param_obj["Image_dimension"] = ImageDimension->text();

    QJsonObject root_obj;
    root_obj["data"] = data_obj;
    root_obj["parameters"] = param_obj;
    root_obj["executables"] = m_exeWidget->GetExeArray();
    return root_obj;
}

QString CSFWindow::OpenFile(){
    QString filename = QFileDialog::getOpenFileName(
                this,
                "Open File",
                "C://",
                "All files (*.*);; NIfTI File (*.nii *.nii.gz);; NRRD File  (*.nrrd);; Json File (*.json)"
                );
    return filename;
}

QString CSFWindow::SaveFile()
  {
    QString filename = QFileDialog::getSaveFileName(
        this,
        "Save Document",
        QDir::currentPath(),
        "JSON file (*.json)") ;
    return filename;
  }

QString CSFWindow::OpenDir(){
    QString dirname = QFileDialog::getExistingDirectory(
                this,
                "Open Directory",
                "C://",
                QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
                );
    return dirname;
}

QString CSFWindow::checkStringValue(QJsonValue str_value)
{
            if(str_value.isUndefined()){
                return QString("");
            }else{
                return str_value.toString();
            }
}

void CSFWindow::infoMsgBox(QString message, QMessageBox::Icon type)
{
    QMessageBox mb;
    mb.setIcon(type);
    mb.setText(message);
    mb.setStandardButtons(QMessageBox::Ok);
    mb.exec();
}


//File menu

void CSFWindow::on_actionLoad_Config_File_triggered()
{

        QString filename= OpenFile();
        if (!filename.isEmpty())
        {
            setConfig(readConfig(filename));
        }
}

void CSFWindow::on_actionSave_Config_File_triggered()
{
        QString filename=SaveFile();
        if (!filename.isEmpty())
        {
            if (!filename.endsWith(QString(".json")))
            {
                filename+=QString(".json");
            }
            bool success=writeConfig(filename);
            if (success)
            {
                infoMsgBox(QString("Configuration saved with success : ")+filename,QMessageBox::Information);
            }
            else
            {
                infoMsgBox(QString("Couldn't save configuration file at this location. Try somewhere else."),QMessageBox::Warning);
            }
        }

}

//Help menu


void CSFWindow::on_actionAbout_triggered()
{
    QString messageBoxTitle = "About " + QString( Local_EACSF_TITLE );
           QString aboutFADTTS;
           aboutFADTTS = "<b>Contributors:</b> " + QString( Local_EACSF_CONTRIBUTORS ) + "<br>"
                  "<b>Github:</b> <a href=" + m_github_url + ">Click here</a><br>";
       QMessageBox::information( this, tr( qPrintable( messageBoxTitle ) ), tr( qPrintable( aboutFADTTS ) ), QMessageBox::Ok );
}

// Window menu

void CSFWindow::on_actionToggle_advanced_mode_toggled(bool toggled)
{
    if (toggled)
       {
           tab->insertTab(3,tab_parameters,QString("Parameters"));
       }
       else
       {
           tab->removeTab(3);
       }
}


// 1st Tab - Input


void CSFWindow::on_Data_Directory_clicked()
{
    QString path=OpenDir();
     if (!path.isEmpty())
     {
         lineEdit_Data_Directory->setText(path);
     }
}
void CSFWindow::on_Find_clicked()
{
    QString data_directory = lineEdit_Data_Directory->text();
           if(data_directory==QString(""))
           {
               QMessageBox::information(
                           this,
                           tr("EACSF"),
                           tr("Please choose a data directory")
                       );
           }
           else
           {
              
                QList<QStringList> data;
                QDirIterator it(data_directory, QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);        
                while(it.hasNext())
                {  
                    QString subdirectory = it.next();
                    QFileInfo file(subdirectory);
                    if (file.isDir()) 
                    { 
                        QDir oDir(file.absoluteFilePath()); 
                        QStringList line = {};
                        bool found = Find_Paths(oDir, lineEdit_T1_filter->text(), line); 
                        if (found)
                        {
                            Find_Paths(oDir, lineEdit_Seg_filter->text(), line);
                            Find_Paths(oDir, lineEdit_CSF_Prob_filter->text(), line);   
                            Find_Paths(oDir, lineEdit_LH_MID_filter->text(), line);
                            Find_Paths(oDir, lineEdit_LH_GM_filter->text(), line);
                            Find_Paths(oDir, lineEdit_RH_MID_filter->text(), line);
                            Find_Paths(oDir, lineEdit_RH_GM_filter->text(), line);
                            line.append(file.absoluteFilePath()); 
                            data.append(line);       
                        }
                                   
                    } 
                }
                model = new CsvTableModel(data,this);
                tableView->setModel(model);
                tableView->resizeColumnsToContents();
                tableView->resizeRowsToContents();
            }
}

bool CSFWindow::Find_Paths(QDir oDir, QString filter,  QStringList &vect)
{ 

    QStringList oDirList = oDir.entryList(QDir::Files);
    bool found = false;

    int count =0;;
    for (int i = 0; i < oDirList.size(); ++i)
    {
        QString filename = oDirList.at(i);
        QFileInfo( oDir, filename).absoluteFilePath();    
        QRegularExpression re(filter);
        QRegularExpressionMatch match = re.match(QFileInfo( oDir, filename).fileName());
        if (filter != QString("") && match.hasMatch())
        {        
            count = count + 1; 
            found = true;
            vect.append(QFileInfo( oDir, filename).absoluteFilePath());        
        }
    }
    return found; 
}


//2nd tab

void CSFWindow::on_T1_clicked()
{

    QString path=OpenFile();

    if (!path.isEmpty())
    {
        lineEdit_T1img->setText(path);
    }
}

void CSFWindow::on_Segmentation_clicked()
{

    QString path=OpenFile();
    if (!path.isEmpty())
    {
        lineEdit_Segmentation->setText(path);
    }
}

void CSFWindow::on_CSF_Probability_Map_clicked()
{
    QString path=OpenFile();
    if (!path.isEmpty())
    {
       lineEdit_CSF_Probability_Map->setText(path);
    }
}

void CSFWindow::on_LH_MID_Surface_clicked()
{
    QString path=OpenFile();
    if (!path.isEmpty())
    {
        lineEdit_LH_MID_Surface->setText(path);
    }

}

void CSFWindow::on_LH_GM_Surface_clicked()
{
    QString path=OpenFile();
    if (!path.isEmpty())
    {
        lineEdit_LH_GM_Surface->setText(path);
    }
}

void CSFWindow::on_RH_MID_Surface_clicked()
{

    QString path=OpenFile();
    if (!path.isEmpty())
    {
        lineEdit_RH_MID_Surface->setText(path);
    }

}

void CSFWindow::on_RH_GM_Surface_clicked()
{
    QString path=OpenFile();
    if (!path.isEmpty())
    {
       lineEdit_RH_GM_Surface->setText(path);
    }

}
void CSFWindow::on_output_directory_clicked()
{
   QString path=OpenDir();
    if (!path.isEmpty())
    {
        lineEdit_Output_Directory->setText(path);
    }
}


// 3rd Tab - Executables tab
void CSFWindow::updateExecutables(QString exeName, QString path)
{
   m_exeWidget->setExecutable(exeName, path);
}



// 4rd Tab -  Smoothing + Cleaning + Location 

void CSFWindow::on_local_clicked()
{
    if (local->isChecked()){slurm->setEnabled(false);}
    else{slurm->setEnabled(true);}
}

void CSFWindow::on_slurm_clicked()
{
    if (slurm->isChecked()){local->setEnabled(false);}
    else{local->setEnabled(true);}

}

void CSFWindow::on_slurm_stateChanged(int state)
{
    bool enab;
        if (state==Qt::Checked){enab=true;}
        else{enab=false;}

        slurm_parameters->setEnabled(enab);
        Core->setEnabled(enab);
        Memory->setEnabled(enab);
        Node->setEnabled(enab);
        Time->setEnabled(enab);
        lineEdit_Core->setEnabled(enab);
        lineEdit_Memory->setEnabled(enab);
        lineEdit_Node->setEnabled(enab);
        lineEdit_Time->setEnabled(enab);
}

void CSFWindow::on_smooth_stateChanged(int state)
{
    bool enab;
    if (state==Qt::Checked){enab=true;}
    else{enab=false;}
    smoothing_parameters_label->setEnabled(enab);
    Smoothing_NumberIter->setEnabled(enab);
    NumberIter_lineEdit->setEnabled(enab);
    Smoothing_Bandwith->setEnabled(enab);
    Bandwith_lineEdit->setEnabled(enab);

}


// 5th tab - Execution

void CSFWindow::prc_finished(QProcess *prc, QString outlog_filename, QString errlog_filename){

    QString exit_message;
    int exitCode =  prc->exitCode();
    QProcess::ExitStatus exitStatus = prc->exitStatus();
    if (local->isChecked())
    {
        exit_message = QString("Local_EACSF pipeline ") + ((exitStatus == QProcess::NormalExit) ? QString("exited with code ") + QString::number(exitCode) : QString("crashed"));
        if (exitCode==0)
        {
            exit_message="<font color=\"green\"><b>"+exit_message+"</b></font>";
            output->append(exit_message);
            cout<<exit_message.toStdString()<<endl;
            QFile file(outlog_filename);
            file.open(QIODevice::WriteOnly | QIODevice::Append);
            QTextStream out(&file); out <<exit_message;
            file.close();
        }
        else
        {
            exit_message="<font color=\"red\"><b>"+exit_message+"</b></font>";
            error->append(exit_message);
            cout<<exit_message.toStdString()<<endl;
            QFile file(errlog_filename);
            file.open(QIODevice::WriteOnly | QIODevice::Append);
            QTextStream out(&file); out <<exit_message;
            file.close();
        }  
    }
}

void CSFWindow::CleanFile(QString filename)
{ 
    QFile file(filename);
    file.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text);
    file.close();
}

void CSFWindow::disp_output(QProcess *prc, QString outlog_filename)
{   
    QString output_log(prc->readAllStandardOutput()); 
    QFile file(outlog_filename);

    if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
    QTextStream out(&file); out <<output_log;
    output->append(output_log);
    file.close();}  
}

void CSFWindow::disp_err(QProcess *prc, QString errlog_filename)
{ 
    QString error_log(prc->readAllStandardError());
    QFile file(errlog_filename);

    if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
    QTextStream out(&file); out <<error_log;
    error->append(error_log);
    file.close();}  
}


void CSFWindow::on_Execute_clicked()
{  
    run_Local_EACSF();   
}

void CSFWindow::run_Local_EACSF()
{
    QProcess *prc; 
    prc = new QProcess;

    QJsonObject root_obj = getConfig();

    CSFScripts csfscripts;
    csfscripts.setConfig(root_obj);
    csfscripts.run_EACSF();

    QJsonObject data_obj = root_obj["data"].toObject();
    QJsonObject param_obj = root_obj["parameters"].toObject();
    QString output_dir = data_obj["Output_Directory"].toString();
    QString scripts_dir = QDir::cleanPath(output_dir + QString("/PythonScripts"));
    QString outlog_filename = QDir::cleanPath(output_dir + QString("/output_log.txt"));
    QString errlog_filename = QDir::cleanPath(output_dir + QString("/errors_log.txt"));


    if (local->isChecked())
    {      
        CleanFile(outlog_filename);
        CleanFile(errlog_filename);

        QString main_script = QDir::cleanPath(scripts_dir + QString("/main_script.py"));
        QStringList params = QStringList() << main_script;

        connect(prc, &QProcess::readyReadStandardOutput, [prc, outlog_filename, this](){
           disp_output(prc, outlog_filename);
        });
        connect(prc, &QProcess::readyReadStandardError, [prc, errlog_filename, this](){
           disp_err(prc, errlog_filename);
        });
        connect(prc, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), [prc, outlog_filename, errlog_filename, this ](){
            prc_finished(prc, outlog_filename, errlog_filename);  
        });
        prc->setWorkingDirectory(output_dir); 
        QMap<QString, QString> executables = m_exeWidget->GetExeMap();
        prc->start(executables[QString("python3")], params);  
        prc->waitForFinished(-1); 
       

    }
    
    if (slurm->isChecked())
    {
        QString slurm_script = QDir::cleanPath(scripts_dir + QString("/slurm-job"));
        QString time = QString("--time=") + param_obj["Slurm_time"].toString();
        QString memory = QString("--mem=") + param_obj["Slurm_memory"].toString();
        QString core = QString("--ntasks=") + param_obj["Slurm_cores"].toString();
        QString node = QString("--nodes=") + param_obj["Slurm_nodes"].toString();
        QString output_file = QString("--output=") + outlog_filename ;
        QString error_file = QString("--error=") + errlog_filename ;

        QStringList params = QStringList() << time << memory << core << node << output_file << error_file << slurm_script;
        prc->setWorkingDirectory(output_dir);
        connect(prc, &QProcess::readyReadStandardOutput, [prc, outlog_filename, this](){
            QString output_log(prc->readAllStandardOutput()); 
            output->append(output_log);
        });
        connect(prc, &QProcess::readyReadStandardError, [prc, errlog_filename, this](){
            QString error_log(prc->readAllStandardError());
            error->append(error_log);
        });
        prc->start(QString("sbatch"), params);
      
        /*QMessageBox::information(
            this,
            tr("Local EACSF"),
            tr("Is running.")
        );*/
    }  

}

//6th Visualization

void CSFWindow::on_output_path_clicked()
{
    QString path=OpenDir();

       if (!path.isEmpty())
       {
           lineEdit_output_path->setText(path);
       }
}

void CSFWindow::on_shapepopoulationviewer_clicked()
{
    if (shapepopoulationviewer->isChecked()){itksnap->setEnabled(false);}
        else {itksnap->setEnabled(true);}
}

void CSFWindow::on_itksnap_clicked()
{
    if (itksnap->isChecked()){shapepopoulationviewer->setEnabled(false);}
        else {shapepopoulationviewer->setEnabled(true);}
}

void CSFWindow::on_visualize_clicked()
{
    QProcess *visualization;
    visualization = new QProcess;
    QString OutputDirectory =lineEdit_output_path->text();
    QString LH_Directory = QDir::cleanPath(OutputDirectory + QString("/LH_Directory"));
    QString RH_Directory = QDir::cleanPath(OutputDirectory + QString("/RH_Directory"));


    if (itksnap->isChecked())
    {
        QString LH_visitation_map = LH_Directory + QString("/LH__Visitation.nrrd");
        QString RH_visitation_map = RH_Directory + QString("/RH__Visitation.nrrd");
        QStringList arguments = QStringList()<< QString("-g") << LH_visitation_map << QString("-s") << RH_visitation_map;
        visualization->setWorkingDirectory(OutputDirectory);
        visualization->start(QString("itksnap"),arguments);
        QMessageBox::information(
            this,
            tr("Visualization"),
            tr("Is running.")
        );
    }
    if (shapepopoulationviewer->isChecked())
    {
        QString LH_CSFDensity = LH_Directory + QString("/LH_CSF_Density.vtk");
        QString RH_CSFDensity  = RH_Directory + QString("/RH_CSF_Density.vtk");
        QStringList arguments = QStringList() << QString("-v") << LH_CSFDensity  << QString("-v")<< RH_CSFDensity ;
        visualization->setWorkingDirectory(OutputDirectory);
        visualization->start(QString("ShapePopulationViewer"),arguments);
        QMessageBox::information(
            this,
            tr("Visualization"),
            tr("Is running.")
        );
    }

}



void CSFWindow::on_Remove_clicked()
{
    QModelIndexList indexes = tableView->selectionModel()->selectedRows();

    while (!indexes.isEmpty()) {
        model->removeRows(indexes.last().row(), 1);
        indexes.removeLast();
    }
}

void CSFWindow::on_Run_Batch_Process_clicked()
{
       QList<QStringList> data = model->getdata();
       for (int j=0;j<model->rowCount();j++)
        {
            lineEdit_T1img->setText(data[j].at(0));
            lineEdit_Segmentation->setText(data[j].at(1));
            lineEdit_CSF_Probability_Map->setText(data[j].at(2));
            lineEdit_LH_MID_Surface->setText(data[j].at(3)); 
            lineEdit_LH_GM_Surface->setText(data[j].at(4));
            lineEdit_RH_MID_Surface->setText(data[j].at(5));
            lineEdit_RH_GM_Surface->setText(data[j].at(6));
            lineEdit_Output_Directory->setText(data[j].at(7));
            //run_Local_EACSF();

            QProcess *prc; 
            prc = new QProcess;

            QJsonObject root_obj = getConfig();

            CSFScripts csfscripts;
            csfscripts.setConfig(root_obj);
            csfscripts.run_EACSF();
            QJsonObject data_obj = root_obj["data"].toObject();
            QJsonObject param_obj = root_obj["parameters"].toObject();
            QString output_dir = data_obj["Output_Directory"].toString();
            QString scripts_dir = QDir::cleanPath(output_dir + QString("/PythonScripts"));
            QString outlog_filename = QDir::cleanPath(output_dir + QString("/output_log.txt"));
            QString errlog_filename = QDir::cleanPath(output_dir + QString("/errors_log.txt"));

            CleanFile(outlog_filename);
            CleanFile(errlog_filename);

            QString main_script = QDir::cleanPath(scripts_dir + QString("/main_script.py"));
            QStringList params = QStringList() << main_script;




        }



}
