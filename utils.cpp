#include <QStringList>
#include <QTextStream>
#include <QFile>
#include <QDebug>
#include <QWidget>
#include <QChar>

#include "utils.h"
#include "param.h"
#include "manager.h"

Utils::Utils()
{

}

QDomElement Utils::getXmlElement(const QDomNode &domNode, const QString &path, Qt::CaseSensitivity caseSensitivity)
{
	QStringList elementTagNames = path.split('/');
	QDomNodeList domNodeList = domNode.childNodes();
	for(int i = 0; i < domNodeList.count(); i++)
	{
		if(domNodeList.at(i).toElement().tagName().compare(elementTagNames.first(),caseSensitivity) == 0)
		{
			elementTagNames.takeFirst();
			if(elementTagNames.isEmpty())
				return domNodeList.at(i).toElement();
			else
				return getXmlElement(domNodeList.at(i).toElement(),elementTagNames.join("/"), caseSensitivity);
		}
	}
	return QDomElement();
}

void Utils::printNode(QDomNode node)
{
	QString pStr;
	QTextStream s(&pStr);
	node.save(s, QDomNode::CDATASectionNode);
	QStringList ls = pStr.split("\n");
	for( int j = 0; j < ls.count(); ++j )
	{
		qDebug() << ls[ j ];
	}
}

QVector<QVector<QString> > Utils::parseString(QString str)
{
	QVector<QVector<QString> > result;
	QVector< QString > str_vector;
	QStringList rows;

	StringType str_type = getStringType( str );

	int last;
	int first;

	QRegExp del_spaces( "\\s*" );
	str.replace( del_spaces, "" );

	switch( str_type )
	{
	case BracketsMatrix:
		str.remove(0, 1);
		str.remove(str.length() - 1, 1);

		rows = str.split("],[");

		first	= 0;
		last	= rows.count() - 1;
		rows[ first ] = rows[ first ].remove(QChar('['));
		rows[ last  ] = rows[ last  ].remove(QChar(']'));

		for( int i = 0; i < rows.count(); ++i )
		{
			str_vector = StringToVector( rows.at( i ) );
			result.append( str_vector );
		}
		break;

	case Matrix:
		rows = str.split("],[");

		first	= 0;
		last	= rows.count() - 1;
		rows[ first ] = rows[ first ].remove(QChar('['));
		rows[ last  ] = rows[ last  ].remove(QChar(']'));

		for( int i = 0; i < rows.count(); ++i )
		{
			str_vector = StringToVector( rows.at( i ) );
			result.append( str_vector );
		}
		break;

	case BracketsArray:
		str = str.remove(QRegExp("[\\[\\]]"));

		str_vector = StringToVector( str );
		result.append( str_vector );
		break;

	case Array:
		str_vector = StringToVector( str );
		result.append( str_vector );
		break;
	case Error:
		result.append( str_vector );
		break;
	case None:
		result.append( str_vector );
		break;
	}

	return result;
}

QString Utils::saveToString(QVector<QVector<QString> > matrix, StringType string_type)
{
	if( matrix.count() < 1 )
	{
		return "";
	}

	QString result = "";
	QString tmp;

	switch( string_type )
	{
	case BracketsMatrix:
		result += "\n[";
		for( int i = 0; i < matrix.count(); ++i )
		{
			tmp = "[";
			for( int j = 0; j < matrix.at( i ).count() - 1; ++j )
			{
				tmp += matrix[ i ][ j ] + ", ";
			}

			tmp += matrix[ i ][ matrix.at( i ).count()-1 ] + "]";
			if( i != matrix.count() - 1 )
			{
				tmp += ",\n";
			}
			result += tmp;
		}
		result += "]\n";
		return result;

	case Matrix:
		result += "\n";
		for( int i = 0; i < matrix.count(); ++i )
		{
			tmp = "[";
			for( int j = 0; j < matrix.at( i ).count() - 1; ++j )
			{
				tmp += matrix[ i ][ j ] + ", ";
			}

			tmp += matrix[ i ][ matrix.at( i ).count()-1 ] + "]";
			if( i != matrix.count() - 1 )
			{
				tmp += ",";
			}
			result += tmp;
		}
		result += "\n";
		return result;

	case BracketsArray:
		if( matrix.at( 0 ).count() == 0 )
		{
			return "[ ]";
		}

		result += "[";
		for( int i = 0; i < matrix.at( 0 ).count() - 1; ++i )
		{
			result += matrix[ 0 ][ i ] + ", ";
		}

		result += matrix[ 0 ][ matrix.at( 0 ).count()-1 ] + "]";
		return result;

	case Array:
		if( matrix.at( 0 ).count() == 0 )
		{
			return "";
		}

		for( int i = 0; i < matrix.at( 0 ).count() - 1; ++i )
		{
			result += matrix[ 0 ][ i ] + ", ";
		}

		result += matrix[ 0 ][ matrix.at( 0 ).count()-1 ];
		return result;

	default:
		qDebug() << "Default called";
		return result;
	}
}

