#include "runexperimentdialog.h"
#include <QGridLayout>
#include <QFileDialog>
#include <QMessageBox>

RunExperimentDialog::RunExperimentDialog()
    :QDialog()
{
    this->setWindowTitle("Run experiment");
    resize(450,200);

    //just to allocate
    m_runArguments << "-n" << " " << " ";

    m_mpichAppLine  = new QLineEdit(this);
    m_mpichAppLine->setPlaceholderText("Select mpich.exe path");
    m_expAppLine    = new QLineEdit(this);
    m_expAppLine->setPlaceholderText("Select path of experiment to run");

    //freq and length should be hardcoded into experiment exe
    //m_freqTxtLine   = new QLineEdit(this);
    //m_freqTxtLine->setPlaceholderText("Select frequency settings text document");
    //m_lengthTxtLine = new QLineEdit(this);
    //m_lengthTxtLine->setPlaceholderText("Select message length settings text document");

    m_mpichAppButton  = new QPushButton("Open file", this);
    m_expAppButton    = new QPushButton("Open file", this);
    //m_freqTxtButton   = new QPushButton("Open file", this);
    //m_lengthTxtButton = new QPushButton("Open file", this);
    m_runButton       = new QPushButton("Run", this);
    m_clearButton     = new QPushButton("Clear", this);

    m_numOfProcLine = new QLabel("Enter the number of processes", this);
    m_numOfProcSpin = new QSpinBox(this);
    m_numOfProcSpin->setValue(0);

    m_fullRunLine = new QLineEdit(this);

    QGridLayout* layout = new QGridLayout();

    layout->addWidget(m_mpichAppLine,   0,0);
    layout->addWidget(m_mpichAppButton, 0,1);
    layout->addWidget(m_expAppLine,     1,0);
    layout->addWidget(m_expAppButton,   1,1);

    //layout->addWidget(m_freqTxtLine,    2,0);
    //layout->addWidget(m_freqTxtButton,  2,1);
    //layout->addWidget(m_lengthTxtLine,  3,0);
    //layout->addWidget(m_lengthTxtButton,3,1);

    layout->addWidget(m_numOfProcLine,  2,0);
    layout->addWidget(m_numOfProcSpin,  2,1);
    layout->addWidget(m_fullRunLine,    3,0);
    layout->addWidget(m_runButton,      4,0);
    layout->addWidget(m_clearButton,    4,1);

    setLayout(layout);

    m_runButton->setFocus();

    connect(m_mpichAppButton,  SIGNAL(clicked()), SLOT(openMpich()));
    connect(m_expAppButton,    SIGNAL(clicked()), SLOT(openExperiment()));
    //connect(m_freqTxtButton,   SIGNAL(clicked()), SLOT(openFrequency()));
    //connect(m_lengthTxtButton, SIGNAL(clicked()), SLOT(openLength()));
    connect(m_fullRunLine,     SIGNAL(textChanged(QString)), SLOT(fullRunLineChanged(QString)));
    connect(m_runButton,       SIGNAL(clicked()), SLOT(run()));
    connect(m_clearButton,     SIGNAL(clicked()), SLOT(clear()));
    connect(m_numOfProcSpin,   SIGNAL(valueChanged(int)), SLOT(numProcChanged(int)));
}

void RunExperimentDialog::openMpich()
{
    QString fileName = QFileDialog::getOpenFileName( this, tr("Open File"), "", tr("Files (*.exe)") );

    if ( !fileName.isNull() )
    {
        m_mpichAppLine->setText(fileName);
        m_mpichAppPath = fileName;
        m_fullRunLine->setText("ku"); //just to emit the signal
    }
}

void RunExperimentDialog::openExperiment()
{
    QString fileName = QFileDialog::getOpenFileName( this, tr("Open File"), "", tr("Files (*.exe)") );

    if ( !fileName.isNull() )
    {
        m_expAppLine->setText(fileName);
        m_runArguments.replace(2, fileName);
        m_fullRunLine->setText("ku"); //just to emit the signal
    }
}

void RunExperimentDialog::openFrequency()
{
    QString fileName = QFileDialog::getOpenFileName( this, tr("Open File"), "", tr("Files (*.txt)") );

    if ( !fileName.isNull() )
    {
        m_freqTxtLine->setText(fileName);
    }
}

void RunExperimentDialog::openLength()
{
    QString fileName = QFileDialog::getOpenFileName( this, tr("Open File"), "", tr("Files (*.exe)") );

    if ( !fileName.isNull() )
    {
        m_lengthTxtLine->setText(fileName);
    }
}

void RunExperimentDialog::numProcChanged(int num)
{
    m_runArguments.replace(1, QString("%1").arg(num));

    if ( 0 != num)
        m_fullRunLine->setText("ku"); //just to emit the signal
}

void RunExperimentDialog::fullRunLineChanged(QString s)
{
    (void)s;
    QString str(m_mpichAppPath);
    str.append(" ");
    str.append(m_runArguments.join(" "));
    m_fullRunLine->setText(str);
}

void RunExperimentDialog::run()
{
    m_experimentProc = new QProcess(this);
    connect(m_experimentProc,  SIGNAL(finished(int)), SLOT(experimentFinished(int)));

    m_experimentProc->start(m_mpichAppPath, m_runArguments);
}

void RunExperimentDialog::clear()
{
    m_expAppLine->clear();
    m_mpichAppLine->clear();
    //m_runArguments.clear();
    m_mpichAppPath.clear();
    m_numOfProcSpin->clear();
    m_numOfProcSpin->setValue(0);
    m_fullRunLine->clear();
}

void RunExperimentDialog::experimentFinished(int exitCode)
{
    if ( 0 == exitCode )
    {
        QMessageBox::information(this, tr("Experiment result"),
                                 tr("Experiment has finished succesfully"));
    }
    else
    {
        QMessageBox::critical(this, tr("Experiment result"),
                              tr("Experiment has failed"));
    }
}
