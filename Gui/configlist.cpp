#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QDebug>
#include <QMenu>

#include "configlist.h"
#include "paramlistitem.h"
#include "mainwindow.h"

ConfigList::ConfigList(QWidget *parent)
{
	this->setParent( parent );
	this->parent = parent;
	fAddingGroup = false;
//	MainWindow	*window	 = dynamic_cast<MainWindow *>(this->parent());
//	Manager		*manager = window->getManager();
//	QObject	*obj = this->parent();

	setSelectionMode( QAbstractItemView::ExtendedSelection	);
	setDragDropMode(  QAbstractItemView::InternalMove	);
	setDropIndicatorShown( true );
	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, SIGNAL(customContextMenuRequested(QPoint)),
			this, SLOT(ShowContextMenu(QPoint)));
}

ConfigList::ConfigList(Group *group, QWidget *parent) :
	ConfigList( parent )
{
	setDragEnabled( true );
	setAcceptDrops( true );

//	if( group->hasData() )
//	{
//		setDragEnabled( true );
//		setAcceptDrops( true );
//	}
//	else
//	{
//		setDragEnabled( false );
//		setAcceptDrops( false );
//	}

	this->setMinimumSize( 70, 120 );
	if( !group->hasData() )
	{
		this->setMaximumWidth( 150 );
	}
//	this->setMaximumSize( 150, 120 );
}


void ConfigList::dragEnterEvent(QDragEnterEvent *event)
{
	const ConfigMime *conf_mime = dynamic_cast<const ConfigMime *>( event->mimeData() );
	if( conf_mime->hasConfigs() )
	{
		event->acceptProposedAction();
	}
	else
	{
		QListWidget::dragEnterEvent(event);
	}
}

void ConfigList::dragMoveEvent(QDragMoveEvent *event)
{
	const ConfigMime *conf_mime = dynamic_cast<const ConfigMime *>( event->mimeData() );
	if( conf_mime->hasConfigs() )
	{
		qDebug() << "move is ok";
		event->acceptProposedAction();
	}
	else
	{
		QListWidget::dragMoveEvent(event);
	}
}

void ConfigList::dragLeaveEvent(QDragLeaveEvent *event)
{
	event->accept();
}

void ConfigList::dropEvent(QDropEvent *event)
{
	ConfigMime * mime = dynamic_cast<ConfigMime *>( const_cast<QMimeData *>( event->mimeData() ) );

	if( ( event->source() != this ) && mime->hasConfigs() )
	{
		Group * src_group = dynamic_cast<ConfigList *>(event->source())->getGroup();

		QVector<Param *> params = mime->getParams();
		if (!params.isEmpty())
		{
			qDebug() << "Adding params: " << params.count();
			group->addParams( params );
			qDebug() << "Total params:  " << group->getParamsCount();
			for( int i = 0; i < params.count(); ++i )
			{
				QString key =	( params.at( i )->getOwner()->getKeyName().length() != 0 )
								? QString(" [%1]").arg( params.at( i )->getOwner()->getKeyName() )
								: "";
				ParamListItem * item = new ParamListItem( params.at( i )->getConfig()->getFullName() + key, this, params.at( i ) );
				addItem( item );
			}

			src_group->removeParams( params );
			Param::updateParams( params, group->getGroupParam()->getData() );
		}
		else
		{
			QVector<Config *> configs = mime->getConfigs();
			if ( !configs.isEmpty() )
			{
				group->addConfigs( configs );
				for( int i = 0; i < configs.count(); ++i )
				{
					ParamListItem * item = new ParamListItem( configs.at( i )->getFullName(), this, configs.at( i ) );
					addItem( item );
				}

				src_group->removeConfigs( configs );
			}
		}
		event->acceptProposedAction();
	}
	QListWidget::dropEvent(event);
}

QMimeData *ConfigList::mimeData(const QList<QListWidgetItem *> items) const
{
	QListWidget::mimeData(items);
	ConfigMime * data = new ConfigMime( const_cast<ConfigList *>(this), items );

	return data;
}

