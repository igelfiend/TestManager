#include "config.h"

#include "test.h"
#include "manager.h"

#include <QDebug>

Config::Config(QString device, QString type, QDomDocument document)
{
	this->device = device;
	this->type = type;
	this->root = document;
}

Config::~Config()
{
	for( int i = 0; i < items.count(); ++i )
	{
		delete items.at( i );
	}
}

void Config::init()
{
	QDomNode node = root.documentElement().firstChild();

	while( !node.isNull() )
	{
		if( node.toElement().tagName() == "main" )
		{
			Test * main_block = new Test("Main", this, node);
			items.append(main_block);
		}
		else
		{
			Test * test = new Test( node.toElement().attribute( "idd" ), this, node );
			test->setVersion( node.toElement().attribute( "version_template" ) );
			test->setKeyName( node.toElement().attribute( "key", "" ) );
			items.append(test);
		}
		node = node.nextSibling();
	}

	for( int i = 0; i < items.count(); ++i )
	{
		items.at( i )->init();
	}
}

void Config::setMain(Item *main_block)
{
	items[0] = main_block;
}

void Config::addTest(Test *test)
{
	items.append(test);
}

Item *Config::getMain() const
{
	return items.at( 0 );
}

Test *Config::getTest(int index) const
{
	if( index < items.size()-1 )
	{
		return items.at( index+1 )->toTest();
	}
	return nullptr;
}

Test *Config::getTest(QString test_name) const
{
	for( int i = 1; i < items.size(); ++i)
	{
		Test * test = items.at( i )->toTest();
		if (test->getName() == test_name)
		{
			return test;
		}
	}
	return nullptr;
}

bool Config::removeTest(int index)
{
	if( index < items.size()-1 )
	{
		items.removeAt( index+1 );
		return true;
	}
	return false;
}

bool Config::removeTest(QString test_name)
{
	for( int i = 1; i < items.size(); ++i)
	{
		if (items.at( i )->getName() == test_name)
		{
			items.removeAt( i );
			return true;
		}
	}
	return false;
}

int Config::getTestCount() const
{
	return items.size()-1;
}

int Config::getItemsCount() const
{
	return items.size();
}

Item *Config::getItem(int index) const
{
	if( index < items.count() )
	{
		return items.at( index );
	}
	return nullptr;
}

Item *Config::getItem(const QString &name, const QString &version , const QString &key) const
{
	for( int i = 0; i < items.count(); ++i )
	{
		if( ( items.at( i )->getName()	  == name	 ) &&
			( items.at( i )->getVersion() == version ) &&
			( items.at( i )->getKeyName() == key	 ) )
		{
			return items.at( i );
		}
	}
	return nullptr;
}

QVector<Item *> Config::getItems(const QString &name, const QString &version) const
{
	QVector< Item * > result;
	for( int i = 0; i < items.count(); ++i )
	{
		if( ( items.at( i )->getName()	  == name ) &&
			( items.at( i )->getVersion() == version ) )
		{
			result << items[ i ];
		}
	}
	return result;
}

QString Config::getFullName() const
{
	return device + " " + type;
}

void Config::printParams()
{
	for( int i = 0; i < items.size(); ++i)
	{
		qDebug() << "\nParams for item[" << i << "]:";
		items[ i ]->ShowAllParams();
	}
}

QDomElement Config::getRoot() const
{
	return root.documentElement();
}

QDomDocument Config::getRootDocument() const
{
	return root;
}

Manager *Config::getManager() const
{
	return manager;
}

void Config::setManager(Manager *value)
{
	manager = value;
}

QString Config::getPath() const
{
	return path;
}

void Config::setPath(const QString &value)
{
	path = value;
}

bool Config::isChanged() const
{
	return changed;
}

void Config::setChanged(bool flag)
{
	changed = flag;
}

