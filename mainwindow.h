#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "headers/Graph.h"
#include "headers/GraphGraphicsScene.h"
#include "headers/GraphMatrixTable.h"
#include "headers/GraphGraphicsView.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow() override;

private slots:
    void onGraphChanged();

    void on_openGraphButton_clicked();
    void on_createGraphButton_clicked();
    void on_actionSave_triggered();
    void on_actionSave_As_triggered();
    void on_actionNew_Graph_triggered();
    void on_actionOpen_Graph_triggered();
    void on_actionCredits_triggered();
    static void on_actionExit_triggered();
    void on_actionAddNode_triggered();
    void on_actionAddArc_triggered();
    void on_actionEditArc_triggered();
    void on_actionDelNode_triggered();
    void on_actionDelArc_triggered();
    void on_articulationNodeBtn_clicked();
    void on_bridgesBtn_clicked();
    void on_coloringBtn_clicked();
    void on_connectedComponentsBtn_clicked();
    void on_shortestPathBtn_clicked();
    void on_topoSortBtn_clicked();
    void on_BFSbtn_clicked();
    void on_DFSbtn_clicked();
    void on_EulerBtn_clicked();
    void on_HamiltonBtn_clicked();
    void on_actionBFS_triggered();
    void on_actionDFS_triggered();
    void on_actionColoring_triggered();
    void on_actionTopo_Sorting_triggered();
    void on_actionEuler_Cycle_triggered();
    void on_actionHamiltonian_Cycle_triggered();
    void on_actionFind_shortest_path_triggered();
    void on_actionFind_all_bridges_triggered();
    void on_actionFind_all_Articulation_nodes_triggered();
    void on_actionFind_connected_components_triggered();
    void on_actionFInd_minimum_spanning_tree_triggered();
    void on_spanningTreeBtn_clicked();
    void on_actionFind_weakly_connected_components_triggered();
    void on_weaklyConnectedBtn_clicked();

signals:
    void graphChanged();
    void startDemoAlgorithm(std::list<std::pair<int, int>> listOfPair, GraphDemoFlag flag);
    void startDemoAlgorithm(std::list<int> listOfNum, GraphDemoFlag flag);
    void startDemoAlgorithm(std::list<std::list<int>> listOfList, GraphDemoFlag flag);

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    QString workingFileName;
    Ui::MainWindow *ui;
    GraphGraphicsScene *scene;
    GraphGraphicsView *view;
    GraphMatrixTable *matrix;
    Graph *graph;
    bool dataNeedSaving;

    QString showOpenFileDialog();
    QString showSaveFileDialog();
    void showNewNodeDialog(QPointF pos = QPointF(0, 0));
    void setWorkspaceEnabled(bool ready);
    void initWorkspace(const QString& filename, bool newFile = false);
};

#endif // MAINWINDOW_H
