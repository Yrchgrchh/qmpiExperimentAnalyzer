#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QDebug>
#include <algorithm>

static const char* axisNames[] =  {"Local time",
                                   "Local counter",
                                   "Time spent",
                                   "Message size",
                                   "Frequency"};

/*
 *  Main window constructor
 *  Create window and menu bar
 * */
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), m_xAxis(LOCAL_COUNTER), m_yAxis(TIME_SPENT),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(tr("Experiment Analyzer"));

    memset(m_maxAxisValues, 0, NUM_RESULT_TYPES*sizeof(double));

    m_scrollArea.setParent(this);
    m_scrollArea.setWidgetResizable(true);
    m_scrollArea.setBackgroundRole(QPalette::Dark);
    m_scrollArea.setFrameShadow(QFrame::Plain);
    m_scrollArea.setFrameShape(QFrame::NoFrame);

    QWidget *window = new QWidget(this);
    window->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    window->setLayout(new QVBoxLayout(window));
    m_scrollArea.setWidget(window);

    m_scrollArea.setGeometry(0,30,size().width(),size().height());

    QMenu* menuFile = ui->menuBar->addMenu(tr("&Experiment"));
    QAction* runExpAction = new QAction(tr("&Run"),this);
    menuFile->addAction(runExpAction);
    connect(runExpAction, SIGNAL(triggered()), SLOT(openRunExp()));
    QAction* openFileAction = new QAction(tr("&Open result"),this);
    menuFile->addAction(openFileAction);
    connect(openFileAction, SIGNAL(triggered()), SLOT(openFile()));

    QMenu* menuTools = ui->menuBar->addMenu(tr("&Tools"));
    QAction* optionsAction = new QAction(tr("&Options"),this);
    menuTools->addAction(optionsAction);
    connect(optionsAction, SIGNAL(triggered()), SLOT(openOptions()));

    menuBar()->addMenu(tr("&Help"));
}

/*
 *  Main window desctucror
 * */
MainWindow::~MainWindow()
{
    for (int i=0; i<m_plots.size(); i++)
    {
        if (NULL != m_plots[i])
        {
            delete m_plots[i];
        }
    }

    delete ui;
}

/*
 *  Slot that opens dialog window for xml file selection
 *  and then initiate results processing and rendering
 * */
void MainWindow::openFile()
{
    QString fileName = QFileDialog::getOpenFileName( this, tr("Open File"), "", tr("Files (*.xml)") );

    if ( !fileName.isNull() )
    {
        if ( 0 == readInputFile(fileName))//QString("res.xml")) )
        {
            for (int i=0; i<m_plots.size(); i++)
            {
                m_plots[i]->setParent(m_scrollArea.widget());
                m_scrollArea.widget()->layout()->addWidget(m_plots[i]);
                //m_plots[i]->replot();
            }

            m_plotsToShow.resize( m_plots.size() );
            m_plotsToShow.fill(true);

            drawResults();
            resize(800,600);
        }
    }
}

/*
 *  Slot that opens options dialog window
 * */
void MainWindow::openOptions()
{
    if ( 0 < m_experimentResults.size() )
    {
        m_pOptionsDialog = new OptionsDialog(&m_plotsToShow, (int*)&m_xAxis, (int*)&m_yAxis);
        m_pOptionsDialog->exec();
        delete m_pOptionsDialog;

        for (int i=0; i < m_plots.size(); i++)
        {
            if ( true == m_plotsToShow[i] )
            {
                m_plots[i]->setParent(m_scrollArea.widget());
                m_scrollArea.widget()->layout()->addWidget(m_plots[i]);
                drawResults();
                m_plots[i]->replot();
            }
            else
            {
                m_plots[i]->setParent(0);
                m_scrollArea.widget()->layout()->removeWidget(m_plots[i]);
            }
        }
    }
}

/*
 *   Slot that opens run experiment dialog window
 * */
void MainWindow::openRunExp()
{
    m_pRunExpDialog = new RunExperimentDialog();
    m_pRunExpDialog->exec();
    delete m_pRunExpDialog;
}

/*
 *  Resize event, obviuosly
 * */
void MainWindow::resizeEvent(QResizeEvent* event)
{
    m_scrollArea.resize(event->size().width(),event->size().height() - 30);
    for (int i=0; i<m_plots.size(); i++)
    {
        m_plots[i]->setFixedHeight(size().height() - 50);
        m_plots[i]->setFixedWidth(size().width() - 35);
    }
}

/*
 * Propagate Esc action to all plots
 * */
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if ( event->key() == Qt::Key_Escape )
    {
        for ( int i=0; i<m_plots.size(); i++ )
            m_plots[i]->keyPressEvent(event);
    }
}

/*
 *  Input xml file parsing
 * */
