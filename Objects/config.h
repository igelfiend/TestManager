#ifndef CONFIG_H
#define CONFIG_H

#include <QList>
#include <QDomElement>

class QString;

class Test;
class Item;
class Manager;

class Config
{
public:
	Config(QString device, QString type, QDomDocument document);
	~Config();

	void	init();
	void	setMain(Item * main_block);
	void	addTest(Test * test);

	Item	*getMain() const;
	Test	*getTest(int index) const;
	Test	*getTest(QString test_name) const;

	bool	removeTest(int index);
	bool	removeTest(QString test_name);

	int	getTestCount()	const;
	int	getItemsCount()	const;

	Item	*getItem(int index) const;
	Item	*getItem(const QString &name, const QString &version, const QString &key = QString() ) const;
	QVector< Item * > getItems( const QString &name, const QString &version ) const;

	QString getFullName() const;
	void	printParams();
	QDomElement		getRoot() const;
	QDomDocument	getRootDocument() const;

	Manager *getManager() const;
	void	setManager(Manager *value);

	QString getPath() const;
	void	setPath(const QString &value);

	bool	isChanged() const;
	void	setChanged( bool flag );

private:
	QString	device;
	QString type;
	bool	changed;
	QString	path;
	Manager	* manager;
	QDomDocument	root;
	QList<Item *>	items;
};

#endif // CONFIG_H
