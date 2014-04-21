#ifndef RUNEXPERIMENTDIALOG_H
#define RUNEXPERIMENTDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QLabel>
#include <QProcess>

class RunExperimentDialog : public QDialog
{
    Q_OBJECT

    QLineEdit* m_mpichAppLine;
    QLineEdit* m_expAppLine;
    QLineEdit* m_freqTxtLine;
    QLineEdit* m_lengthTxtLine;

    QPushButton* m_mpichAppButton;
    QPushButton* m_expAppButton;
    QPushButton* m_freqTxtButton;
    QPushButton* m_lengthTxtButton;

    QLabel*    m_numOfProcLine;
    QSpinBox* m_numOfProcSpin;

    QLineEdit* m_fullRunLine;

    QPushButton* m_runButton;
    QPushButton* m_clearButton;
    QPushButton* m_readTempResButton;

    QStringList m_runArguments;
    QString     m_mpichAppPath;

    QProcess* m_experimentProc;
public:
    RunExperimentDialog();

private:
    void fullRunLineUpdate();
    bool runLineWasOverrided() const;

private slots:
    void openMpich();
    void openExperiment();
    void openFrequency();
    void openLength();
    void numProcChanged(int);
    void readTempRes();

    void run();
    void clear();
    void experimentFinished(int);
};

#endif // RUNEXPERIMENTDIALOG_H
