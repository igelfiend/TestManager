#ifndef GROUP_H
#define GROUP_H

#include <QVector>
#include <QObject>
#include <QPushButton>

class QString;
class QBoxLayout;
class QLabel;
class QListWidget;
class ConfigList;
class QTableWidget;

class Config;
class Param;
class MainWindow;
class TestParam;

class Group : public QObject
{
	Q_OBJECT
public:
	Group();
	Group(QString title, QVector< Config* >	configs,	MainWindow *window);
	Group(QString title, QVector< Param* >	params,		MainWindow *window);
	~Group();

	void	addConfigs( QVector<Config *> conf_array );
	void	addParams( QVector<Param *> param_array );

	bool	removeConfig( Config	*conf );
	bool	removeParam( Param		*param );

	void	removeConfigs( QVector<Config *>	conf_vector );
	void	removeParams( QVector<Param *>		param_vector );
	Param	*getGroupParam() const;

	QBoxLayout	*getContainer() const;


	bool hasData() const;

	TestParam *getParamInfo() const;

private:
	bool	has_data;
	Param	*group_param;
	QLabel	*lb;
	QBoxLayout	*container;
	QPushButton *button;
	ConfigList	*list;
	MainWindow	*window;
	TestParam	*param_info;
	QTableWidget		*table;
	QVector< Config* >	configs;
	QVector< Param* >	params;
	QList<QMetaObject::Connection>	connections;



public slots:
	void	rowChanged( int row );
	void	paramEdited();
	void	editAccepted();
	void	insertRowInTable( bool );
	void	removeRowInTable( bool );
};

#endif // GROUP_H