bool ConfigList::isAddingGroup() const
{
	return fAddingGroup;
}

QWidget *ConfigList::getParent() const
{
	return parent;
}

void ConfigList::ShowContextMenu(const QPoint &pos)
{
	QMenu menu;
	QPoint globalPos = mapToGlobal(pos);
	menu.addAction( "Show info", this, SLOT( ShowItemInfo() ) );
	menu.addAction( "New value", this, SLOT( CreateNewGroup() ) );
	menu.exec(globalPos);
}

void ConfigList::ShowItemInfo()
{
	for( int i = 0; i < selectedItems().count(); ++i )
	{
		dynamic_cast<ParamListItem *>(selectedItems().at( i ))->ShowInfo();
	}
}

void ConfigList::CreateNewGroup()
{
	qDebug() << "Creating start";
	fAddingGroup = true;
	MainWindow	*window	 = dynamic_cast<MainWindow *>( getParent() );

	qDebug() << "Initing new group";
    Group *new_group = nullptr;
    if( group->getGroupType() == Group::Normal )
    {
        new_group = new Group(group->getButtonTitle(), getParams(), window);
    }
    else if( group->getGroupType() == Group::NoParameter )
    {
        new_group = new Group(group->getButtonTitle(), getConfigs(), window);
    }
    else
    {
        qDebug() << "ConfigList::CreateNewGroup(): Trying to create new group from no-test";
        return;
    }
	qDebug() << "Adding new group";
	window->addGroup( new_group );
	group->removeParams( getParams() );

	QList<QListWidgetItem *> items = selectedItems();

	qDebug() << "Edit params";
	emit(new_group->paramEdited());

	qDebug() << "Deleting items";
	for( int i = 0; i < items.count(); ++i )
	{
		delete items[ i ];
	}
	fAddingGroup = false;
}

Group *ConfigList::getGroup() const
{
	return group;
}

void ConfigList::setGroup(Group *value)
{
	group = value;
}

QVector<Param *> ConfigList::getParams() const
{
	qDebug() << "getParams started";
	QVector<Param *> result;
	for( int i = 0; i < selectedItems().count(); ++i )
	{
		result.append( dynamic_cast<ParamListItem *>(selectedItems().at( i ))->getParam() );
	}

	qDebug() << "params returned";
    qDebug() << "params count: " << result.count();
    return result;
}

QVector<Config *> ConfigList::getConfigs() const
{
    qDebug() << "getConfigs started";
    QVector<Config *> result;
    for( int i = 0; i < selectedItems().count(); ++i )
    {
        Param *param = dynamic_cast<ParamListItem *>(selectedItems().at( i ))->getParam();
        result.append( param->getConfig() );
    }

    qDebug() << "configs returned";
    qDebug() << "configs count: " << result.count();
    return result;
}


ConfigMime::ConfigMime( ConfigList *src, const QList<QListWidgetItem *> items)
{
	config_list = src;

	for( int i = 0; i < items.count(); ++i )
	{
		ParamListItem * item = dynamic_cast<ParamListItem *>( items.at( i ) );
		if( item->getParam() )
		{
			params.append( item->getParam() );
		}

		if( item->getConfig() )
		{
			configs.append( item->getConfig() );
		}
	}
}

ConfigList *ConfigMime::getConfigList() const
{
	return config_list;
}

QVector<Param *> ConfigMime::getParams() const
{
	return params;
}

QVector<Config *> ConfigMime::getConfigs() const
{
	return configs;
}

bool ConfigMime::hasConfigs() const
{
	if( !configs.isEmpty() )
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool ConfigMime::hasParams() const
{
	if( !params.isEmpty() )
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool ConfigMime::hasTests() const
{
	if( !items.isEmpty() )
	{
		return true;
	}
	else
	{
		return false;
	}
}
