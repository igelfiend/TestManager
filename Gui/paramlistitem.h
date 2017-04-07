#ifndef PARAMLISTITEM_H
#define PARAMLISTITEM_H

#include <QListWidgetItem>
#include <QListWidget>
#include <QMetaType>
#include <QLineEdit>

class Param;
class Item;
class Config;

class ParamListItem : public QListWidgetItem
{
public:
	ParamListItem();
	ParamListItem(QString text, QListWidget *parent = 0, Param	*param	= nullptr);
	ParamListItem(QString text, QListWidget *parent = 0, Item	*item	= nullptr);
	ParamListItem(QString text, QListWidget *parent = 0, Config	*config	= nullptr);
	~ParamListItem();

	void	setParam(Param *value);

	Param	*getParam()		const;
	Item	*getItem()		const;
	Config	*getConfig()	const;
	void	ShowInfo()		const;


protected:
	Param	*param;
	Config	*config;
	Item	*item;
};


#endif // PARAMLISTITEM_H
