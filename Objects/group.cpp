
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
#include <QGroupBox>
#include <QScrollArea>

#include "config.h"
#include "param.h"
#include "test.h"
#include "paramlistitem.h"
#include "group.h"
#include "manager.h"
#include "editparamform.h"
#include "mainwindow.h"
#include "configlist.h"
#include "test_info.h"
#include "tableparam.h"
#include "spoilergroupbox.h"

Group::Group()
{
	group_param	= nullptr;
	button		= nullptr;
	list		= nullptr;
	lb			= nullptr;
}

Group::Group(QString title, QVector<Config *> configs, MainWindow *window) :
	BaseGroup( configs, window )
{
	initPtr();

	container	= new QVBoxLayout;
	lb			= new QLabel(title);
	list		= new ConfigList( this, window );

	list->setGroup( this );
	for( int i = 0; i < configs.count(); ++i )
	{
		Param *t_param = nullptr;
        ParamListItem * item = new ParamListItem(configs.at( i )->getFullName(), list, t_param );
		list->addItem( item );
	}

	container->addWidget(lb);
	container->addWidget(list);
}

Group::Group(QString title, QVector<Item *> tests, MainWindow *window):
	BaseGroup( tests, window )
{
	initPtr();

	container	= new QVBoxLayout;
	lb			= new QLabel(title);
	list		= new ConfigList( this, window );

	list->setGroup( this );
	for( int i = 0; i < tests.count(); ++i )
	{
		QString key = ( tests.at( i )->getKeyName().length() != 0 )
						? QString(" [%1]").arg( tests.at( i )->getKeyName() )
						: QString();
		Param *t_param = nullptr;
		ParamListItem * item = new ParamListItem( tests.at( i )->getConfig()->getFullName() + key, list, t_param );
		list->addItem( item );

//		list->addItem( tests.at( i )->getConfig()->getFullName() + key );
	}

	container->addWidget(lb);
	container->addWidget(list);
}

Group::Group(QString title, QVector<Param *> params, MainWindow *window) :
	BaseGroup( params, window )
{
    qDebug() << "Group constructor(params based)";
	initPtr();

	if( params.count() > 0 )
	{
		this->group_param	= new Param( params.at( 0 ) );
	}

    qDebug() << "group param setted";
	container	= new QVBoxLayout;
	button		= new QPushButton( title );
	list		= new ConfigList( this, window );

	button->setFlat( true );
	list->setGroup( this );
    qDebug() << "Cycle started";
	for( int i = 0; i < params.count(); ++i )
	{
		QString key =	( params.at( i )->getOwner()->getKeyName().length() != 0 )
						? QString(" [%1]").arg( params.at( i )->getOwner()->getKeyName() )
						: "";
        qDebug() << "New paramlist item created";
		ParamListItem * item = new ParamListItem(params.at( i )->getOwner()->getConfig()->getFullName() + key, list, params.at( i ));
		list->addItem( item );
	}

	connect( list, list->currentRowChanged, this, rowChanged );
	connect( button, button->clicked, this, paramEdited );
	if( params.count() > 0 )
	{
		EditParamForm * edit_form = params.at( 0 )->getOwner()->getConfig()->getManager()->getEditForm();
		edit_form->clearConnections();
		edit_form->getConnections() <<  connect( edit_form, edit_form->accepted, this, editAccepted ) ;
	}

	container->addWidget( button );
	container->addWidget( list   );
    qDebug() << "Group constructor ends(params based)";
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
}

QString Group::getButtonTitle() const
{
	if( button )
	{
		return button->text();
	}
	else
	{
		return QString();
	}
}

void Group::initPtr()
{
	lb			= nullptr;
	list		= nullptr;
	button		= nullptr;
	container	= nullptr;
}

Group::GroupType Group::getGroupType() const
{
    return group_type;
}

void Group::setGroupType(const GroupType &value)
{
    group_type = value;
}

Param *Group::getGroupParam() const
{
    return group_param;
}

