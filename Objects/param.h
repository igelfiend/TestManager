#ifndef PARAM_H
#define PARAM_H

#include <QString>
#include <QDomElement>

#include <test_info.h>
#include <test.h>
#include "config.h"
#include "utils.h"

class QDomNode;

class Param
{
public:
    Param();
    Param( const QString &name, const QDomNode &node, ParamType p_type, StringType str_type = StringType::None );
	Param( Param *param );

	bool	compare( Param * param );

	void	setName(const QString &value);
	void	setType(const ParamType &value);
	void	setOwner(Item *value);
	void	setNode(const QDomNode &value);
	void	setStrFormat(const StringType &value);

	QString getName() const;
	QString getData() const;
	Item	*getOwner()     const;
	Config  *getConfig()    const;

	StringType	getStrFormat() const;
	ParamType	getType() const;
	QDomNode	getNode() const;

    static void updateParams(QVector<Param *> params, const QString &data);



private:
	QString	name;
	Item	*owner;
	QDomNode	node;
	ParamType	type;
	StringType	str_format;

};

#endif // PARAM_H
