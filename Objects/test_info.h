#ifndef TEST_INFO_H
#define TEST_INFO_H

#include <QList>
#include "utils.h"


enum ParamType
{
	Simple,
	Table,
	Complex
};

class QString;
class Manager;
class TestInfo;
struct TestParam;

class ConfigInfo
{
public:
	ConfigInfo(Manager *manager);
	~ConfigInfo();

	void	init();

	TestInfo    *getTest( int index ) const;
	TestInfo    *getTest(const QString &name , const QString &version) const;

	int	getTestsCount() const;

	void    setManager(Manager *value);
	Manager *getManager() const;

private:
        Manager *manager;
        QList< TestInfo *> tests;
};


class TestInfo
{
public:
	TestInfo(){}
	TestInfo( const QString &_name, const QString &_version ):
		name( _name ), version( _version ){}
	~TestInfo(){}

	TestParam	*getParam( int index );
	TestParam	*getParam( QString name );
	int			getParamCount();
	QString		getName() const;
	QString		getVersion() const;
	QString		getVersionedName() const;

	void		addParam( TestParam *param );

private:
	QString name;
	QString version;
	QList< TestParam *> params;

};


class TestParam
{
public:
    TestParam();
    TestParam( QString path, QString name, ParamType type );
	TestParam( QString path, QString name, ParamType type, StringType str_type, QStringList column_names );
    ~TestParam();

    QString     getPath()   const;
    QString     getName()   const;
    ParamType   getParamType()      const;
    QStringList getColumnNames()    const;

    void setPath(const QString &value);
    void setName(const QString &value);
    void setParamType(const ParamType &value);
    void setColumnNames(const QStringList &value);

	StringType getStringType() const;

private:
	QString path;
	QString name;
    ParamType   param_type;
	StringType	string_type;
    QStringList column_names;
};

#endif // TEST_INFO_H