void Group::rowChanged(int row)
{
    if( list->isAddingGroup() )
    {
        return;
	}
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

	Manager *manager	= params.first()->getOwner()->getConfig()->getManager();
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
		int	rows_count;
		if( columns.isEmpty() )
		{
			columns.append( param_info->getName() );
			rows_count = ( data.count() > 0 ) ? ( data.at( 0 ).count() ) : 0 ;
		}
		else
		{
			rows_count = data.count();
		}

		QHBoxLayout		*h_layout	= new QHBoxLayout();
		QVBoxLayout		*v_layout	= new QVBoxLayout();
		QPushButton		*plus		= new QPushButton( "+" );
		QPushButton		*minus		= new QPushButton( "-" );
		TableParam		*table		= new TableParam( rows_count, columns.count(),  manager->getEditForm() );

		h_layout->addWidget( table );
		v_layout->addWidget( plus );
		v_layout->addWidget( minus );
		v_layout->addSpacing( 500 );

		h_layout->addLayout( v_layout );

		minus->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );
		minus->setMaximumWidth( 50 );

		plus->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );
		plus->setMaximumWidth( 50 );

		connections << QObject::connect( plus,	SIGNAL(clicked(bool)),
										 table,	SLOT(insertRowWhereSelected(bool)));

		connections << QObject::connect( minus,	SIGNAL(clicked(bool)),
										 table,	SLOT(removeRowWhereSelected(bool)));


		qDebug() << "Columns = " << columns;
		table->setHorizontalHeaderLabels( columns );
		table->uploadData( data, group_param->getStrFormat() );
		table->correctSize( 400 );

		manager->getEditForm()->getLayout()->addLayout( h_layout );
		manager->getEditForm()->adjustSize();

		break;
	}
	case Complex:
	{
		qDebug() << "Complex case";
		fields.clear();
		SpoilerGroupBox *gbox	= Utils::NodeToGroupBox( group_param->getNode(), fields );
		QScrollArea *area		= new QScrollArea();
		area->setWidgetResizable( true );
		area->setWidget( gbox );
		manager->getEditForm()->getLayout()->addWidget( area );
//		manager->getEditForm()->setMinimumSize( 600, 800 );

		break;
	}
	default:
		qDebug() << "Default case";
		QPlainTextEdit *edit = new QPlainTextEdit( text, manager->getEditForm() );
		edit->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred );
		edit->setMinimumSize( 500, 300 );
		manager->getEditForm()->getLayout()->addWidget( edit );
		manager->getEditForm()->adjustSize();
		manager->getEditForm()->setDataType( StringType::None );
	}

//	manager->getEditForm()->setText( params.at( 0 )->getData() );
	manager->getEditForm()->setCaller( this );
	manager->getEditForm()->setWindowTitle( "Editing: " + button->text() );
	manager->getEditForm()->exec();
}

void Group::editAccepted()
{
	if(params.count() == 0)
	{
		return;
	}

	Manager * manager = params.at( 0 )->getOwner()->getConfig()->getManager();
	if( manager->getEditForm()->getCaller() != this )
	{
		return;
	}

	manager->setChanged( true );

	QVector<Param *> params_all;
	params_all.append( params );
	params_all.append( group_param );

	QString new_data;
	if( param_info->getParamType() == Complex )
	{
		for( int i = 0; i < fields.count(); ++i )
		{
			QString content = fields[ i ].second->toPlainText();
			QDomText txt = fields[ i ].first.ownerDocument().createTextNode( content );
			Utils::removeChilds( fields[ i ].first );

			fields[ i ].first.appendChild( txt );
		}

		QString str;
		QTextStream s(&str);
		group_param->getNode().save(s, QDomNode::CDATASectionNode);

		qDebug() << "Updated node: ";
		qDebug() << str.toStdString().c_str();

		Param::updateParams( params_all, str );
	}
	else
	{
		EditParamForm	*edit_form	= manager->getEditForm();
		new_data	= edit_form->getText();
		qDebug() << "NEW DATA: " << new_data;

		Param::updateParams( params_all, new_data );
		button->setText( new_data );
	}
}

