#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QXmlStreamReader>
#include <QMap>
#include <QScrollArea>
#include "mycustomplot.h"
#include "dialogs/optionsdialog.h"
#include "dialogs/runexperimentdialog.h"

namespace Ui {
class MainWindow;
}

#define NUM_RESULT_TYPES 5

enum RESULT_KEYS{
    LOCAL_TIME = 0,
    LOCAL_COUNTER,
    TIME_SPENT,
    MESSAGE_SIZE,
    FREQUENCY
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

    typedef QMap<RESULT_KEYS, QVector<double> > results_map_t;
    typedef QMap<int, results_map_t >           sender_map_t;
    typedef QMap<int, sender_map_t >            experiment_map_t;

    QScrollArea            m_scrollArea;

    QXmlStreamReader       m_xmlReader;
    experiment_map_t       m_experimentResults;
    QVector<MyCustomPlot*> m_plots;
    OptionsDialog         *m_pOptionsDialog;
    RunExperimentDialog   *m_pRunExpDialog;

    /*to khow which plots should be shown*/
    QVector<bool>          m_plotsToShow;

    /*type of axises that can be changed by user*/
    RESULT_KEYS m_xAxis;
    RESULT_KEYS m_yAxis;

    /*to adapt axis ranges to parsed values*/
    double m_maxAxisValues[NUM_RESULT_TYPES];

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void resizeEvent(QResizeEvent* event);
    void keyPressEvent(QKeyEvent* event);
    int  readInputFile(QString fileName);

    void drawResults();
private:
    Ui::MainWindow *ui;

private slots:
    void openFile();
    void openOptions();
    void openRunExp();
};



#endif // MAINWINDOW_H
