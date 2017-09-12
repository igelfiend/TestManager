#include "paramlistitem.h"
#include "param.h"
#include "test.h"
#include "config.h"

#include <QDebug>


ParamListItem::ParamListItem()
{
	initPtr();
//	config	= nullptr;
//	param	= nullptr;
//	item	= nullptr;
}

ParamListItem::ParamListItem(QString text, QListWidget *parent, Param *param) :
	QListWidgetItem(text, parent, ItemType::UserType)
{
//	ParamListItem();
	initPtr();
	this->param = param;
}

ParamListItem::ParamListItem(QString text, QListWidget *parent, Item *item) :
	QListWidgetItem(text, parent, ItemType::UserType)
{
//	ParamListItem();
	initPtr();
	this->item = item;
}

ParamListItem::ParamListItem(QString text, QListWidget *parent, Config *config) :
	QListWidgetItem(text, parent, ItemType::UserType)
{
//	ParamListItem();
	initPtr();
	this->config = config;
}

ParamListItem::~ParamListItem()
{

}

Param *ParamListItem::getParam() const
{
	return param;
}

Item *ParamListItem::getItem() const
{
	if( param )
	{
		return param->getOwner();
	}
	else
	{
		return item;
	}
}

Config *ParamListItem::getConfig() const
{
	if( param )
	{
		return param->getOwner()->getConfig();
	}
	else if ( item )
	{
		return item->getConfig();
	}
	else
	{
		return config;
	}
}

void ParamListItem::ShowInfo() const
{
	if( param )
	{
		qDebug()	<< " /*----------------Parameter-Info--------------*/\n"
					<< "Name = "	<< param->getName() << "\n"
					<< "Data = "	<< param->getData() << "\n"
					<< "Owner = "	<< ( (param->getOwner()) ? param->getOwner()->getName() : QString("No owner") ) << "\n"
					<< "/*------------------End-of-Info---------------*/";
	}
	else
	{
		qDebug() << "Error! No param detected!";
	}
}

void ParamListItem::initPtr()
{
	config	= nullptr;
	param	= nullptr;
	item	= nullptr;
}

void ParamListItem::setParam(Param *value)
{
	param = value;
}
