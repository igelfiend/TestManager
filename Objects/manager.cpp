#include "manager.h"

#include <QStringList>
#include <QString>
#include <QFile>
#include <QDir>
#include <QDomElement>
#include <QDebug>
#include <QMainWindow>
#include <QStatusBar>
#include <QTime>
#include <QTextCodec>

#include "config.h"
#include "test_info.h"
#include "test.h"
#include "param.h"
#include "group.h"
#include "mainwindow.h"
#include "editparamform.h"
#include "utils.h"

Manager::Manager(MainWindow *parent)
{
	this->window = parent;
	root	= "../../release/devices";
	loaded	= false;
	changed	= false;
	edit_form	= new EditParamForm( parent );
}

Manager::~Manager()
{
	for( int i = 0; i < configs.count(); ++i )
	{
		delete configs.at( i );
	}
}

Config *Manager::getConfig(int index) const
{
	if( index < configs.count() )
	{
		return configs.at( index );
	}
	return nullptr;
}

int Manager::getConfigsCount() const
{
	return configs.count();
}

ConfigInfo *Manager::getConfigInfo() const
{
	return config_info;
}

bool Manager::isLoaded() const
{
	return loaded;
}

void Manager::Load(QStringList devices, QStringList modes)
{
	for (int i = 0; i < devices.size(); ++i)
	{
		for (int j = 0; j < modes.size(); ++j)
		{
			QString path( root + "/" + devices.at( i ) + "/" + modes.at( j ) + ".prf" );
			QFile * config_file = new QFile( path );

			if( !config_file->open( QIODevice::ReadOnly ) )
			{
				qWarning() << "Error opening path: " << path;
				continue;
			}

			QString errorStr;
			int errorLine;
			int errorColumn;
			QDomDocument config_dom;
			if( !config_dom.setContent( config_file, true, &errorStr, &errorLine, &errorColumn ) )
			{

				qWarning() << QString("Manager::Load: XML ERROR: Line %1, column %2: %3").arg(errorLine).arg(errorColumn).arg(errorStr);
				continue;
			}
			qDebug() << path << "loaded!";
			config_file->close();

			Config * config = new Config( devices.at( i ), modes.at( j ), config_dom );
			config->setManager( this );
			config->setPath( path );
			config->init();
			configs.append( config );
		}
	}

	loaded = true;
	window->statusBar()->showMessage("All available configurations are loaded! Select test and parameter.");
}

void Manager::Save()
{
	if( !changed )
	{
		return;
	}

	qDebug() << "Manager::Save() : Start";
	QTime t;
	t.start();
	for( int i = 0; i < configs.count(); ++i)
	{
		qDebug() << "Saving path: " << configs.at( i )->getPath();
		QFile file( configs.at( i )->getPath() );
		if( file.open( QIODevice::WriteOnly ) )
		{
			QDomDocument doc = configs.at( i )->getRootDocument();
			QTextStream str( &file );
			str.setCodec(QTextCodec::codecForName("UTF-8"));
//			str << Utils::spacesToTabs( doc.toString() );
			doc.save( str, 4 );
			file.close();
		}
		else
		{
			window->statusBar()->showMessage( "Error writing file!" );
			return;
		}
	}
	window->statusBar()->showMessage( "Changes saved!" );
	changed = false;
}

void Manager::Clear()
{
	for( int i = 0; i < configs.count(); ++i )
	{
		delete configs.at( i );
	}
	configs.clear();

	loaded	= false;
	changed	= false;
}

void Manager::ShowLoadedData()
{
//	for ( int i = 0; i < configs.size(); ++i)
//	{
//		configs.at( i )->printParams();
//	}

	QVector< Config* > vConfigs;
	for( int i = 0; i < configs.count(); ++i )
	{
		vConfigs.append( configs.at( i ) );
	}

	Group * group = new Group( "All objects", vConfigs, window );
	window->addGroup( group );

}

void Manager::setConfigInfo(ConfigInfo *value)
{
	config_info = value;
	value->setManager( this );
}

void Manager::setWindow(MainWindow *value)
{
	window = value;
}

EditParamForm *Manager::getEditForm() const
{
	return edit_form;
}

bool Manager::isChanged() const
{
	return changed;
}

void Manager::setChanged(bool value)
{
	window->statusBar()->showMessage( "Data chagned! Press Save to confirm changes" );
	changed = value;
}

void Manager::setBarText(QString text)
{
	window->statusBar()->showMessage( text );
}

