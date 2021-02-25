#include "csfscripts.h"


CSFScripts::CSFScripts(){
    m_PythonScripts = QString("/PythonScripts");
}

CSFScripts::~CSFScripts(){}


void CSFScripts::run_EACSF()
{
   
    QJsonObject data_obj = m_Root_obj["data"].toObject();

    QDir out_dir = QDir();
    out_dir.mkdir(QDir::cleanPath(checkStringValue(data_obj["Output_Directory"].toString())));
    out_dir.mkdir(QDir::cleanPath(checkStringValue(data_obj["Output_Directory"]) + m_PythonScripts));

    
    QFile saveFile(QDir::cleanPath(checkStringValue(data_obj["Output_Directory"]) + "/Local_EACSF_config.json"));
    saveFile.open(QIODevice::WriteOnly);
    QJsonDocument saveDoc(m_Root_obj);
    saveFile.write(saveDoc.toJson());
    saveFile.close();


    //0. WRITE MAIN_SCRIPT
    write_main_script();

    //1. WRITE PROCESS_LEFT_HEMISPHERE
    write_process_left_hemisphere();

    //2. WRITE PROCESS_RIGHT_HEMISPHERE
     write_process_right_hemisphere();
    
}

void CSFScripts::setConfig(QJsonObject config){
    m_Root_obj = config;
}

void CSFScripts::write_main_script()
{

    QJsonObject data_obj = m_Root_obj["data"].toObject();
    QJsonObject param_obj = m_Root_obj["parameters"].toObject();

    QFile file(QString(":/PythonScripts/main_script.py"));
    file.open(QIODevice::ReadOnly);
    QString script = file.readAll();
    file.close();

    script.replace("@Output_Directory@", checkStringValue(data_obj["Output_Directory"]));

    QJsonArray exe_array = m_Root_obj["executables"].toArray();
    foreach (const QJsonValue exe_val, exe_array)
    {
        QJsonObject exe_obj = exe_val.toObject();
        m_Executables[exe_obj["name"].toString()] = exe_obj["path"].toString();
    }

    script.replace("@python3_PATH@", checkStringValue(m_Executables["python3"]));

    QString scripts_dir = QDir::cleanPath(checkStringValue(data_obj["Output_Directory"]) + m_PythonScripts);
    
    QString main_script = QDir::cleanPath(scripts_dir + QString("/main_script.py"));
    QFile outfile(main_script);
    outfile.open(QIODevice::WriteOnly);
    QTextStream outstream(&outfile);
    outstream << script;
    outfile.close();
}



void CSFScripts::write_process_left_hemisphere()
{

    QJsonObject data_obj = m_Root_obj["data"].toObject();
    QJsonObject param_obj = m_Root_obj["parameters"].toObject();

    QFile file_left_hemisphere(QString(":/PythonScripts/process_left_hemisphere.py"));
    file_left_hemisphere.open(QIODevice::ReadOnly);
    QString script_left_hemisphere = file_left_hemisphere.readAll();
    file_left_hemisphere.close();

    script_left_hemisphere.replace("@T1_IMAGE@", checkStringValue(data_obj["T1"]));
    script_left_hemisphere.replace("@Tissu_Segmentation@", checkStringValue(data_obj["Tissu_Segmentation"]));
    script_left_hemisphere.replace("@CSF_Probability_Map@", checkStringValue(data_obj["CSF_Probability_Map"]));
    script_left_hemisphere.replace("@LH_MID_surface@", checkStringValue(data_obj["LH_MID_surface"]));
    script_left_hemisphere.replace("@LH_GM_surface@", checkStringValue(data_obj["LH_GM_surface"]));
    script_left_hemisphere.replace("@Output_Directory@", checkStringValue(data_obj["Output_Directory"]));



    QJsonArray exe_array = m_Root_obj["executables"].toArray();
    foreach (const QJsonValue exe_val, exe_array)
    {
        QJsonObject exe_obj = exe_val.toObject();
        m_Executables[exe_obj["name"].toString()] = exe_obj["path"].toString();
    }

    script_left_hemisphere.replace("@CreateOuterImage_PATH@", checkStringValue(m_Executables["CreateOuterImage"]));
    script_left_hemisphere.replace("@CreateOuterSurface_PATH@", checkStringValue(m_Executables["CreateOuterSurface"]));
    script_left_hemisphere.replace("@EditPolyData_PATH@", checkStringValue(m_Executables["EditPolyData"]));
    script_left_hemisphere.replace("@klaplace_PATH@", checkStringValue(m_Executables["klaplace"]));
    script_left_hemisphere.replace("@EstimateCortexStreamlinesDensity_PATH@", checkStringValue(m_Executables["EstimateCortexStreamlinesDensity"]));
    script_left_hemisphere.replace("@AddScalarstoPolyData_PATH@", checkStringValue(m_Executables["AddScalarstoPolyData"]));

    
    QString scripts_dir = QDir::cleanPath(checkStringValue(data_obj["Output_Directory"]) + m_PythonScripts);
    
    QString left_hemisphere_script = QDir::cleanPath(scripts_dir + QString("/process_left_hemisphere.py"));
    QFile outfile_left_hemisphere(left_hemisphere_script);
    outfile_left_hemisphere.open(QIODevice::WriteOnly);
    QTextStream  outstream_left_hemisphere(&outfile_left_hemisphere);
    outstream_left_hemisphere << script_left_hemisphere;
    outfile_left_hemisphere.close();
}


