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


class ConfigList : public QListWidget
{
	Q_OBJECT
public:
	ConfigList();
	ConfigList( Group *group );

	Group	*getGroup() const;
	void	setGroup(Group *value);

protected:
	void dragEnterEvent(QDragEnterEvent *event);
	void dragMoveEvent(QDragMoveEvent *event);
	void dragLeaveEvent(QDragLeaveEvent *event);
	void dropEvent(QDropEvent *event);
	QMimeData *mimeData(const QList<QListWidgetItem *> items) const;

private:
	Group	*group;
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

private:
	ConfigList	*config_list;
	QList<QListWidgetItem *>	items;
	QVector<Config *>	configs;
	QVector<Param *>	params;

};

#endif // CONFIGLIST_H
