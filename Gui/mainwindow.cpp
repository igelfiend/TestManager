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
#include <QMessageBox>
#include <QCloseEvent>
#include <QSettings>
#include <QTextCodec>

#include "manager.h"
#include "test_info.h"
#include "config.h"
#include "test.h"
#include "param.h"
#include "utils.h"
#include "group.h"
#include "paramlistitem.h"
#include "comparator.h"
#include "AddNewTestDialog.h"

#include "colorreplacerform.h"
#define COL_COUNT 5

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
	manager = new Manager( this );

	info = new ConfigInfo( manager );

	group_coord = { 0, 0 };

	no_test_group	= nullptr;
	no_param_group	= nullptr;

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

    delete no_test_group;

    delete no_param_group;
}

void MainWindow::init()
{
    QSettings settings( "settings.ini", QSettings::IniFormat );
    settings.setIniCodec( QTextCodec::codecForName("Windows-1251") );

    QStringList config_devs;
    QMap< QString, bool > config_devs_flagged;
    settings.beginGroup( "DEVICES" );
    {
        config_devs = settings.allKeys();
        foreach( QString dev, config_devs )
        {
            config_devs_flagged[ dev ] = settings.value( dev, false ).toBool();
        }
    }
    settings.endGroup();

    QDir path("../../release/devices");
    QStringList devices = path.entryList(QDir::Dirs);

	for (int i = 2; i < devices.size(); ++i)
	{
        QListWidgetItem * item = new QListWidgetItem( devices[ i ] );

        if( config_devs.contains( devices[ i ], Qt::CaseInsensitive ) )
        {
            item->setCheckState( ( config_devs_flagged[ devices[ i ] ] )
                                    ? Qt::Checked
                                    : Qt::Unchecked );
        }
        else
        {
            item->setCheckState(Qt::Unchecked);
        }
		ui->listWidgetDevices->addItem(item);
	}


    QStringList verifications;
    settings.beginGroup( "MODES" );
    {
        verifications = settings.allKeys();
        foreach( QString verif, verifications )
        {
            QListWidgetItem *item = new QListWidgetItem( verif );
            bool checked = settings.value( verif, false ).toBool();
            item->setCheckState( ( checked ) ? Qt::Checked : Qt::Unchecked );
            ui->listWidgetModes->addItem( item );
        }
    }
    settings.endGroup();

	combobox_tests.clear();
	for( int i = 0; i < info->getTestsCount(); ++i )
	{
		ui->comboBoxTests->addItem( info->getTest( i )->getVersionedName() );
		combobox_tests << QPair< QString, QString >( info->getTest( i )->getName(), info->getTest( i )->getVersion() );
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

	comparator = new Comparator( manager, ui->LayoutCompareConfig );

	connect( ui->toolButtonPlus,	SIGNAL(clicked(bool)),
			 comparator,			SLOT(addBlock(bool)));
	connect( ui->toolButtonMinus,	SIGNAL(clicked(bool)),
			 comparator,			SLOT(removeBlock(bool)));
}

QListWidget *MainWindow::getListModes()
{
    return ui->listWidgetModes;
}

QStringList MainWindow::getCheckedDevicesList()
{
    QStringList result;
    QListWidget *lw = ui->listWidgetDevices;
    for( int i = 0; i < lw->count(); ++i )
    {
        QListWidgetItem *listRow = lw->item( i );
        if( listRow->checkState() == Qt::Checked )
        {
            result.append( lw->item( i )->text() );
        }
    }

    return result;
}

TestInfo *MainWindow::getCurrentTest() const
{
    int curr_index = ui->comboBoxTests->currentIndex();
	return manager->getConfigInfo()->getTest( combobox_tests.at( curr_index ).first, combobox_tests.at( curr_index ).second );
}

TestParam *MainWindow::getCurrentParam() const
{
	QString name_param = ui->comboBoxParameters->currentText();
	return getCurrentTest()->getParam( name_param );
}

void MainWindow::addGroup(BaseGroup *group)
{
	ui->gridLayout->addLayout( group->getContainer(), group_coord.x, group_coord.y, 1, 1 );

	if ( group_coord.y == COL_COUNT )
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

void MainWindow::addCompareGroups(QVector<CompareGroup *> groups)
{
	group_coord.x++;
	group_coord.y = 0;

	for( int i = 0; i < groups.count(); ++i )
	{
		addGroup( groups.at( i ) );
	}

//	if( group_coord.y == COL_COUNT )
//	{
//		group_coord.x++;
//		group_coord.y = 0;
//	}

//	QFrame *line;
//	line = new QFrame(this);
//	line->setFrameShape(QFrame::HLine);
//	line->setFrameShadow(QFrame::Sunken);

//	group_coord.x++;
//	group_coord.y = 0;
//	ui->gridLayout->addWidget( line, group_coord.x, group_coord.y, 1, (COL_COUNT+1) );
	//	group_coord.x++;
}

void MainWindow::addHeader(const QString &text, int level )
{
	QLabel *label_test	= new QLabel( text );

	QFont f1( "Arial", 12, QFont::Bold);
	QFont f2( "Arial", 12, QFont::Normal );
	QFont f3( "Times New Roman", 8, QFont::Normal );

	switch( level )
	{
	case 1:
		label_test->setFont( f1 );
		label_test->setStyleSheet( "QLabel { color : red; }" );
		break;
	case 2:
		label_test->setFont( f2 );
		label_test->setStyleSheet( "QLabel { color : green; }" );
		break;
	default:
		label_test->setFont( f3 );
		label_test->setStyleSheet( "QLabel { color : black; }" );
	}

	group_coord.x++;
	group_coord.y = 0;

	ui->gridLayout->addWidget(label_test,	group_coord.x,	group_coord.y,	1, 1);

	group_coord.x++;
	group_coord.y = 0;
}

void MainWindow::addHLine()
{
	QFrame *line;
	line = new QFrame(this);
	line->setFrameShape(QFrame::HLine);
	line->setFrameShadow(QFrame::Sunken);

	group_coord.x++;
	group_coord.y = 0;

	ui->gridLayout->addWidget( line, group_coord.x,	group_coord.y,	1, (COL_COUNT+1) );

	group_coord.x++;
	group_coord.y = 0;
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

	Utils::clearLayout(ui->gridLayout);
}

void MainWindow::loadGroups()
{
    int current_index	=  ui->comboBoxTests->currentIndex();
	QString test_name	= combobox_tests.at( current_index ).first;
	QString version		= combobox_tests.at( current_index ).second;
	QString param_name	= ui->comboBoxParameters->currentText();

	qDebug() << "Test:    " << test_name;
	qDebug() << "Version: " << version;
	qDebug() << "Param:   " << param_name;

	QVector< Config* >	configs;

	int conf_count = manager->getConfigsCount();
	for( int i = 0; i < conf_count; ++i )
	{
		configs.append( manager->getConfig( i ) );
	}

	QVector< LocalGroup >	groups;
	QVector< Item * >		all_tests;
	LocalGroup no_tests;
	LocalGroup no_params;

	no_tests.title	= "Configuration without test";
	no_params.title	= "Configuration without parameter";

	int param_indx	= 1;		// Temprary for naming  Complex units
	while( configs.count() > 0 )
	{

		QVector<Item *> tests = configs.at( 0 )->getItems( test_name, version );

		if( tests.count() == 0 )
		{
			no_tests.configs.append( configs.at( 0 ) );
			configs.removeAt( 0 );
			continue;
		}

		all_tests << tests;
		configs.removeAt( 0 );
	}

	while( all_tests.count() > 0 )
	{
		LocalGroup group;

		Param *param = all_tests.at( 0 )->getParam( param_name );
		if( !param )
		{
			no_params.items.append( all_tests.at( 0 ) );
			all_tests.removeAt( 0 );
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
		for(int i = 1; i < all_tests.count(); ++i )
		{
			Item *checked_item = all_tests.at( i );
			if( ( !checked_item ) || ( !checked_item->getParam( param_name  ) ) )
			{
				continue;
			}

			if( param->compare( checked_item->getParam( param_name ) ) )
			{
				group.params.append( checked_item->getParam( param_name ) );
				all_tests.removeAt( i-- );
			}
		}

		all_tests.removeAt( 0 );
		groups.append(group);
	}

	/*
	while( configs.count() > 0 )
	{
//		Item * test	= configs.at( 0 )->getItem( test_name, version );
//		if( !test )
//		{
//			no_tests.configs.append( configs.at( 0 ) );
//			configs.removeAt( 0 );
//			continue;
//		}

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
			if( ( !configs.at( i )->getItem( test_name, version ) ) ||
				( !configs.at( i )->getItem( test_name, version )->getParam( param_name  ) ) )
			{
				continue;
			}

			if( param->compare( configs.at( i )->getItem( test_name, version )->getParam( param_name )))
			{
				group.params.append( configs.at( i )->getItem( test_name, version )->getParam( param_name ) );
				configs.removeAt( i-- );
			}
		}

		configs.removeAt( 0 );
		groups.append(group);
	}
	*/

	for( int i = 0; i < groups.count(); ++i )
	{
		Group	*group = new Group( groups.at( i ).title, groups.at( i ).params, this );
        group->setGroupType( Group::Normal );
		addGroup( group );
	}

	if( no_tests.configs.count() > 0 )
	{
		no_test_group	= new Group( no_tests.title,	no_tests.configs,	this );
        no_test_group->setGroupType( Group::NoTest );
		ui->verticalLayoutNoData->addLayout( no_test_group->getContainer() );
	}
	if( no_params.items.count() > 0 )
	{
		no_param_group	= new Group( no_params.title,	no_params.items,	this );
        no_param_group->setGroupType( Group::NoParameter );
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
	comparator->update();
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
		qWarning()<< "Error loading ready devices file! Searching at: " << (QApplication::applicationDirPath() + "/" +  "ready_devices_list.cfg");
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

QDockWidget *MainWindow::getDockWidgetComparator() const
{
	return ui->dockWidgetComparator;
}

void MainWindow::on_pushButtonNext_clicked()
{
	if( ui->comboBoxParameters->currentIndex() < ui->comboBoxParameters->count()-1 )
	{
		ui->comboBoxParameters->setCurrentIndex( ui->comboBoxParameters->currentIndex() + 1 );
	}
}

void MainWindow::on_listWidgetDevices_itemChanged(QListWidgetItem *item)
{
	QListWidget *parent = item->listWidget();
	QList<QListWidgetItem *> selection = parent->selectedItems();
	if( selection.indexOf( item ) == -1 )
	{
		return;
	}

	for( int i = 0; i < selection.count(); ++i)
	{
		selection.at( i )->setCheckState( item->checkState() );
	}
}

void MainWindow::on_pushButtonNextTest_clicked()
{
	if( ui->comboBoxTests->currentIndex() < ui->comboBoxTests->count()-1 )
	{
		ui->comboBoxTests->setCurrentIndex( ui->comboBoxTests->currentIndex() + 1 );
	}
}

void MainWindow::on_pushButtonCompare_clicked()
{
//	QRect r = ui->dockWidgetComparator->geometry();
//	r.setHeight( 70 );
//	ui->dockWidgetComparator->setGeometry( r );
//	this->resize(this->width(), this->minimumHeight());

//	QPushButton	*button = new QPushButton("Testing Tests");
//	ui->LayoutScrollArea->addWidget(button);

//	QVector<Param *> conf;
//	conf.append( manager->getConfig( 0 )->getItem( 0 )->getParam( 0 ) );
//	Group *gr = new Group("Test", conf, this);
//	addGroup(gr);

	clearGroups();
	comparator->execute();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings( "settings.ini", QSettings::IniFormat );
    settings.setIniCodec( QTextCodec::codecForName("Windows-1251") );

    settings.beginGroup( "DEVICES" );
    {
        int dev_count = ui->listWidgetDevices->count();
        for( int i = 0; i < dev_count; ++i )
        {
            bool result     = ( ui->listWidgetDevices->item( i )->checkState() == Qt::Checked )
                                ? true
                                : false;
            QString devname = ui->listWidgetDevices->item( i )->text();

            settings.setValue( devname, result );
        }
    }
    settings.endGroup();

    settings.beginGroup( "MODES" );
    {
        int mode_count = ui->listWidgetModes->count();
        for( int i = 0; i < mode_count; ++i )
        {
            bool result      = ( ui->listWidgetModes->item( i )->checkState() == Qt::Checked )
                                ? true
                                : false;
            QString modename = ui->listWidgetModes->item( i )->text();

            settings.setValue( modename, result );
        }
    }
    settings.endGroup();

	if( !manager->isChanged() )
	{
		return;
	}

	event->ignore();
	int result = QMessageBox::question(this, "Close Confirmation", "Save changes?", QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
	switch( result )
	{
	case QMessageBox::Yes:
		manager->Save();
		event->accept();
		break;
	case QMessageBox::No:
		event->accept();
		break;
	case QMessageBox::Cancel:
		return;
	}
}

void MainWindow::on_actionFix_range_in_attenuator_triggered()
{
	QStringList tests  = QStringList() << "AccuracyTest"
									   << "AccuracyTestUncorrectedAndStabilities"
									   << "HighLowReflection";

	QStringList versions = QStringList() << "1.0"
										 << "1.0"
										 << "1.0";

	for( int i = 0; i < tests.count(); ++i )
	{
		Utils::fixAccuracyTestRange( tests[ i ], versions[ i ], manager );
	}
}

void MainWindow::on_actionAdd_equip_to_periodic_triggered()
{
    Utils::addEquipToPerformance( manager );
}

void MainWindow::on_actionFix_equip_order_in_periodic_triggered()
{
    Utils::fixEquipOrderInPeriodic( manager );
}

void MainWindow::on_actionFix_PowerAccuracy_naming_triggered()
{
    Utils::fixPowerAccuracyNaming( manager )   ;
}

void MainWindow::on_actionAdd_test_triggered()
{
    AddNewTestDialog add_test;
    if(add_test.exec() == QDialog::Accepted)
    {
        qDebug() << "Text: " << add_test.getTemplateCode();
        Utils::addTestToConfigs( add_test.getTemplateCode(),
                                 add_test.isInsertAtFirst(),
                                 add_test.getPrevTest(), manager );
    }
    else
    {
        qDebug() << "Dialog rejected";
    };
}

void MainWindow::on_actionAdd_Select_Method_to_Configs_triggered()
{
    Utils::addTagToMain( manager, "select_method", "read_mode" );
}

void MainWindow::on_actionAdd_Select_Print_to_Configs_triggered()
{
    Utils::addTagToMain( manager, "select_print", "read_mode" );
}

void MainWindow::on_actionAdd_Print_File_to_Configs_triggered()
{
    Utils::addTagToMain( manager, "print_file", "read_mode" );
}

void MainWindow::on_actionReplace_color_in_Svg_triggered()
{
    QMap< QString, QString > replace_map;
    replace_map[ "#0059FF" ] = "#56698F";
    replace_map[ "#6969AD" ] = "#7684A6";
    replace_map[ "#00A8FF" ] = "#7791BE";
    replace_map[ "#33B9FF" ] = "#5D7295";
    replace_map[ "#7BB0E0" ] = "#97A9CD";
    replace_map[ "#252527" ] = "none";
    replace_map[ "#3D3C41" ] = "#FFFFFF";
    replace_map[ "#727271" ] = "#8C8C8C";
    replace_map[ "#FEFEFE" ] = "#000000";

    bool fReplaceOrigin = false;
//    Utils::ReplaceColorInSvgs( QString( "../../release/devices" ), replace_map, fReplaceOrigin, this );
}

void MainWindow::on_actionReplace_color_on_origin_Svg_triggered()
{
    QMap< QString, QString > replace_map;
    replace_map[ "#252527" ] = "none";

    bool fReplaceOrigin = true;
//    Utils::ReplaceColorInSvgs( QString( "../../release/devices" ), replace_map, fReplaceOrigin, this );
}

void MainWindow::on_actionFix_Freq_Acc_Svg_triggered()
{
    QPoint pic_size = QPoint( 1152, 500 );
    QString stylename = "dev_style";
    QString css_style = ".dev_style {fill:#252527}";
    QString pics_foulder = "images";

    Utils::FixSvgStyle( pics_foulder, pic_size, stylename, css_style, this );
}

void MainWindow::on_actionReplace_colors_triggered()
{
    ColorReplacerForm replaceForm;
    QStringList checkedDevicesList = getCheckedDevicesList();
    replaceForm.setProcessingDevices( checkedDevicesList );
    if( replaceForm.exec() == QDialog::Accepted )
    {
        ColorReplacementTable table = replaceForm.getDataFromTable();

        if( table.isEmpty() )
        {
            QMessageBox mbox;
            mbox.setText( "Replacing data is empty" );
            mbox.exec();
            return;
        }

        QMap<QString, QString> replaceMap = table.toMap();
        bool fReplaceOrigin = replaceForm.isSelfReplaced();

        Utils::ReplaceColorInSvgs( QString( "../../release/devices" ), checkedDevicesList, replaceMap, fReplaceOrigin, this );
    }
}