QString Utils::saveToString(QVector<QString> matrix, StringType string_type)
{
	QString result = "";
	QString tmp;

	switch( string_type )
	{
	case BracketsArray:
		result += "[";
		for( int i = 0; i < matrix.count() - 1; ++i )
		{
			result += matrix[ i ] + ", ";
		}

		result += matrix[ matrix.count()-1 ] + "]";
		return result;

	case Array:
		for( int i = 0; i < matrix.count() - 1; ++i )
		{
			result += matrix[ i ] + ", ";
		}

		result += matrix[ matrix.count()-1 ];
		return result;

	default:
		return result;
	}
}

StringType Utils::getStringType(QString str)
{
	StringType str_type = StringType::Error;

	QRegExp del_spaces( "\\s*" );
	str.replace( del_spaces, "" );

	if( str.length() > 1 )
	{
		if( ( str[ 0 ] == '[' ) &&
			( str[ 1 ] == '[' ) &&
			( str[ str.length()-1 ] == ']' ) &&
			( str[ str.length()-2 ] == ']' ) )
		{
			str_type = StringType::BracketsMatrix;
		}
		else if ( ( str[ 0 ] == '[' ) &&
				  ( str[ str.length()-1 ] == ']' ) )
		{
			str_type = StringType::BracketsArray;

			int bracket_index = str.indexOf( ']' );
			if( bracket_index != ( str.length()-1) )
			{
				str_type = StringType::Matrix;
			}
		}
		else
		{
			str_type = StringType::Array;
		}
	}


	return str_type;
}

QString Utils::spacesToTabs(QString str)
{
	for( int i = 0; i < str.length(); ++i )
	{
		if( str.at( i ) == '\n' )
		{
			int j = i + 1;
			while( ( j < str.length() ) && ( str.at( j ) == ' ' ) )
			{
				str[ j++ ] = '\t';
			}
			i = j;
		}
	}
	return str;
}

void Utils::clearLayout(QLayout *layout)
{
	if( layout->count() == 0 )
	{
		return;
	}

	while( QLayoutItem *item = layout->takeAt( 0 ) )
	{
		if( item->widget() )
		{
			delete item->widget();
		}
		if( item->layout() )
		{
			clearLayout( item->layout() );
		}
		delete item;
	}
}

void Utils::fixAccuracyTestRange(const QString &test_name, Manager *manager)
{
	qDebug() << "fixAccuracyTestRange::started()";
	qDebug() << "fixAccuracyTestRange::test name: " << test_name;
	int conf_count = manager->getConfigsCount();
	for( int i = 0; i < conf_count; ++i )
	{
		Config	*conf		= manager->getConfig( i );
		Test	*conf_test	= conf->getItem( test_name )->toTest();
		if( !conf_test )
		{
			qDebug() << QString("fixAccuracyTestRange::in config %1 no item %2").arg( conf->getFullName() ).arg( test_name );
			continue;
		}

		double	max_freq	= conf_test->getParam( "max_freq" )->getData().toInt();
		Param	*param		= conf_test->getParam( "measurement bands" );

		if( !param )
		{
			qDebug() << QString("fixAccuracyTestRange::in config %1 no param measurement").arg( conf->getFullName() );
			continue;
		}

		QDomNode node = param->getNode();

		int prevFreq = 0;
		for( int j = 0; j < node.childNodes().count(); ++j )
		{
			QDomNode band = node.childNodes().at( j );

			QDomElement mhz_node = band.toElement().firstChildElement( "MHz" );
			QDomElement count_points_node = band.toElement().firstChildElement( "count_points" );
			QVector<QVector <QString> > mhz = Utils::parseString( mhz_node.text() );


			double endFreq = 0;
			if( mhz.at(0).at(1).contains( "MAX" ) )
			{
				endFreq = max_freq;
			}
			else
			{
				endFreq = mhz.at(0).at(1).toDouble();
			}

			if( j > 0 )
			{
				double currFreq = mhz.at(0).at(0).toDouble();
				if( abs( currFreq - prevFreq ) < 0.0001 )
				{
					int count_points = count_points_node.text().toInt();
					double step = ( endFreq - currFreq ) / ( count_points - 1 );
					double new_start_freq = currFreq + step;

					// Updating range data
					QDomNode old_node = mhz_node.childNodes().at( 0 );
					mhz_node.removeChild( old_node );
					QDomText txt_node = mhz_node.toDocument().createTextNode( QString( "%1, %2" )
																			 .arg( new_start_freq )
																			 .arg( mhz.at(0).at(1) ) );
					mhz_node.appendChild( txt_node );


					// Updating count_points data
					old_node = count_points_node.childNodes().at( 0 );
					count_points_node.removeChild( old_node );
					txt_node = mhz_node.toDocument().createTextNode( QString::number( count_points - 1 ) );
					count_points_node.appendChild( txt_node );
				}

				conf->setChagned( true );
				manager->setChanged( true );
			}

			prevFreq = endFreq;
		}
		qDebug() << QString("fixAccuracyTestRange::in config %1 fixed").arg( conf->getFullName() );
	}
}

QVector<QString> Utils::StringToVector(QString str)
{
	QStringList tmp = str.split(",");
	QVector< QString > str_vector;

	for( int i = 0; i < tmp.count(); ++i )
	{
		str_vector.append( tmp.at( i ) );
	}

	return str_vector;
}

