#ifndef GROUP_H
#define GROUP_H

#include <QVector>
#include <QObject>
#include <QPushButton>

#include <utils.h>

class QString;
class QBoxLayout;
class QLabel;
class QListWidget;
class ConfigList;
class QTableWidget;
class QPlainTextEdit;

class Config;
class Param;
class Item;
class MainWindow;
class TestParam;
class TableParam;
class Group;
class CompareGroup;

class BaseGroup: public QObject
{
public:
	BaseGroup();
	BaseGroup( QVector< Config* >	configs,	MainWindow *window );
	BaseGroup( QVector< Item* >		items,		MainWindow *window );
	BaseGroup( QVector< Param* >	params,		MainWindow *window );
	virtual ~BaseGroup();

	void	addConfigs( QVector<Config *> conf_array );
	void	addParams( QVector<Param *> param_array );

	bool	removeConfig( Config	*conf );
	bool	removeParam( Param		*param );

	void	removeConfigs( QVector<Config *>	conf_vector );
	void	removeParams( QVector<Param *>		param_vector );
	int		getParamsCount() const;

	bool	hasData() const;
	QBoxLayout	*getContainer() const;
	TestParam	*getParamInfo() const;

	Group			*toGroup();
	CompareGroup	*toCompareGroup();


protected:
	bool		has_data;
	MainWindow	*window;
	QBoxLayout	*container;
	TestParam	*param_info;
	QVector< Config* >	configs;
	QVector< Param* >	params;
	QVector< Item* >	items;
	QList<QMetaObject::Connection>	connections;
};

class Group: public BaseGroup
{

	Q_OBJECT
public:
    enum GroupType
    {
        Normal = 0,
        NoTest = 1,
        NoParameter = 2
    };

	Group();
	Group(QString title, QVector< Config* >	configs,	MainWindow *window);
	Group(QString title, QVector< Item* >	tests,		MainWindow *window);
	Group(QString title, QVector< Param* >	params,		MainWindow *window);
	~Group();

	Param		*getGroupParam() const;
	QString		getButtonTitle() const;

    GroupType getGroupType() const;
    void setGroupType(const GroupType &value);

private:
    void	initPtr();
    Param	*group_param;
    QLabel	*lb;

    QPushButton *button;
	ConfigList	*list;
	FIELDS	fields;
    GroupType group_type;

public slots:
	void	rowChanged( int row );
	void	paramEdited();
	void	editAccepted();
};

class CompareGroup: public BaseGroup
{
	Q_OBJECT
public:
	CompareGroup();
	CompareGroup(QString textarea_text, QVector< Config* >	configs,	MainWindow *window );
	CompareGroup( QVector< Param* >		params,		MainWindow *window );
	~CompareGroup();
private:
	QLabel		*label;
	QPlainTextEdit	*textarea;
};


#endif // GROUP_H
