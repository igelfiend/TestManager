#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QListWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QDebug>
#include <QStringList>
#include <QDir>
#include <QVariant>
#include <QBoxLayout>
#include <QLabel>
#include <QFile>

#include "manager.h"
#include "test_info.h"
#include "config.h"
#include "test.h"
#include "param.h"
#include "utils.h"
#include "group.h"
#include "paramlistitem.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
	manager = new Manager( this );

	info = new ConfigInfo( manager );

	group_coord = { 0, 0 };

	no_test_group = nullptr;
	no_param_group = nullptr;

	init();
}

MainWindow::~MainWindow()
{
	delete info;
	delete manager;
	delete ui;

	for( int i = 0; i < groups.count(); ++i )
	{
		delete groups.at( i );
	}

	if( no_test_group )
	{
		delete no_test_group;
	}

	if( no_param_group )
	{
		delete no_param_group;
	}

}

void MainWindow::init()
{
	QDir path("../../release/devices");
	QStringList devices = path.entryList(QDir::Dirs);

	for (int i = 2; i < devices.size(); ++i)
	{
		QListWidgetItem * item = new QListWidgetItem(devices[i]);
		item->setCheckState(Qt::Unchecked);
		ui->listWidgetDevices->addItem(item);
	}

	QListWidgetItem * item = new QListWidgetItem("periodic");
	item->setCheckState(Qt::Unchecked);
	ui->listWidgetModes->addItem(item);

	item = new QListWidgetItem("periodic_ru");
	item->setCheckState(Qt::Unchecked);
	ui->listWidgetModes->addItem(item);

	item = new QListWidgetItem("factory_cmt");
	item->setCheckState(Qt::Unchecked);
	ui->listWidgetModes->addItem(item);


	for( int i = 0; i < info->getTestsCount(); ++i )
	{
		ui->comboBoxTests->addItem( info->getTest( i )->getName() );
	}

	if( ui->comboBoxTests->count() > 0 )
	{
		ui->comboBoxTests->setCurrentIndex(0);

		ui->comboBoxParameters->clear();
		for( int i = 0; i < info->getTest( 0 )->getParamCount(); ++i )
		{
			ui->comboBoxParameters->addItem( info->getTest( 0 )->getParam( i )->getName() );
		}
		if( ui->comboBoxParameters->count() > 0 )
		{
			ui->comboBoxParameters->setCurrentIndex( 0 );
		}
	}
}

QListWidget *MainWindow::getListModes()
{
	return ui->listWidgetModes;
}

TestInfo *MainWindow::getCurrentTest() const
{
	QString name_test = ui->comboBoxTests->currentText();
	return manager->getConfigInfo()->getTest( name_test );
}

TestParam *MainWindow::getCurrentParam() const
{
	QString name_param = ui->comboBoxParameters->currentText();
	return getCurrentTest()->getParam( name_param );
}

void MainWindow::addGroup(Group *group)
{
	ui->gridLayout->addLayout( group->getContainer(), group_coord.x, group_coord.y, 1, 1 );

	if ( group_coord.y == 7 )
	{
		group_coord.y = 0;
		group_coord.x++;
	}
	else
	{
		group_coord.y++;
	}

	groups.append( group );
}

void MainWindow::clearGroups()
{
	for( int i = 0; i < groups.count(); ++i)
	{
		delete groups.at( i );
	}

	if( no_test_group )
	{
		delete no_test_group;
		no_test_group = nullptr;
	}

	if( no_param_group )
	{
		delete no_param_group;
		no_param_group = nullptr;
	}
	groups.clear();

	group_coord = { 0, 0 };
}

