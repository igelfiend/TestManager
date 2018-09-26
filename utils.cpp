#include <QStringList>
#include <QTextStream>
#include <QFile>
#include <QDebug>
#include <QWidget>
#include <QChar>
#include <QDomElement>
#include <QGroupBox>
#include <QTextEdit>
#include <QFormLayout>
#include <QLabel>
#include <QMessageBox>
#include <QProgressDialog>
#include <QDir>
#include <QApplication>

#include "utils.h"
#include "param.h"
#include "manager.h"
#include "spoilergroupbox.h"


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

void Utils::printNode(const QDomNode &node)
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

QString Utils::saveToString(const QVector<QVector<QString> > &matrix, StringType string_type)
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

QString Utils::saveToString(const QVector<QString> &matrix, StringType string_type)
{
	QString result = "";

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

QDomText Utils::TextInside(const QDomNode &node)
{
	QDomNodeList nodes = node.childNodes();
	for( int i = 0; i < nodes.count(); ++i )
	{
		if( nodes.at( i ).isText() )
		{
			return nodes.at( i ).toText();
		}
	}

	return QDomText();
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
                if( fabs( currFreq - prevFreq ) < 0.0001 )
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

void Utils::fixEquipOrderInPeriodic(Manager *manager)
{
    for( int i = 0; i < manager->getConfigsCount(); ++i )
    {
        Config *conf = manager->getConfig( i );
        QDomNode main_root = conf->getMain()->getRoot();
        QDomElement select_method = main_root.firstChildElement( "select_method" );
        if( !select_method.isNull() )
        {
            QDomNode method = select_method.firstChild();
            while( !method.isNull() )
            {
                QDomElement tests = method.firstChildElement( "tests" );

                QString tests_str = tests.text();
                QStringList tests_list = tests_str.split( "," );

                int equip_pos = tests_list.indexOf( "Equipment" );
                if( equip_pos != -1 )
                {
                    tests_list.move( equip_pos, 1 );
                }
                tests.firstChild().setNodeValue( tests_list.join( "," ) );

                method = method.nextSibling();
            }
        }
        conf->setChanged( true );
        manager->setChanged( true );
    }
}

void Utils::fixPowerAccuracyNaming(Manager *manager)
{
    int config_count = manager->getConfigsCount();
    for( int i = 0; i < config_count; ++i )
    {
        Config *conf    = manager->getConfig( i );
        Item *mainTest  = conf->getMain();
        Param *method_param = mainTest->getParam( "select_method" );
        if( method_param == nullptr )
        {
            continue;
        }

        QDomNode node = method_param->getNode();

        QDomElement method = node.firstChildElement( "method" );
        while( !method.isNull() )
        {
            QDomElement tests_node = method.firstChildElement( "tests" );
            QString tests = tests_node.text();
            if( tests.contains( "PowerAccuracy,", Qt::CaseInsensitive ) )
            {
                tests = tests.replace( "PowerAccuracy,", "PowerAccuracyV2," );
                qDebug() << "Replaced: " << QString( "%1 : PowerAccuracy -> PowerAccuracyV2" )
                                            .arg( conf->getFullName() );

                tests_node.firstChild().toText().setData( tests );
                manager->setChanged( true );
                conf->setChanged( true );
            }

            method = method.nextSiblingElement( "method" );
        }
    }
}

void Utils::addTagToMain(Manager *manager, const QString &tag_name, const QString &paste_after)
{
    int config_count = manager->getConfigsCount();
    bool fChanged = false;
    bool fInsertLast = paste_after.isEmpty();
    for( int i = 0; i < config_count; ++i )
    {
        Item *mainElement = manager->getConfig( i )->getMain();

        if( !mainElement->getParam( tag_name ) )
        {
            QDomNode mainRoot = mainElement->getRoot();

            if( !mainRoot.firstChildElement( tag_name ).isNull() )
            {
                qDebug() << "Utils::addTagToMain: tag present in Main block";
                continue;
            }

            QDomDocument d;
            QDomElement newNode = d.createElement( tag_name );

            if( fInsertLast )
            {
                mainRoot.appendChild( newNode );
            }
            else
            {
                mainRoot.insertAfter( newNode, mainRoot.firstChildElement( paste_after ) );
            }

            manager->getConfig( i )->setChanged( true );
            fChanged = true;
        }
    }
    manager->setChanged( fChanged );
}

void Utils::addTestToConfigs( const QString &test_code, bool fInsertFirst, const QString &prevTestName, Manager *manager )
{
    bool fChanged = false;
    QString errorStr;
    int errorLine;
    int errorColumn;
    QDomDocument test_dom;
    if( !test_dom.setContent( test_code, true, &errorStr, &errorLine, &errorColumn ) )
    {

        QMessageBox mbox;
        mbox.setText( QString( " XML error:\n Error line: %1"
                               "\n Error column: %2"
                               "\n Error str: %3").arg( errorLine   )
                                                  .arg( errorColumn )
                                                  .arg( errorStr    ) );
        mbox.exec();
        return;
    }

    int configs_count = manager->getConfigsCount();
    for( int i = 0; i < configs_count; ++i )
    {
        Config *config = manager->getConfig( i );
        qDebug() << "current config: " << config->getFullName();

        QDomNode prevNode;

        if( fInsertFirst )
        {
            Item *mainElement = config->getMain();
            if( !mainElement )
            {
                qDebug() << "Main not found";
                continue;
            }

            prevNode = mainElement->getRoot();
        }
        else
        {
            Test *prevTest = config->getTest( prevTestName );
            if( !prevTest )
            {
                qDebug() << "Test not found";
                continue;
            }
            prevNode = prevTest->getRoot();
        }

        QDomNode parentNode = prevNode.parentNode();
        parentNode.insertAfter( test_dom.cloneNode( true ), prevNode );
        config->setChanged( true );
        fChanged = true;
    }

    manager->setChanged( fChanged );
    qDebug() << "Insertion done";
}

bool Utils::NodeIsFieldType(const QDomNode &node)
{
	qDebug() << "checking node " << node.toElement().tagName();
	bool fIsField = true;

	QDomNodeList children = node.childNodes();
	for( int i = 0; i < children.count(); ++i )
	{
		if( !children.at( i ).isComment() && !children.at( i ).isText() )
		{
			qDebug() << "at " << i << " not comment and not text";
			fIsField = false;
			break;
		}
	}

	return fIsField;
}

SpoilerGroupBox *Utils::NodeToGroupBox( const QDomNode &node, FIELDS &fields )
{
	QString title	= node.toElement().tagName();
	SpoilerGroupBox *gbox = new SpoilerGroupBox( title );
//	gbox->setCheckable( true );
//	gbox->setStyleSheet( "QGroupBox "
//						 "{"
//							"border: 1px solid gray;"
//							"border-radius: 3px;"
//							"padding: 10px;	"
//						 "}" );
	QFormLayout *gbox_layout = new QFormLayout();
	gbox->setLayout( gbox_layout );

	QDomNodeList children = node.childNodes();
	for( int i = 0; i < children.count(); ++i )
	{
        if( children.at( i ).isComment() ) continue;

		if( NodeIsFieldType( children.at( i ) ) )
		{
			qDebug() << "data in text node: " << TextInside( children.at( i ) ).data();
			QTextEdit *edit	= new QTextEdit( TextInside( children.at( i ) ).data(), gbox );
			edit->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
			edit->setMaximumHeight( 25 );
			edit->setMinimumHeight( 25 );

			gbox_layout->addRow( children.at( i ).toElement().tagName(), edit );

			QPair< QDomNode, QTextEdit* > field;
			field.first		= children.at( i );
			field.second	= edit;

			fields << field;
		}
		else
		{
			gbox_layout->addRow( NodeToGroupBox( children.at( i ), fields ) );
		}
	}

    return gbox;
}

bool Utils::ReplaceInFile(const QString &src_filename, const QString &tgt_filename, const QMap<QString, QString> replace_map)
{
    QFile svg_file( src_filename );
    if( !svg_file.open( QIODevice::ReadOnly | QIODevice::Text ) )
    {
        qCritical() << "Utils::ReplaceInFile: Error opening file on read " << src_filename;
        return false;
    }

    QTextStream in( &svg_file );
    QString svg_code = in.readAll();
    svg_file.close();

    qDebug() << "Processing " << src_filename;
    for( QString key: replace_map.keys() )
    {
        svg_code = svg_code.replace( key, replace_map.value( key ) );
        qDebug() << "\tReplaced " << key << " with " << replace_map.value( key );
    }

    qDebug() << "Output file " << tgt_filename;
    QFile svg_file_out( tgt_filename );
    if( !svg_file_out.open( QIODevice::WriteOnly | QIODevice::Text ) )
    {
        qCritical() << "Utils::ReplaceInFile: Error opening file on write " << tgt_filename;
        return false;
    }

    QTextStream out( &svg_file_out );
    out << svg_code;
    svg_file_out.close();

    return true;
}

bool Utils::ReplaceColorInSvgs(const QString &dev_dir_path, const QStringList &devices, const QMap<QString, QString> &replace_map, bool fReplaceOrigin, QWidget *parent)
{
    qDebug() << "Utils::ReplaceColorInSvgs started";
    qDebug() << "Utils::ReplaceColorInSvgs dir path: " << dev_dir_path;

    QString msg( "Processing Svg..." );
    QProgressDialog progress( msg, "Cancel", 0, devices.count()-1, parent );
    progress.setWindowModality( Qt::WindowModal );
    progress.setMinimumDuration( 0 );
    progress.setMinimumSize( 400, 100 );
    QApplication::processEvents();

    for( int i = 0; i < devices.count(); ++i )
    {
        progress.setValue( i );
        progress.setLabelText( msg + "\n current device: " + devices[ i ] );
        if( progress.wasCanceled() )
        {
            break;
        }

        QString img_dir_path = QString( "%1/%2/images" )
                               .arg( dev_dir_path )
                               .arg( devices[ i ] );

        QString img_inverse_dir_path;

        if( fReplaceOrigin )
        {
            img_inverse_dir_path = img_dir_path;
        }
        else
        {
            img_inverse_dir_path = QString( "%1/%2/images_inverse" )
                                                   .arg( dev_dir_path )
                                                   .arg( devices[ i ] );
        }

        QDir img_dir( img_dir_path );
        QDir img_inverse_dir( img_inverse_dir_path );
        if( !img_inverse_dir.exists() )
        {
            img_inverse_dir.mkpath( "." );
        }

        QStringList files = img_dir.entryList( QStringList() << "*.svg", QDir::Files );

        qDebug() << "files: " << files;
        foreach( QString filename, files )
        {
            QApplication::processEvents();
            QString src_filename = QString( "%1/%2" ).arg( img_dir_path ).arg( filename );
            QString tgt_filename = QString( "%1/%2" ).arg( img_inverse_dir_path ).arg( filename );
            if( !ReplaceInFile( src_filename, tgt_filename, replace_map ) )
            {
                return false;
            }
        }
    }
    progress.setValue( devices.count()-1 );

    return true;
}

bool Utils::FixSvgStyle(const QString &pics_foulder, QPoint pic_size, const QString &style, const QString &css_string, QWidget *parent)
{
    QString dev_dir_path = QString( "../../release/devices" );

    QDir path( dev_dir_path );
    QStringList devices = path.entryList( QDir::Dirs );

    if( devices.count() < 3 )
    {
        return false;
    }

    QProgressDialog progress( "Processing Svg...", "Cancel", 2, devices.count()-1, parent );
    progress.setWindowModality( Qt::WindowModal );
    progress.setMinimumDuration( 0 );
    progress.setMinimumSize( 400, 100 );
    QApplication::processEvents();

    for( int i = 2; i < devices.count(); ++i )
    {
        progress.setValue( i );
        if( progress.wasCanceled() )
        {
            break;
        }

        QString img_dir_path = QString( "%1/%2/%3" )
                               .arg( dev_dir_path )
                               .arg( devices[ i ] )
                               .arg( pics_foulder );

        QDir img_dir( img_dir_path );

        QStringList files = img_dir.entryList( QStringList() << "Adapter_Frequency_accuracy_*_port_*.svg", QDir::Files );

        qDebug() << "device: " << devices[ i ];
        qDebug() << "\tfiles: " << files;
        foreach( QString filename, files )
        {
            QApplication::processEvents();
            QString svg_filename = QString( "%1/%2" ).arg( img_dir_path ).arg( filename );

            FixSvgStyleInFile( svg_filename, pic_size, style, css_string );
        }
    }
    progress.setValue( devices.count()-1 );

    return true;

}

bool Utils::FixSvgStyleInFile(const QString &filepath, QPoint pic_size, const QString &style, const QString &css_string)
{
    QString tempfile_path = filepath + ".temp";

    QFile file_in ( filepath );
    QFile file_out( tempfile_path );

    if( !file_in.open( QIODevice::ReadOnly | QIODevice::Text ) )
    {
        qWarning() << "Utils::FixSvgStyle: can't find file " << filepath;
        return false;
    }

    if( !file_out.open( QIODevice::WriteOnly | QIODevice::Text ) )
    {
        qWarning() << "Utils::FixSvgStyle: can't find file " << tempfile_path;
        return false;
    }

    QTextStream in ( &file_in  );
    QTextStream out( &file_out );

    while( !in.atEnd() )
    {
        QString row = in.readLine();

        QString new_row = row;
        if( processRowInSvg( new_row, pic_size, style, css_string ) )
        {
//            qInfo() << "replace:\n " << row << "\nwith\n" << new_row;
            out << new_row << "\n";
        }
        else
        {
            out << row << "\n";
        }
    }

    file_in .close();
    file_out.close();

    file_in.remove();
    file_out.rename( tempfile_path, filepath );

    return true;
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

void Utils::removeChilds(QDomNode node)
{
	QDomNodeList nodes = node.childNodes();
	while(!nodes.isEmpty())
	{
	   QDomNode child = nodes.at(0);
	   node.removeChild( child );
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

QVector<QString> Utils::StringToVector(const QString &str)
{
	QStringList tmp = str.split(",");
	QVector< QString > str_vector;

	for( int i = 0; i < tmp.count(); ++i )
	{
		str_vector.append( tmp.at( i ) );
	}

    return str_vector;
}

QPoint Utils::getWidthAndHeightFromPointsParameterInSvg(const QString &points_str)
{
    QStringList groups = points_str.split( " ", QString::SkipEmptyParts );

    if( groups.count() < 4 )
    {
        return QPoint();
    }

    QStringList gr = groups[ 0 ].split( "," );
    QPoint x1( gr[ 0 ].toInt(), gr[ 1 ].toInt() );

    gr = groups[ 1 ].split( "," );
    QPoint x2( gr[ 0 ].toInt(), gr[ 1 ].toInt() );

    gr = groups[ 2 ].split( "," );
    QPoint x3( gr[ 0 ].toInt(), gr[ 1 ].toInt() );


    QPoint result;

    result.setX( (int)sqrt( std::pow( x1.x() - x2.x(), 2 ) + std::pow( x1.y() - x2.y(), 2 ) ) );
    result.setY( (int)sqrt( std::pow( x2.x() - x3.x(), 2 ) + std::pow( x2.y() - x3.y(), 2 ) ) );

    return result;
}

bool Utils::processRowInSvg(QString &row, QPoint pic_size, const QString &stylename, const QString &css_style)
{
    QStringList splitted_row = row.split( "\"" );
    QString row_start = splitted_row.first();
    row_start = row_start.trimmed().left( 8 );

    if( row.contains( "]]>", Qt::CaseSensitive ) )
    {
        row = "    " + css_style + "\n" + row;
        return true;
    }

    if( row_start != "<polygon" )
    {
        return false;
    }

    if( splitted_row.count() < 4 )
    {
        qCritical() << "Utils::processRowInSvg: count parameters error!";
        return false;
    }

    QString coords_block = splitted_row[ 3 ];

    QPoint poly_size = getWidthAndHeightFromPointsParameterInSvg( coords_block );

    qDebug() << "\t\tcompare: " << pic_size << poly_size;
    if( ( abs( pic_size.x() - poly_size.x() ) > 2 ) ||
        ( abs( pic_size.y() - poly_size.y() ) > 2 ) )
    {
        return false;
    }


    QString style_block = splitted_row[ 1 ];
    style_block = stylename + " " + style_block.split( " " )[ 1 ];
    splitted_row[ 1 ] = style_block;
    row = splitted_row.join( "\"" );

    return true;
}

