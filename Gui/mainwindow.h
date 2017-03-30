#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QVector>


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


	Manager *getManager() const;

private slots:
	void on_pushButton_3_clicked();

	void on_comboBoxParameters_currentIndexChanged(int index);

	void on_comboBoxTests_currentIndexChanged(int index);

	void on_pushButtonCheckAll_clicked();

	void on_pushButtonUncheckAll_clicked();

	void on_pushButtonCheckAvailable_clicked();

	void on_pushButtonSave_clicked();

	void on_pushButtonNext_clicked();

private:
	void createStatusBar();
	Ui::MainWindow	*ui;
	ConfigInfo	*info;
	Manager		*manager;
	coord	group_coord;
	QVector< Group* > groups;
	Group * no_test_group;
	Group * no_param_group;
};

#endif // MAINWINDOW_H
