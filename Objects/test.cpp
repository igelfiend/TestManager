#include "test.h"

#include <QString>
#include <QDebug>
#include <QtGlobal>
#include <QMessageBox>
#include "param.h"
#include "config.h"
#include "manager.h"
#include "test_info.h"
#include "utils.h"

Test::Test(QString name, Config *config, QDomNode root):Item(name, config, root)
{
}

Test::~Test()
{

}

void Test::setTemplateDir(const QString &path)
{
	template_dir = path;
}

void Test::setInstructionPath(const QString &path)
{
	instruction_path = path;
}

QString Item::getVersion() const
{
	return version;
}

QString Item::getKeyName() const
{
	return key_name;
}

void Item::setKeyName(const QString &value)
{
	key_name = value;
}

void Item::setVersion(const QString &value)
{
	version = value;
}

Item::Item(QString name, Config *config, QDomNode root)
{
	this->name = name;
	this->config = config;
	this->root = root;
}

Item::~Item()
{
	for( int i = 0; i < params.count(); ++i )
	{
		delete params.at( i );
	}
}

void Item::init()
{
	ConfigInfo	*info = config->getManager()->getConfigInfo();
	TestInfo	*test = info->getTest( name, version );

	if( !test )
	{
		QMessageBox mbox;
		mbox.setText(" Initialization error:\n Error in config: " + this->config->getFullName()
					 + "\n In test: " + this->name
					 + "\n such template in params.xml not found!");
		mbox.exec();
		return;
	}

	for( int i = 0; i < test->getParamCount(); ++i)
	{
		TestParam *param = test->getParam( i );

		QDomElement param_node = Utils::getXmlElement( root, param->getPath(), Qt::CaseInsensitive );
		if( param_node.isNull() )
		{
			continue;
		}

		Param * tParam = new Param( param->getName(), param_node,
									param->getParamType(), param->getStringType() );
		addParam( tParam );
	}
}

void Item::addParam(Param *param)
{
	params.append( param );
	param->setOwner( this );
}

Param *Item::getParam(int index) const
{
	if( index < params.count() )
	{
		return params.at( index );
	}
	return nullptr;
}

Param *Item::getParam(QString param_name) const
{
	for( int i = 0; i < params.count(); ++i )
	{
		if( params.at( i )->getName() == param_name )
		{
			return params.at( i );
		}
	}
	return nullptr;
}

bool Item::removeParam(int index)
{
	if( index < params.count() )
	{
		params.removeAt( index );
		return true;
	}
	return false;
}

bool Item::removeParam(QString param_name)
{
	for( int i = 0; i < params.count(); ++i )
	{
		if( params.at( i )->getName() == param_name )
		{
			params.removeAt( i );
			return true;
		}
	}
	return false;
}

Test *Item::toTest()
{
	return dynamic_cast<Test *>(this);
}

Main *Item::toMain()
{
	return dynamic_cast<Main *>(this);
}

const QDomNode Item::getRoot() const
{
	return root;
}

void Item::ShowAllParams()
{
	qDebug() << "------------" << name << "------------";
	for( int i = 0; i < params.size(); ++i )
	{
		qDebug() << params[ i ]->getName() << ":" << params[ i ]->getData().toStdString().c_str();
	}

}

QString Item::getName() const
{
	return name;
}

Config *Item::getConfig() const
{
	return config;
}


Main::Main(QString name, Config *config, QDomNode root):Item(name, config, root)
{

}

Main::~Main()
{

}
