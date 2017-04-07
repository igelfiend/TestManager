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
	if( group->hasData() )
	{
		setDragEnabled( true );
		setAcceptDrops( true );
	}
	else
	{
		setDragEnabled( false );
		setAcceptDrops( false );
	}
}


void ConfigList::dragEnterEvent(QDragEnterEvent *event)
{
	if ( dynamic_cast<const ConfigMime *>( event->mimeData() )->hasConfigs() )
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
	if (dynamic_cast<const ConfigMime *>( event->mimeData() )->hasConfigs() )
	{
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

	if( ( event->source() != this ) && ( mime->hasConfigs() ) )
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
				ParamListItem * item = new ParamListItem( params.at( i )->getConfig()->getFullName(), this, params.at( i ) );
				addItem( item );
			}

			src_group->removeParams( params );
			Param::updateParams( params, group->getGroupParam()->getData() );
		}
		else
		{
			QVector<Config *> configs = mime->getConfigs();
			if (!configs.isEmpty())
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

void ConfigList::ShowContextMenu(QPoint pos)
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
	MainWindow	*window	 = dynamic_cast<MainWindow *>(parent());
	Manager		*manager = window->getManager();

	Group *group = new Group(group->getButtonTitle(), getParams(), window);
	window->addGroup( group );
	emit(group->paramEdited());
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
	QVector<Param *> result;
	for( int i = 0; i < selectedItems().count(); ++i )
	{
		result.append( dynamic_cast<ParamListItem *>(selectedItems().at( i ))->getParam() );
	}

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
	if( configs.count() > 0 )
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
	if( params.count() > 0 )
	{
		return true;
	}
	else
	{
		return false;
	}
}