void MainWindow::loadGroups()
{
	QString test_name	= ui->comboBoxTests->currentText();
	QString param_name	= ui->comboBoxParameters->currentText();

	qDebug() << "Test:  " << test_name;
	qDebug() << "Param: " << param_name;

	QVector< Config* >	configs;

	int conf_count = manager->getConfigsCount();
	for( int i = 0; i < conf_count; ++i )
	{
		configs.append( manager->getConfig( i ) );
	}

	struct LocalGroup
	{
		QString title;
		QVector< Config* >	configs;
		QVector< Param* >	params;
	};

	QVector<  LocalGroup > groups;
	LocalGroup no_tests;
	LocalGroup no_params;

	no_tests.title	= "Configuration without test";
	no_params.title	= "Configuration without parameter";

	int param_indx	= 1;		// Temprary for naming  Complex units
	while( configs.count() > 0 )
	{
		LocalGroup group;

		Item * test	= configs.at( 0 )->getItem( test_name );
		if( !test )
		{
			no_tests.configs.append( configs.at( 0 ) );
			configs.removeAt( 0 );
			continue;
		}

		Param * param = test->getParam( param_name );
		if( !param )
		{
//			qDebug() << "No such param in test";
//			qDebug() << "Test has:";
//			test->ShowAllParams();
			no_params.configs.append(configs.at( 0 ));
			configs.removeAt( 0 );
			continue;
		}

		if( param->getType() != ParamType::Complex )
		{
			group.title = param->getData();
		}
		else
		{
			group.title = QString::number( param_indx++ );
		}

		group.params.append( param );
		for(int i = 1; i < configs.count(); ++i )
		{
			if( ( !configs.at( i )->getItem( test_name ) ) || ( !configs.at( i )->getItem( test_name )->getParam( param_name  ) ) )
			{
				continue;
			}

			if( param->compare( configs.at( i )->getItem( test_name )->getParam( param_name )))
			{
				group.params.append( configs.at( i )->getItem( test_name )->getParam( param_name ) );
				configs.removeAt( i-- );
			}
		}

		configs.removeAt( 0 );
		groups.append(group);
	}

	for( int i = 0; i < groups.count(); ++i )
	{
		Group	*group = new Group( groups.at( i ).title, groups.at( i ).params, this );
		addGroup( group );
	}

	if( no_tests.configs.count() > 0 )
	{
		no_test_group	= new Group( no_tests.title,	no_tests.configs,	this );
		ui->verticalLayoutNoData->addLayout( no_test_group->getContainer() );
	}
	if( no_params.configs.count() > 0 )
	{
		no_param_group	= new Group( no_params.title,	no_params.configs,	this );
		ui->verticalLayoutNoData->addLayout( no_param_group->getContainer() );
	}
}

QListWidget *MainWindow::getListDevices()
{
    return ui->listWidgetDevices;
}

void MainWindow::on_pushButton_3_clicked()
{
    QStringList devices;
    for (int i = 0; i < ui->listWidgetDevices->count(); ++i)
    {
        if (ui->listWidgetDevices->item(i)->checkState() == Qt::Checked)
        {
            devices.append(ui->listWidgetDevices->item(i)->text());
        }
    }

    QStringList modes;
    for (int i = 0; i < ui->listWidgetModes->count(); ++i)
    {
        if (ui->listWidgetModes->item(i)->checkState() == Qt::Checked)
        {
            modes.append(ui->listWidgetModes->item(i)->text());
        }
    }

	manager->Clear();
	clearGroups();
	if( ui->comboBoxTests->count() > 0 )
	{
		ui->comboBoxTests->setCurrentIndex( 0 );
	}

    manager->Load(devices, modes);
	emit(ui->comboBoxParameters->currentIndexChanged( ui->comboBoxParameters->currentIndex() ));

//	manager->ShowLoadedData();
}


void MainWindow::on_comboBoxTests_currentIndexChanged(int index)
{
	ui->comboBoxParameters->clear();
	clearGroups();

	for( int i = 0; i < info->getTest( index )->getParamCount(); ++i )
	{
		ui->comboBoxParameters->addItem( info->getTest( index )->getParam( i )->getName() );
	}

	if( ui->comboBoxParameters->count() > 0 )
	{
		ui->comboBoxParameters->setCurrentIndex(0);
	}
	else
	{
		ui->comboBoxParameters->setCurrentIndex( -1 );
	}
}

void MainWindow::on_comboBoxParameters_currentIndexChanged(int index)
{
	if(( index == -1 ) || ( !manager->isLoaded() ))
	{
		return;
	}

	if( index == ui->comboBoxParameters->count() - 1 )
	{
		ui->pushButtonNext->setEnabled( false );
	}
	else
	{
		ui->pushButtonNext->setEnabled( true );
	}
	clearGroups();
	loadGroups();
}

