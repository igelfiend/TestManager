#include <QStringList>
#include <QTextStream>
#include <QFile>
#include <QDebug>
#include <QWidget>
#include <QChar>
#include <QDomElement>

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

void Utils::fixAccuracyTestRange(const QString &test_name, const QString &version, Manager *manager)
{
	qDebug() << "fixAccuracyTestRange::started()";
	qDebug() << "fixAccuracyTestRange::test name: " << test_name;
	int conf_count = manager->getConfigsCount();
	for( int i = 0; i < conf_count; ++i )
	{
		Config	*conf		= manager->getConfig( i );
		Test	*conf_test	= conf->getItem( test_name, version )->toTest();
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

				conf->setChanged( true );
				manager->setChanged( true );
			}

			prevFreq = endFreq;
		}
		qDebug() << QString("fixAccuracyTestRange::in config %1 fixed").arg( conf->getFullName() );
	}
}

void Utils::addEquipToPerformance( Manager *manager)
{
	qDebug() << "addEquipToPerformance::started()";
	QStringList N_devices;
	N_devices		<< "R54"		<< "R60"		<< "R140"		<< "R160"
					<< "S5048"		<< "S5065"		<< "S5085"		<< "TR1300"
					<< "TR5048"		<< "Obzor304"	<< "Obzor804"	<< "Obzor808"
					<< "Obzor814"	<< "C1205"		<< "C1207"		<< "C1209_01"
					<< "C1209_02"	<< "C1214"		<< "C1409"		<< "C2209"
					<< "C2409"		<< "C4209"		<< "C4409";

	QStringList mm35_devices;
	mm35_devices	<< "RP180_01"	<< "RP180_02"	<<  "RP180_03"	<< "C1420"
					<< "RP5"		<< "RP60"		<< "C1220"		<< "C2220"
					<< "C2420"		<< "C4220"		<< "C4420";

	QStringList N75_devices;
	N75_devices << "S7530" << "TR7530";

	QStringList N_and_35mm_devices;
	N_and_35mm_devices << "R180";

	for( int i = 0; i < manager->getConfigsCount(); ++i )
	{
		QString dev_name = manager->getConfig( i )->getMain()->getParam( "device_info/device" )->getData();
		QStringList dev_list;

		Config	*conf		= manager->getConfig( i );
		qDebug() << "addEquipToPerformance::processed configuration: " << conf->getFullName();

		for( int j = 0; j < conf->getItemsCount(); ++j )
		{
			qDebug() << "addEquipToPerformance::dev list: " << dev_list;
			Test	*test	= conf->getItem( j )->toTest();
			if( !test ) continue;

			QDomElement config_node = test->getRoot().firstChildElement("config");
			qDebug() << "addEquipToPerformance::processed test: " << test->getName();
//			qDebug() << "addEquipToPerformance::with params: ";
//			test->ShowAllParams();

			//  -------------------------- Frequency Accuracy ------------------------
			if( test->getName().contains( "FrequencyAccuracy", Qt::CaseInsensitive ) )
			{
				insertNodeAndAddToList( config_node, dev_list, "key_dev_list", "freqCounter,spectrumAn" );
			}
			// --------------------------- Power Accuracy ----------------------------
			else if( test->getName().contains( "PowerAccuracy", Qt::CaseInsensitive ) )
			{
				if( mm35_devices.indexOf( dev_name ) != -1 )
				{
					insertNodeAndAddToList( config_node, dev_list, "key_dev_list", "powerSensor_Z52" );
				}
				else
				{
					insertNodeAndAddToList( config_node, dev_list, "key_dev_list", "powerSensor_Z51" );
				}
			}
			// ---------- Harmonic Distortion & NonHarmonicSpurious -------------------
			else if( test->getName().contains( "HarmonicDistortion",  Qt::CaseInsensitive ) ||
					 test->getName().contains( "NonHarmonicSpurious", Qt::CaseInsensitive ) )
			{
				insertNodeAndAddToList( config_node, dev_list, "key_dev_list", "spectrumAn" );
			}
			// ----- Transmission coefficient magnitude and phase accuracy test -------
			else if( test->getName().contains( "TransCoeffMagPhAcc", Qt::CaseInsensitive ) )
			{
				if( dev_name == "Obzor304" )
				{
					insertNodeAndAddToList( config_node, dev_list, "key_dev_list", "att20_typeN,att40_typeN,att60_typeN" );
				}
				else if( N75_devices.indexOf( dev_name ) != -1 )
				{
					insertNodeAndAddToList( config_node, dev_list, "key_dev_list", "att20_typeN75,att40_typeN75" );
				}
				else if( mm35_devices.indexOf( dev_name ) != -1 )
				{
					insertNodeAndAddToList( config_node, dev_list, "key_dev_list", "att20_35mm,att40_35mm" );
				}
				else
				{
					insertNodeAndAddToList( config_node, dev_list, "key_dev_list", "att20_typeN,att40_typeN" );
				}
			}
			// ----- Reflection Coefficient Magnitude and Phase Accuracy Test ---------
			else if( test->getName().contains( "ReflCoeffMagPhAcc", Qt::CaseInsensitive ) )
			{
				if( N75_devices.indexOf( dev_name ) != -1 )
				{
					insertNodeAndAddToList( config_node, dev_list, "key_dev_list", "airline25ohm_typeN75" );
				}
				else if( mm35_devices.indexOf( dev_name ) != -1 )
				{
					insertNodeAndAddToList( config_node, dev_list, "key_dev_list", "airline25ohm_35mm" );
				}
				else
				{
					insertNodeAndAddToList( config_node, dev_list, "key_dev_list", "airline25ohm_typeN" );
				}
			}
			// ---------------------------- Accuracy Measurement -----------------------
			else if( test->getName().contains( "AccuracyMeasurement", Qt::CaseInsensitive ) )
			{
				if( N75_devices.indexOf( dev_name ) != -1 )
				{
					insertNodeAndAddToList( config_node, dev_list, "key_dev_list", "calKit_typeN75" );
				}
				else if( mm35_devices.indexOf( dev_name ) != -1 )
				{
					insertNodeAndAddToList( config_node, dev_list, "key_dev_list", "calKit_35mm" );
				}
				else
				{
					insertNodeAndAddToList( config_node, dev_list, "key_dev_list", "calKit_typeN" );
				}
			}
			// -------------------- Accuracy Test High and Low Reflection --------------
			else if( test->getName().contains( "HighLowReflection", Qt::CaseInsensitive ) )
			{
				if( N_and_35mm_devices.indexOf( dev_name ) != -1 )
				{
					insertNodeAndAddToList( config_node, dev_list, "key_dev_list", "att40_typeN,att40_35mm,"
																				   "short_typeN_male,short_typeN_female,"
																				   "short_35mm_female,short_35mm_male" );
				}
				else if( mm35_devices.indexOf( dev_name ) != -1 )
				{
					insertNodeAndAddToList( config_node, dev_list, "key_dev_list", "att40_35mm,short_35mm_female" );
				}
				else
				{
					insertNodeAndAddToList( config_node, dev_list, "key_dev_list", "att40_typeN,short_typeN_male,short_typeN_female" );
				}
			}
		}

		// ---------------- Insert Equip Item in prf file -------------------------------
		qDebug() << "addEquipToPerformance::Inserting Equip block()";
		for( int j = 0; j < conf->getItemsCount(); ++j )
		{
			Item *test	= conf->getItem( j );
			qDebug() << "addEquipToPerformance::\tChecked test: " << test->getName();

			if( test->getName() == "MainTable" )
			{
				qDebug() << "addEquipToPerformance::\t\tMain part found:   " << test->getName();
				QDomNode prf_node = test->getRoot().parentNode();
				qDebug() << "addEquipToPerformance::\t\tParent node found: " << prf_node.toElement().tagName();
				QDomDocument d = prf_node.ownerDocument();
				prf_node.insertAfter( createEquipItem( dev_list, d ), test->getRoot() );
				break;
			}
		}

		// ---------------- Add Equipment if there is some select_methods ----------------
		QDomNode main_root = conf->getMain()->getRoot();
		QDomElement select_method = main_root.firstChildElement( "select_method" );
		if( !select_method.isNull() )
		{
			QDomNode method = select_method.firstChild();
			while( !method.isNull() )
			{
				QDomElement tests = method.firstChildElement( "tests" );
				tests.firstChild().setNodeValue( tests.text() + ",Equipment" );
				method = method.nextSibling();
			}
		}
		conf->setChanged( true );
		manager->setChanged( true );
	}
	qDebug() << "addEquipToPerformance::finished()";

}

