#ifndef CONFIGLIST_H
#define CONFIGLIST_H

#include <QListWidget>
#include <QVector>
#include "config.h"
#include "param.h"
#include "group.h"
#include <QMimeData>

class QMimeData;
class ConfigMime;
class ParamListItem;


class ConfigList : public QListWidget
{
	Q_OBJECT
public:
	ConfigList(QWidget *parent);
	ConfigList(Group *group , QWidget *parent = nullptr);	// Конструктор на основе группы

	Group	*getGroup() const;
	void	setGroup(Group *value);

	QVector<Param *> getParams() const;		// Получить параметры выделенных элементов

	QWidget *getParent() const;				// Получить родителя

	bool isAddingGroup() const;

public slots:
	void	ShowContextMenu(QPoint pos);
	void	ShowItemInfo();
	void	CreateNewGroup();

protected:
	void	dragEnterEvent(QDragEnterEvent *event);
	void	dragMoveEvent(QDragMoveEvent *event);
	void	dragLeaveEvent(QDragLeaveEvent *event);
	void	dropEvent(QDropEvent *event);
	QMimeData	*mimeData(const QList<QListWidgetItem *> items) const;

private:
	Group	*group;
	QWidget	*parent;
	bool	fAddingGroup;
};


class ConfigMime : public QMimeData
{
	Q_OBJECT

public:
	ConfigMime( ConfigList * src, const QList<QListWidgetItem *> items );
	ConfigList	*getConfigList() const;

	QVector<Config *>	getConfigs() const;
	QVector<Param *>	getParams() const;

	bool	hasConfigs()	const;
	bool	hasParams()		const;
	bool	hasTests()		const;

private:
	ConfigList	*config_list;
	QList<QListWidgetItem *>	items;
	QVector<Config *>	configs;
	QVector<Item *>		tests;
	QVector<Param *>	params;

};

#endif // CONFIGLIST_H
