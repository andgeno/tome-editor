#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>

#include "../Features/Search/Model/searchresultlist.h"
#include "../Features/Tasks/Model/messagelist.h"

class ComponentsWindow;
class FieldDefinitionsWindow;
class FieldValueWindow;
class AboutWindow;
class NewProjectWindow;
class RecordWindow;
class CustomTypesWindow;
class DuplicateRecordWindow;

namespace Ui {
    class MainWindow;
}

namespace Tome
{
    class Controller;
    class ErrorListDockWidget;
    class Project;
    class RecordFieldsTableWidget;
    class RecordTreeWidget;
    class RecordTreeWidgetItem;
    class SearchResultsDockWidget;
}

class MainWindow : public QMainWindow
{
        Q_OBJECT

    public:
        explicit MainWindow(Tome::Controller* controller, QWidget *parent = 0);
        ~MainWindow();

    private slots:
        void on_actionExit_triggered();

        void on_actionField_Definions_triggered();
        void on_actionManage_Components_triggered();
        void on_actionManage_Custom_Types_triggered();

        void on_actionNew_Project_triggered();
        void on_actionOpen_Project_triggered();
        void on_actionSave_Project_triggered();

        void on_actionNew_Record_triggered();
        void on_actionEdit_Record_triggered();
        void on_actionDuplicate_Record_triggered();
        void on_actionRevert_Record_triggered();
        void on_actionRemove_Record_triggered();

        void on_actionFind_Usages_triggered();

        void on_actionRun_Integrity_Checks_triggered();

        void on_actionAbout_triggered();
        void on_actionManual_triggered();
        void on_actionReport_a_Bug_triggered();
        void on_actionReleases_triggered();

        void on_actionError_List_triggered();

        void exportRecords(QAction* exportAction);
        void onFieldChanged();
        void openRecentProject(QAction* recentProjectAction);
        void revertFieldValue();
        void searchResultChanged(const QString& title, const Tome::SearchResultList results);
        void tableWidgetDoubleClicked(const QModelIndex &index);
        void treeWidgetDoubleClicked(const QModelIndex &index);
        void treeWidgetRecordReparented(const QString& recordId, const QString& newParentId);
        void treeWidgetSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);


    private:
        Ui::MainWindow *ui;

        Tome::RecordTreeWidget* recordTreeWidget;
        Tome::RecordFieldsTableWidget* recordFieldTableWidget;
        Tome::ErrorListDockWidget* errorListDockWidget;
        Tome::SearchResultsDockWidget* searchResultsDockWidget;

        Tome::Controller* controller;

        AboutWindow *aboutWindow;
        ComponentsWindow* componentsWindow;
        CustomTypesWindow* customTypesWindow;
        FieldDefinitionsWindow* fieldDefinitionsWindow;
        FieldValueWindow* fieldValueWindow;
        NewProjectWindow *newProjectWindow;
        RecordWindow* recordWindow;
        DuplicateRecordWindow* duplicateRecordWindow;

        Tome::MessageList messages;

        void addRecordField(const QString& fieldId);
        void openProject(QString path);
        void removeRecordField(const QString& fieldId);
        void onProjectChanged();
        void refreshErrorList();
        void refreshRecordTree();
        void refreshRecordTable();
        void showWindow(QWidget* widget);
        void updateMenus();
        void updateRecentProjects();
        void updateRecord(const QString& id, const QString& displayName);
        void updateRecordRow(const int i);
        void updateWindowTitle();
};

#endif // MAINWINDOW_H
