#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QVector>
#include <QListWidgetItem>


namespace Ui
{
    class MainWindow;
}

class QListWidget;
class QTableWidget;
class QVBoxLayout;
class Manager;
class ConfigInfo;
class TestInfo;
class Group;
class Comparator;

struct TestParam;
struct coord
{
	int x;
	int y;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

	void init();

	QListWidget	*getListDevices();
	QListWidget	*getListModes();

	TestInfo	*getCurrentTest()	const;
	TestParam	*getCurrentParam()	const;

	void	addGroup(Group *group);
	void	clearGroups();
	void	loadGroups();

	void	addCompareConfig();
	void	removeCompareConfig();


	Manager *getManager() const;
	QDockWidget	*getDockWidgetComparator() const;

private slots:
	void on_pushButton_3_clicked();
	void on_comboBoxParameters_currentIndexChanged(int index);
	void on_comboBoxTests_currentIndexChanged(int index);
	void on_pushButtonCheckAll_clicked();
	void on_pushButtonUncheckAll_clicked();
	void on_pushButtonCheckAvailable_clicked();
	void on_pushButtonSave_clicked();
	void on_pushButtonNext_clicked();
	void on_listWidgetDevices_itemChanged(QListWidgetItem *item);
	void on_pushButtonNextTest_clicked();

	void on_pushButtonCompare_clicked();

private:
	void createStatusBar();
	Ui::MainWindow	*ui;
	ConfigInfo	*info;
	Comparator	*comparator;
	Manager		*manager;
	coord	group_coord;
	QVector< Group* > groups;
	Group * no_test_group;
	Group * no_param_group;
};

#endif // MAINWINDOW_H
