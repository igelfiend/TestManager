
#include <QString>
#include <QDomElement>
#include <QFile>
#include <QDebug>
#include <QApplication>

#include "test_info.h"
#include "manager.h"

ConfigInfo::ConfigInfo( Manager *manager )
{
	this->manager = manager;
	manager->setConfigInfo( this );
	init();
}

ConfigInfo::~ConfigInfo()
{

}

void ConfigInfo::init()
{
	qDebug() << "Initialisation configuration of tests...";

	QString path = QApplication::applicationDirPath() + "/" +  "params.xml";
	QFile * params_file = new QFile(path);

	if( !params_file->open(QIODevice::ReadOnly) )
	{
		qWarning() << "Error opening path: " << path;
		manager->setBarText( "Error opening device file!" );
		return;
	}

	QString errorStr;
	int errorLine;
	int errorColumn;
	QDomDocument param_dom;
	if( !param_dom.setContent(params_file, true, &errorStr, &errorLine, &errorColumn))
	{

		qWarning() << QString("TestInfo::init: XML ERROR: Line %1, column %2: %3").arg(errorLine).arg(errorColumn).arg(errorStr);
		return;
	}
	qDebug() << path << "loaded!";
	params_file->close();

	QDomElement root = param_dom.documentElement();
	QDomElement test_node = root.firstChildElement("test");

	while( !test_node.isNull() )
	{
		TestInfo * test = new TestInfo(test_node.attribute("name", ""));
		QDomElement param_node = test_node.firstChildElement("param");

		while( !param_node.isNull() )
		{
			// Reading all attributes
			QString param_path_str = param_node.attribute("path",	"");
			QString param_name_str = param_node.attribute("name",	"");
			QString param_type_str = param_node.attribute("type",	"");
			QString param_frmt_str = param_node.attribute("format", "");

			// Detecting format of data (for Table-type)
			ParamType param_type;
			StringType str_type;

			QVector<QString>	format_list_str = { "bracket_matrix", "bracket_array", "matrix", "array" };
			StringType			format_list_type[4] = {	StringType::BracketsMatrix, StringType::BracketsArray,
														StringType::Matrix,			StringType::Array};

			int indx = format_list_str.indexOf(param_frmt_str);

			if( indx < 0 )
			{
				str_type = StringType::None;
			}
			else
			{
				str_type = format_list_type[ indx ];
			}

			// Detecing data type
			if( param_type_str == "simple" )
			{
				param_type = ParamType::Simple;
			}
			else
				if( param_type_str == "table" )
			{
				param_type = ParamType::Table;
			}
			else
			{
				param_type = ParamType::Complex;
			}


			//Create new param, each case for different datatype
			TestParam	*param;
			if( param_type == ParamType::Table )
			{
				QStringList column_list;

				QDomElement param_column = param_node.firstChildElement("column");

				while( !param_column.isNull() )
				{
					column_list.append( param_column.attribute( "name" ) );
					param_column = param_column.nextSiblingElement( "column" );
				}

				param = new TestParam( param_path_str, param_name_str, param_type, str_type, column_list );
			}
			else
			{
				param = new TestParam( param_path_str, param_name_str, param_type );
			}
			test->addParam(param);

			param_node = param_node.nextSiblingElement("param");
		}
		tests.append(test);
		test_node = test_node.nextSiblingElement("test");
	}

	manager->setBarText( "All configs loaded! Select devices and modes, then press Load" );
}

TestInfo *ConfigInfo::getTest(int index) const
{
	if( index < tests.count() )
	{
		return tests.at( index );
	}
	return nullptr;
}

TestInfo *ConfigInfo::getTest(QString name) const
{
	for( int i = 0; i < tests.count(); ++i )
	{
		if( tests.at( i )->getName() == name )
		{
			return tests.at( i );
		}
	}
	return nullptr;
}

int ConfigInfo::getTestsCount() const
{
	return tests.count();
}

Manager *ConfigInfo::getManager() const
{
	return manager;
}

void ConfigInfo::setManager(Manager *value)
{
	manager = value;
}

TestInfo::TestInfo()
{

}

TestInfo::TestInfo(QString name)
{
	this->name = name;
}

TestInfo::~TestInfo()
{

}

TestParam *TestInfo::getParam( int index)
{
	if( index < params.count() )
	{
		return params.at( index );
	}
	return nullptr;
}

TestParam *TestInfo::getParam(QString name)
{
	for( int i = 0; i < params.count(); ++i )
	{
		if( params.at( i )->getName() == name )
		{
			return params.at( i );
		}
	}
	return nullptr;
}

void TestInfo::addParam(TestParam *param)
{
	params.append( param );
}

int TestInfo::getParamCount()
{
	return params.count();
}

QString TestInfo::getName() const
{
	return name;
}


TestParam::TestParam(QString path, QString name, ParamType type)
{
	this->path	= path;
	this->name	= name;
	this->param_type	= type;
	this->string_type	= StringType::None;
}

TestParam::TestParam(QString path, QString name, ParamType type, StringType str_type, QStringList column_names)
{
	this->path	= path;
	this->name	= name;
	this->param_type	= type;
	this->column_names	= column_names;
	this->string_type	= str_type;
}

TestParam::~TestParam()
{

}

QString TestParam::getPath() const
{
	return path;
}

void TestParam::setPath(const QString &value)
{
	path = value;
}

QString TestParam::getName() const
{
	return name;
}

void TestParam::setName(const QString &value)
{
	name = value;
}

ParamType TestParam::getParamType() const
{
	return param_type;
}

void TestParam::setParamType(const ParamType &value)
{
	param_type = value;
}

QStringList TestParam::getColumnNames() const
{
	return column_names;
}

void TestParam::setColumnNames(const QStringList &value)
{
	column_names = value;
}

StringType TestParam::getStringType() const
{
	return string_type;
}
