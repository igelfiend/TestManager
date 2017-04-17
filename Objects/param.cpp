#include <QTextStream>
#include <QRegExp>
#include <QDebug>

#include "param.h"
#include "manager.h"

Param::Param()
{
	owner = nullptr;
}

Param::Param(QString name, QDomNode node, ParamType p_type, StringType str_type):
	Param()
{
	this->name	= name;
	this->node	= node;
	this->type	= p_type;
	this->str_format	= str_type;
}

Param::Param(Param *param):
	Param()
{
	name	= param->name;
	node	= param->node.cloneNode( true );
	type	= param->type;
//	owner	= param->owner;
	str_format	= param->str_format;

}

bool Param::compare(Param *param)
{
	if( !param )
	{
		return false;
	}

	QString src_str = getData().toLower();
	QString comp_str = param->getData().toLower();

	QRegExp del_spaces( "\\s*" );

	src_str.replace( del_spaces, "" );
	comp_str.replace( del_spaces, "" );

	return ( src_str == comp_str );
}

QString Param::getName() const
{
	return name;
}

QString Param::getData() const
{
	if( ( type == ParamType::Simple ) || ( type == ParamType::Table) )
	{
		return node.toElement().text();
	}
	else
	{
		QString str;
		QTextStream s(&str);
		node.save(s, QDomNode::CDATASectionNode);
		return str;
	}
}

ParamType Param::getType() const
{
	return type;
}

Item *Param::getOwner() const
{
	return owner;
}

Config *Param::getConfig() const
{
	return owner->getConfig();
}

void Param::setOwner(Item *value)
{
    owner = value;
}

void Param::setName(const QString &value)
{
	name = value;
}

QDomNode Param::getNode() const
{
	return node;
}

void Param::setNode(const QDomNode &value)
{
	node = value;
}

void Param::setType(const ParamType &value)
{
	type = value;
}

void Param::updateParams(QVector<Param *> params, QString data)
{
	if( params.count() == 0 )
	{
		return;
	}

	params.at( 0 )->getConfig()->getManager()->setChanged( true );
	if( ( params.at( 0 )->getType() == ParamType::Simple ) ||
		( params.at( 0 )->getType() == ParamType::Table  ) )
	{
		for( int i = 0; i < params.count(); ++i )
		{
			if( params.at( i )->getNode().firstChild().isNull() )
			{
				QDomText txt = params.at( i )->getNode().toDocument().createTextNode( data );
				params.at( i )->getNode().appendChild( txt );
			}
			else
			{
				QDomNode txt_node = params.at( i )->getNode().firstChild();

				while( ( !txt_node.isNull() ) && ( txt_node.isComment() ) )
				{
					txt_node = txt_node.nextSibling();
				}

				if( !txt_node.isNull() )
				{
					txt_node.setNodeValue( data );
				}
				else
				{
					QDomText txt = params.at( i )->getNode().toDocument().createTextNode( data );
					params.at( i )->getNode().appendChild( txt );
				}
			}

			if( ( params.at( i )->getOwner() ) && (params.at( i )->getConfig() ) )
			{
				params.at( i )->getConfig()->setChagned( true );
			}
		}
	}
	else
	{
		for( int i = 0; i < params.count(); ++i )
		{
			QDomDocument d;
			QString err_str;
			int err_colmn;
			int err_row;

			d.setContent(data, &err_str, &err_row, &err_colmn);
			if( err_str.length() > 0 )
			{
				qWarning() << err_str << " in line " << err_row << " and col " << err_colmn;
				return;
			}

			QDomNode old_node = params.at( i )->getNode();
			QDomNode new_node = d.documentElement();

			QDomNode child = old_node.firstChild();
			QDomNode next_child;

			while( !child.isNull())
			{
				next_child = child.nextSibling();
				old_node.removeChild( child );
				child = next_child;
			}

			child = new_node.firstChild();

			while( !child.isNull() )
			{
				old_node.appendChild( child.cloneNode( true ) );
				child = child.nextSibling();
			}

			if( ( params.at( i )->getOwner() ) && (params.at( i )->getConfig() ) )
			{
				params.at( i )->getConfig()->setChagned( true );
			}
		}
	}
}

StringType Param::getStrFormat() const
{
	return str_format;
}

void Param::setStrFormat(const StringType &value)
{
	str_format = value;
}

