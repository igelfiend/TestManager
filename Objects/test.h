#ifndef TEST_H
#define TEST_H

#include <QList>
#include <QDomElement>

class QString;
class Param;
class Config;
class Main;
class Test;

class Item
{
public:
	Item(QString name, Config * config, QDomNode root);
	virtual ~Item();

	virtual	void init();
	void	addParam(Param * param);

	Param	*getParam(int index) const;
	Param	*getParam(QString param_name) const;
	virtual QString	getName() const;
	Config	*getConfig()const;
	Test	*toTest();
	Main	*toMain();

	bool removeParam(int index);
	bool removeParam(QString param_name);

	const QDomNode getRoot() const;

	void ShowAllParams();


protected:
	QString	name;
	Config	*config;
	QDomNode	root;
	QList<Param *> params;
};

class Main: public Item
{
public:
	Main(QString name, Config * config, QDomNode root);
	~Main();

//	void init();
private:
};


class Test: public Item
{
public:
	Test(QString name, Config * config, QDomNode root);
	virtual ~Test();

//	void init();
	void setTemplateDir(QString path);
	void setInstructionPath(QString path);

private:
	QString name;
	QString version;
	QString template_dir;
	QString instruction_path;
};


#endif // TEST_H
