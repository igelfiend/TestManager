#include "group.h"

#include <QString>
#include <QBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPlainTextEdit>
#include <QListWidget>
#include <QDomElement>
#include <QDebug>
#include <QTableWidget>
#include <QHeaderView>

#include "config.h"
#include "param.h"
#include "paramlistitem.h"
#include "manager.h"
#include "editparamform.h"
#include "mainwindow.h"
#include "configlist.h"
#include "utils.h"
#include "test_info.h"
#include "tableparam.h"

Group::Group()
{
	group_param	= nullptr;
	container	= nullptr;
	button		= nullptr;
	list		= nullptr;
	lb			= nullptr;
}

Group::Group(QString title, QVector<Config *> configs, MainWindow *window) : Group()
{
	this->configs	= configs;
	this->window	= window;
	this->has_data	= false;

	container	= new QVBoxLayout;
	lb			= new QLabel(title);
	list		= new ConfigList( this );

	list->setGroup( this );
	for( int i = 0; i < configs.count(); ++i )
	{
		list->addItem( configs.at( i )->getFullName() );
	}

	container->addWidget(lb);
	container->addWidget(list);
}

Group::Group(QString title, QVector<Param *> params, MainWindow *window) : Group()
{
	this->params	= params;
	this->window	= window;
	this->has_data	= true;
	if( params.count() > 0 )
	{
		this->group_param	= new Param( params.at( 0 ) );
	}

	container	= new QVBoxLayout;
	button		= new QPushButton( title );
	list		= new ConfigList( this );

	button->setFlat( true );
	list->setGroup( this );
	for( int i = 0; i < params.count(); ++i )
	{
		ParamListItem * item = new ParamListItem(params.at( i )->getOwner()->getConfig()->getFullName(), list, params.at( i ));
		list->addItem( item );
	}

	connect( list, list->currentRowChanged, this, rowChanged );
	connect( button, button->clicked, this, paramEdited );
	if( params.count() > 0 )
	{
		EditParamForm * edit_form = params.at( 0 )->getOwner()->getConfig()->getManager()->getEditForm();
		connect( edit_form, edit_form->accepted, this, editAccepted );
	}
	param_info = window->getCurrentParam();

	container->addWidget( button );
	container->addWidget( list   );
}

Group::~Group()
{
	if( lb )
	{
		delete lb;
	}

	if( button )
	{
		delete button;
	}
	delete list;
	delete container;
}

void Group::addConfigs(QVector<Config *> conf_array)
{
	configs.append( conf_array );
}

void Group::addParams(QVector<Param *> param_array)
{
	params.append( param_array );
}

bool Group::removeConfig(Config *conf)
{
	for( int i = 0; i < configs.count(); ++i )
	{
		if( configs.at( i ) == conf )
		{
			configs.removeAt( i );
			return true;
		}
	}
	return false;
}

bool Group::removeParam(Param *param)
{
	for( int i = 0; i < params.count(); ++i )
	{
		if( params.at( i ) == param )
		{
			params.removeAt( i );
			return true;
		}
	}
	return false;
}

void Group::removeConfigs(QVector<Config *> conf_vector)
{
	for( int i = 0; i < conf_vector.count(); ++i )
	{
		removeConfig( conf_vector.at( i ) );
	}
}

void Group::removeParams(QVector<Param *> param_vector)
{
	for( int i = 0; i < param_vector.count(); ++i )
	{
		removeParam( param_vector.at( i ) );
	}
}

QBoxLayout *Group::getContainer() const
{
	return container;
}

bool Group::hasData() const
{
	return has_data;
}

TestParam *Group::getParamInfo() const
{
	return param_info;
}

Param *Group::getGroupParam() const
{
	return group_param;
}

void Group::rowChanged(int row)
{
	qDebug() << "Selected " << row << " row";
	if( row != -1 )
	{
//		if( params.at( row )->getStrFormat() == StringType::BracketsMatrix )
//		{
//			qDebug() << "BracketsMatrix ";
//		}
//		else if( params.at( row )->getStrFormat() == StringType::Matrix )
//		{
//			qDebug() << "Matrix ";
//		}
//		else if( params.at( row )->getStrFormat() == StringType::BracketsArray )
//		{
//			qDebug() << "BracketsArray ";
//		}
//		else if( params.at( row )->getStrFormat() == StringType::Array )
//		{
//			qDebug() << "Array ";
//		}
//		else
//		{
//			qDebug() << "Default type";
//		}

		params.at( row )->getOwner()->ShowAllParams();
	}

}

void Group::paramEdited()
{
	if( params.count() == 0 )
	{
		return;
	}

	Manager *manager	= params.at( 0 )->getOwner()->getConfig()->getManager();
	QString	text		= group_param->getData();

	Utils::clearLayout( manager->getEditForm()->getLayout() );
	foreach (auto var, connections)
	{
		QObject::disconnect(var);
	}

	switch( group_param->getType() )
	{
	case Table:
	{

		QVector<QVector<QString>> data = Utils::parseString( text );
		manager->getEditForm()->setDataType( group_param->getStrFormat() );

		QStringList	columns	= param_info->getColumnNames();

		QHBoxLayout		*h_layout	= new QHBoxLayout();
		QVBoxLayout		*v_layout	= new QVBoxLayout();
		QPushButton		*plus		= new QPushButton( "+" );
		QPushButton		*minus		= new QPushButton( "-" );
		TableParam		*table		= new TableParam( data.count(), columns.count(),  manager->getEditForm() );

		h_layout->addWidget( table );
		v_layout->addWidget( plus );
		v_layout->addWidget( minus );
		v_layout->addSpacing( 500 );

		h_layout->addLayout( v_layout );

		minus->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );
		minus->setMaximumWidth( 50 );

		plus->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );
		plus->setMaximumWidth( 50 );

		connections << QObject::connect(plus,	SIGNAL(clicked(bool)),
										table,	SLOT(insertRowWhereSelected(bool)));

		connections << QObject::connect(minus,	SIGNAL(clicked(bool)),
										table,	SLOT(removeRowWhereSelected(bool)));

		table->setHorizontalHeaderLabels( columns );
		table->uploadData( data, group_param->getStrFormat() );
		table->correctSize( 400 );

		manager->getEditForm()->getLayout()->addLayout( h_layout );
		manager->getEditForm()->adjustSize();

		break;
	}
	default:
		QPlainTextEdit *edit = new QPlainTextEdit( text, manager->getEditForm() );
		edit->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred );
		edit->setMinimumSize( 500, 300 );
		manager->getEditForm()->getLayout()->addWidget( edit );
		manager->getEditForm()->adjustSize();
		manager->getEditForm()->setDataType( StringType::None );
	}

//	manager->getEditForm()->setText( params.at( 0 )->getData() );
	manager->getEditForm()->setCaller( this );
	manager->getEditForm()->exec();
}

void Group::editAccepted()
{
	Manager * manager = params.at( 0 )->getOwner()->getConfig()->getManager();
	if( manager->getEditForm()->getCaller() != this )
	{
		return;
	}

	manager->setChanged( true );
	EditParamForm	*edit_form	= manager->getEditForm();
	QString			new_data	= edit_form->getText();
	qDebug() << "NEW DATA: " << new_data;


	QVector<Param *> params_all;
	params_all.append( params );
	params_all.append( group_param );

	Param::updateParams( params_all, new_data );

	if( params_all.at( 0 )->getType() != ParamType::Complex )
	{
		button->setText( new_data );
	}
	qDebug() << "Update complite!";
//	window->clearGroups();
	//	window->loadGroups();
}