void MainWindow::on_pushButtonCheckAll_clicked()
{
	for( int i = 0; i < ui->listWidgetDevices->count(); ++i )
	{
		ui->listWidgetDevices->item( i )->setCheckState( Qt::Checked );
	}
}

void MainWindow::on_pushButtonUncheckAll_clicked()
{
	for( int i = 0; i < ui->listWidgetDevices->count(); ++i )
	{
		ui->listWidgetDevices->item( i )->setCheckState( Qt::Unchecked );
	}
}

void MainWindow::on_pushButtonCheckAvailable_clicked()
{
	QString path = QApplication::applicationDirPath() + "/" +  "ready_devices_list.cfg";
	QFile file( path );
	if( !file.open(QIODevice::ReadOnly |QIODevice::Text) )
	{
		qWarning()<< "Error loading ready devices file!";
		return;
	}

	while(!file.atEnd())
	{
	  QString str = file.readLine();
	  str.remove(str.length() - 1, 1);

	  for( int i = 0; i < ui->listWidgetDevices->count(); ++i )
	  {
		  if( ui->listWidgetDevices->item( i )->text() == str )
		  {
			  ui->listWidgetDevices->item( i )->setCheckState( Qt::Checked );
		  }
	  }
	}
}

void MainWindow::on_pushButtonSave_clicked()
{
//	QString str = "[[1, 2, 3, 4],[5, 6, 7, 8]]";
//	QString str2 = "[1, 2, 3, 4],[5, 6, 7, 8]";
//	QString str3 = "[1, 2, 3, 4, 5, 6, 7, 8]";
//	QString str4 = "1, 2, 3, 4, 5, 6, 7, 8";

//	qDebug() << "----STR1----";
//	QVector<QVector< QString >> result = Utils::parseString(str);
//	for( int i = 0; i < result.count(); ++i )
//	{
//		for( int j = 0; j < result.at( i ).count(); ++j )
//		{
//			qDebug() << result[ i ][ j ];
//		}
//		qDebug() << "---------------";
//	}
//	qDebug() << "--CONVERT BACK--";
//	qDebug() << Utils::saveToString(result, StringType::BracketsMatrix);
//	qDebug() << "---------------";
//	qDebug() << "----STR2------";
//	result = Utils::parseString(str2);
//	for( int i = 0; i < result.count(); ++i )
//	{
//		for( int j = 0; j < result.at( i ).count(); ++j )
//		{
//			qDebug() << result[ i ][ j ];
//		}
//		qDebug() << "-------------";
//	}
//	qDebug() << "--CONVERT BACK--";
//	qDebug() << Utils::saveToString(result, StringType::Matrix);
//	qDebug() << "-------------";
//	qDebug() << "----STR3----";
//	result = Utils::parseString(str3);
//	for( int i = 0; i < result.count(); ++i )
//	{
//		for( int j = 0; j < result.at( i ).count(); ++j )
//		{
//			qDebug() << result[ i ][ j ];
//		}
//		qDebug() << "-------------";
//	}
//	qDebug() << "--CONVERT BACK--";
//	qDebug() << Utils::saveToString(result, StringType::BracketsArray);
//	qDebug() << "-------------";
//	qDebug() << "----STR4----";
//	result = Utils::parseString(str4);
//	for( int i = 0; i < result.count(); ++i )
//	{
//		for( int j = 0; j < result.at( i ).count(); ++j )
//		{
//			qDebug() << result[ i ][ j ];
//		}
//		qDebug() << "-------------";
//	}
//	qDebug() << "--CONVERT BACK--";
//	qDebug() << Utils::saveToString(result, StringType::Array);
//	qDebug() << "-------------";
//	qDebug() << "-------------";


	manager->Save();
}

void MainWindow::createStatusBar()
{
	statusBar()->showMessage("Ready");

}

Manager *MainWindow::getManager() const
{
	return manager;
}

void MainWindow::on_pushButtonNext_clicked()
{
	if( ui->comboBoxParameters->currentIndex() < ui->comboBoxParameters->count()-1 )
	{
		ui->comboBoxParameters->setCurrentIndex( ui->comboBoxParameters->currentIndex() + 1 );
	}
}