BaseGroup::BaseGroup()
{
	param_info	= nullptr;
	container	= nullptr;
	has_data	= false;
	window		= nullptr;
}

BaseGroup::BaseGroup(QVector<Config *> configs, MainWindow *window)
{
	has_data	= false;
	param_info	= window->getCurrentParam();
	this->configs	= configs;
	this->window	= window;
}

BaseGroup::BaseGroup(QVector<Item *> items, MainWindow *window)
{
	has_data	= false;
	param_info	= window->getCurrentParam();
	this->items	= items;
	this->window	= window;
}

BaseGroup::BaseGroup(QVector<Param *> params, MainWindow *window)
{
	has_data	= true;
	param_info	= window->getCurrentParam();
	this->params	= params;
	this->window	= window;
}

BaseGroup::~BaseGroup()
{
	Utils::clearLayout( container );
	delete container;
	foreach (auto var, connections)
	{
		QObject::disconnect( var );
	}
}

void BaseGroup::addConfigs(QVector<Config *> conf_array)
{
	configs.append( conf_array );
}

void BaseGroup::addParams(QVector<Param *> param_array)
{
	params.append( param_array );
}

bool BaseGroup::removeConfig(Config *conf)
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

bool BaseGroup::removeParam(Param *param)
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

void BaseGroup::removeConfigs(QVector<Config *> conf_vector)
{
	for( int i = 0; i < conf_vector.count(); ++i )
	{
		removeConfig( conf_vector.at( i ) );
	}
}

void BaseGroup::removeParams(QVector<Param *> param_vector)
{
	for( int i = 0; i < param_vector.count(); ++i )
	{
		removeParam( param_vector.at( i ) );
	}
}

int BaseGroup::getParamsCount() const
{
	return params.count();
}

bool BaseGroup::hasData() const
{
	return has_data;
}

QBoxLayout *BaseGroup::getContainer() const
{
	return container;
}

TestParam *BaseGroup::getParamInfo() const
{
	return param_info;
}

Group *BaseGroup::toGroup()
{
	return dynamic_cast<Group *>( this );
}

CompareGroup *BaseGroup::toCompareGroup()
{
	return dynamic_cast<CompareGroup *>( this );
}

CompareGroup::CompareGroup()
{

}

CompareGroup::CompareGroup( QString textarea_text, QVector<Config *> configs, MainWindow *window):
	BaseGroup( configs, window )
{
	QString title;
	for( int i = 0; i < configs.count(); ++i )
	{
		title += configs.at( i )->getFullName() + ",";
		if( (i+1) % 3 == 0 )
		{
			title += "\n";
		}
		else
		{
			title += " ";
		}
	}
	title.remove( title.length() - 2, 2 );

	label		= new QLabel( title );
	container	= new QVBoxLayout;
	textarea	= new QPlainTextEdit( textarea_text );

	container->addWidget( label );
	container->addWidget( textarea );
}

CompareGroup::CompareGroup(QVector<Param *> params, MainWindow *window):
	BaseGroup( params, window )
{
	QString title;
	for( int i = 0; i < params.count(); ++i )
	{
		title += params.at( i )->getConfig()->getFullName() + ",";
		if( (i+1) % 3 == 0 )
		{
			title += "\n";
		}
		else
		{
			title += " ";
		}
	}
	title.remove( title.length() - 2, 2 );

	QString content = "";
	if( params.count() == 0 )
	{
		content = "";
	}
	else
	{
		content = params.at( 0 )->getData();
	}

	label		= new QLabel( title );
	container	= new QVBoxLayout;
	textarea	= new QPlainTextEdit( content );


	QFont font = textarea->document()->defaultFont();
	QFontMetrics metrics = QFontMetrics(font);
	QSize textSize		 = metrics.size( 0, textarea->toPlainText() );

	int height	= textSize.height() + 30;
	int width	= textSize.width() + 30;

	textarea->setMinimumSize( width, height );

	container->addWidget( label );
	container->addWidget( textarea );
}

CompareGroup::~CompareGroup()
{

}