void CSFScripts::write_process_right_hemisphere()
{

    QJsonObject data_obj = m_Root_obj["data"].toObject();
    QJsonObject param_obj = m_Root_obj["parameters"].toObject();

    QFile file_right_hemisphere(QString(":/PythonScripts/process_right_hemisphere.py"));
    file_right_hemisphere.open(QIODevice::ReadOnly);
    QString script_right_hemisphere = file_right_hemisphere.readAll();
    file_right_hemisphere.close();

    script_right_hemisphere.replace("@T1_IMAGE@", checkStringValue(data_obj["T1"]));
    script_right_hemisphere.replace("@Tissu_Segmentation@", checkStringValue(data_obj["Tissu_Segmentation"]));
    script_right_hemisphere.replace("@CSF_Probability_Map@", checkStringValue(data_obj["CSF_Probability_Map"]));
    script_right_hemisphere.replace("@RH_MID_surface@", checkStringValue(data_obj["RH_MID_surface"]));
    script_right_hemisphere.replace("@RH_GM_surface@", checkStringValue(data_obj["RH_GM_surface"]));
    script_right_hemisphere.replace("@Output_Directory@", checkStringValue(data_obj["Output_Directory"]));


     QJsonArray exe_array = m_Root_obj["executables"].toArray();
    foreach (const QJsonValue exe_val, exe_array)
    {
        QJsonObject exe_obj = exe_val.toObject();
        m_Executables[exe_obj["name"].toString()] = exe_obj["path"].toString();
    }

    script_right_hemisphere.replace("@CreateOuterImage_PATH@", checkStringValue(m_Executables["CreateOuterImage"]));
    script_right_hemisphere.replace("@CreateOuterSurface_PATH@", checkStringValue(m_Executables["CreateOuterSurface"]));
    script_right_hemisphere.replace("@EditPolyData_PATH@", checkStringValue(m_Executables["EditPolyData"]));
    script_right_hemisphere.replace("@klaplace_PATH@", checkStringValue(m_Executables["klaplace"]));
    script_right_hemisphere.replace("@EstimateCortexStreamlinesDensity_PATH@", checkStringValue(m_Executables["EstimateCortexStreamlinesDensity"]));
    script_right_hemisphere.replace("@AddScalarstoPolyData_PATH@", checkStringValue(m_Executables["AddScalarstoPolyData"]));

    
    QString scripts_dir = QDir::cleanPath(checkStringValue(data_obj["Output_Directory"]) + m_PythonScripts);
    
    QString right_hemisphere_script = QDir::cleanPath(scripts_dir + QString("/process_right_hemisphere.py"));
    QFile outfile_right_hemisphere(right_hemisphere_script);
    outfile_right_hemisphere.open(QIODevice::WriteOnly);
    QTextStream outstream_right_hemisphere(&outfile_right_hemisphere);
    outstream_right_hemisphere << script_right_hemisphere;
    outfile_right_hemisphere.close();
}