void Utils::insertNodeAndAddToList(QDomNode &target, QStringList &target_list, const QString &node_name, const QString &data)
{
	insertNode( target, node_name, data );
	addElementsToList( target_list, data );
}

void Utils::insertNode(QDomNode &target, const QString &node_name, const QString &data)
{

	QDomElement new_node = target.ownerDocument().createElement( node_name );

	new_node.appendChild( target.ownerDocument().createTextNode( data ) );
	if( target.childNodes().count() > 0 )
	{
		QDomNode first_node = target.firstChild();
		target.insertBefore( new_node, first_node );
	}
	else
	{
		target.appendChild( new_node );
	}
}

QDomNode Utils::createEquipItem(const QStringList &dev_list, QDomDocument &doc)
{
	QDomElement equip = doc.createElement( "item" );
	equip.setAttribute( "name_template",	"Equipment" );
	equip.setAttribute( "version_template", "1.0" );
	equip.setAttribute( "idd", "Equipment" );

	insertNode( equip, "dir", "templates/Equipment/Equipment_version_1" );

	QDomElement config = doc.createElement( "config" );
	equip.appendChild( config );

	insertNode( config, "key_dev_list", dev_list.join( ',' ) );

	equip.appendChild( doc.createElement( "initialization" ) );
	qDebug() << "Utils::createEquipItem::item created with: " << dev_list;
	return equip;
}

void Utils::addElementsToList(QStringList &target_list, const QString &data)
{
	QStringList data_list = data.split(",");
	foreach( QString str, data_list )
	{
		str = str.trimmed();
		if( target_list.indexOf( str ) == -1 )
		{
			target_list << str;
		}
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