int MainWindow::readInputFile(QString fileName)
{
    int result = 0;
    QFile file(fileName);
    QString elementText;
    experiment_map_t::Iterator rankIterator = m_experimentResults.begin();
    sender_map_t::Iterator toIterator;

    if ( !file.open(QFile::ReadOnly | QFile::Text) )
    {
        qDebug() << "Cannot open file";
        result = 1;
    }
    else
    {
        m_xmlReader.setDevice(&file);

        while ( !m_xmlReader.atEnd() )
        {
            m_xmlReader.readNext();
            if ( "rank" == m_xmlReader.name() )
            {
                elementText = m_xmlReader.readElementText();
                rankIterator = m_experimentResults.insert(elementText.toInt(), sender_map_t());
                m_plots.push_back(new MyCustomPlot());
            }
            else if ( "to" == m_xmlReader.name() )
            {
                elementText = m_xmlReader.readElementText();
                toIterator = rankIterator.value().insert(elementText.toInt(), results_map_t());
                toIterator.value().insert(LOCAL_COUNTER, QVector<double>());
                toIterator.value().insert(LOCAL_TIME,    QVector<double>());
                toIterator.value().insert(TIME_SPENT,    QVector<double>());
                toIterator.value().insert(MESSAGE_SIZE,  QVector<double>());
                toIterator.value().insert(FREQUENCY,     QVector<double>());

            }
            else if ( "time_send" == m_xmlReader.name() )
            {
                elementText = m_xmlReader.readElementText();
                (toIterator.value())[TIME_SPENT].push_back( elementText.toDouble() );

                if ( m_maxAxisValues[TIME_SPENT] < elementText.toDouble() )
                     m_maxAxisValues[TIME_SPENT] = elementText.toDouble();

            }
            else if ( "bytes" == m_xmlReader.name() )
            {
                elementText = m_xmlReader.readElementText();
                (toIterator.value())[MESSAGE_SIZE].push_back( elementText.toDouble() );

                if ( m_maxAxisValues[MESSAGE_SIZE] < elementText.toDouble() )
                     m_maxAxisValues[MESSAGE_SIZE] = elementText.toDouble();
            }
            else if ( "local_time" == m_xmlReader.name() )
            {
                elementText = m_xmlReader.readElementText();
                (toIterator.value())[LOCAL_TIME].push_back( elementText.toDouble() );

                if ( m_maxAxisValues[LOCAL_TIME] < elementText.toDouble() )
                     m_maxAxisValues[LOCAL_TIME] = elementText.toDouble();
            }
            else if ( "local_counter" == m_xmlReader.name() )
            {
                elementText = m_xmlReader.readElementText();
                (toIterator.value())[LOCAL_COUNTER].push_back( elementText.toDouble() );

                if ( m_maxAxisValues[LOCAL_COUNTER] < elementText.toDouble() )
                     m_maxAxisValues[LOCAL_COUNTER] = elementText.toDouble();
            }
            else if ( "frequency" == m_xmlReader.name() )
            {
                elementText = m_xmlReader.readElementText();
                (toIterator.value())[FREQUENCY].push_back( elementText.toDouble() );

                if ( m_maxAxisValues[FREQUENCY] < elementText.toDouble() )
                     m_maxAxisValues[FREQUENCY] = elementText.toDouble();
            }
            else {}
        }
    }

    return result;
}

/*
 *  Pass all results to QCustomPlot library and set up plots
 * */
void MainWindow::drawResults()
{
    //TODO: should be reinit when new file si opened
    static bool firstDraw = true;

    experiment_map_t::iterator rankIterator = m_experimentResults.begin();

    while ( rankIterator != m_experimentResults.end() )
    {
        sender_map_t::Iterator toIterator = rankIterator.value().begin();
        int counter = 0;
        MyCustomPlot* curPlot = m_plots[rankIterator.key()];

        if ( firstDraw )
        {
            curPlot->plotLayout()->insertRow(0);
            curPlot->plotLayout()->addElement(0, 0, new QCPPlotTitle(curPlot,
                                                                     QString("From %1 PC")
                                                                     .arg(rankIterator.key())));
        }

        curPlot->legend->setVisible(true);

        toIterator = rankIterator.value().begin();
        while ( toIterator  != rankIterator.value().end() )
        {
            if ( firstDraw )
            {
                curPlot->addGraph();
            }

            curPlot->graph(counter)->setName("to " + QString::number(counter) + " PC");
            curPlot->graph(counter)->setPen(QPen(Qt::GlobalColor(counter+7)));
            curPlot->graph(counter)->setData((toIterator.value())[m_xAxis],
                                             (toIterator.value())[m_yAxis]);
            curPlot->graph(counter)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 5));
            counter++;
            toIterator++;
        }

        // give the axes some labels:
        curPlot->xAxis->setLabel(axisNames[m_xAxis]);
        curPlot->yAxis->setLabel(axisNames[m_yAxis]);

        // set axes ranges, so we see all data:
        curPlot->xAxis->setRange(0, m_maxAxisValues[m_xAxis]);
        curPlot->yAxis->setRange(0, m_maxAxisValues[m_yAxis]);
        curPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

        rankIterator++;
    }
    firstDraw = false;
}
