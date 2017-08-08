#include "comparator.h"
#include "config.h"
#include "group.h"
#include "param.h"
#include "test.h"
#include "utils.h"
#include "mainwindow.h"
#include "test_info.h"

#include <QDockWidget>
#include <QDebug>

Comparator::Comparator()
{

}

Comparator::Comparator(Manager *manager, QVBoxLayout *outLayout)
{
	this->manager	= manager;
	this->outLayout	= outLayout;

	blocks.append( new CompareBlock( this ) );
	blocks.append( new CompareBlock( this ) );

}

Comparator::~Comparator()
{

}

void Comparator::update()
{
	for( int i = 0; i < blocks.count(); ++i )
	{
		blocks.at( i )->update();
	}
}

void Comparator::execute()
{
	MainWindow	*window = manager->getWindow();
	ConfigInfo	*info	= manager->getConfigInfo();

	QVector<Config *> configs;

	for( int i = 0; i < blocks.count(); ++i )
	{
		configs.append( blocks.at( i )->getConfig() );
	}

	for( int i = 0; i < info->getTestsCount(); ++i )
	{
		TestInfo *test = info->getTest( i );
		qDebug() << "Checking test: "	<< test->getName()
				 << "param count: "		<< test->getParamCount();

		LocalGroup	no_tests;
		no_tests.title	= "Test is absent";

		//----Checking included test in stored tests
		QString name_test = info->getTest( i )->getName();
		QString version = info->getTest( i )->getVersion();

		QVector<Config *>	valid_configs;
		for( int j = 0; j < configs.count(); ++j )
		{
			if( configs.at( j )->getItem( name_test, version ) )
			{
				valid_configs.append( configs.at( j ) );
			}
			else
			{
				no_tests.configs.append( configs.at( j ) );
			}
		}
		qDebug() << "\tTotal configs: " << configs.count();
		qDebug() << "\tValid configs: " << valid_configs.count();
		if( valid_configs.count() == 0 )
		{
			continue;
		}

		QString header_title = QString("Test: %1").arg( name_test );
		bool fTestTitleInserted = false;
		for( int j = 0; j < test->getParamCount(); ++j )
		{
			qDebug() << "\tChecking param: " << test->getParam( j )->getName();
			int param_indx = 1;
			LocalGroup no_params;
			no_params.title = "Param is absent";

			QString name_param = test->getParam( j )->getName();

			QVector<Param *> valid_params;
			for( int k = 0; k < valid_configs.count(); ++k )
			{
				Param *param = valid_configs.at( k )->getItem( name_test, version )->getParam( name_param );
				if( param )
				{
					valid_params.append( param );
				}
				else
				{
					no_params.configs.append( valid_configs.at( k ) );
					valid_configs.removeAt( k-- );
				}
			}
			qDebug() << "\tValid params: " << valid_params.count();

			if( valid_params.count() == 0 )
			{
				continue;
			}

			QString param_title = QString("Parameter: %1").arg( name_param );
			bool fParamTitleInserted = false;

			QVector<LocalGroup>	diff_params;
			int diff_param_index = 0;

			bool fPass = true;
			for( int k = 0; k < valid_params.count(); ++k )
			{
				LocalGroup start_element;
				start_element.params.append( valid_params.at( k ) );

				if( valid_params.at( k )->getType() != ParamType::Complex )
				{
					start_element.title = valid_params.at( k )->getData();
				}
				else
				{
					start_element.title = QString::number( param_indx++ );
				}

				diff_params.append( start_element );

				for( int l = (k + 1); l < valid_params.count(); ++l )
				{
					if( valid_params.at( k )->compare( valid_params.at( l ) ) )
					{
						diff_params[ diff_param_index ].params.append( valid_params.at( l ) );
						valid_params.removeAt( l-- );
					}
					else
					{
						fPass = false;
					}
				}

				if( valid_params.count() == 1 )
				{
					break; // leave after 1-turn cycle
				}

				++diff_param_index;
			}

			qDebug() << "\tdiff_params: " << diff_params.count();
			qDebug() << "\tno_tests:    " << no_tests.configs.count();
			qDebug() << "\tno_params:   " << no_params.configs.count();

			QVector<CompareGroup *> compare_groups;
			if( ( diff_params.count() > 1 ) ||
				( ( no_tests.configs.count() > 0 ) || ( no_params.configs.count() > 0 ) ) )
			{
				for( int k = 0; k < diff_params.count(); ++k )
				{
					CompareGroup	*group = new CompareGroup( diff_params.at( k ).params, window );
					compare_groups.append( group );
				}
				if( no_tests.configs.count() > 0 )
				{
					CompareGroup	*no_test_group	= new CompareGroup( no_tests.title,	no_tests.configs,	window );
					compare_groups.append( no_test_group );
				}
				if( no_params.configs.count() > 0 )
				{
					CompareGroup	*no_param_group	= new CompareGroup( no_params.title,	no_params.configs,	window );
					compare_groups.append( no_param_group );
				}

				if( !fTestTitleInserted )
				{
					window->addHLine();
					window->addHeader( header_title );
					fTestTitleInserted = true;
				}

				if( !fParamTitleInserted )
				{
					window->addHeader( param_title, 2 );
					fParamTitleInserted = true;
				}
				window->addCompareGroups( compare_groups );
			}
		}
	}
}

void Comparator::addBlock( bool )
{
	blocks.append( new CompareBlock( this ) );
}

void Comparator::removeBlock( bool )
{
	if( blocks.count() < 1 )
	{
		return;
	}

	delete blocks.at( blocks.count() - 1 );
	blocks.removeAt( blocks.count() - 1 );

	manager->getWindow()->resize( manager->getWindow()->width(),
								  manager->getWindow()->minimumHeight());
}

CompareBlock::CompareBlock(Comparator *comparator)
{
	this->comparator = comparator;
	label = new QLabel("Select compare parameter:");
	combobox = new QComboBox();
	addWidget(label);
	addWidget(combobox);
	comparator->outLayout->addLayout( this );

	update();
	int index = ( comparator->blocks.count() > 0 ) ? comparator->blocks.last()->combobox->currentIndex() : -1;
	if( ( index > -1 ) && ( (index < combobox->count()-1) ))
	{
		qDebug() << "Set next index";
		combobox->setCurrentIndex( index+1 );
	}
	else if( combobox->count() > 0 )
	{
		qDebug() << "Set zero index";
		combobox->setCurrentIndex( 0 );
	}
}

CompareBlock::~CompareBlock()
{
	Utils::clearLayout( this );
}

void CompareBlock::update()
{
	Manager * manager = this->comparator->manager;
	combobox->clear();

	for( int i = 0; i < manager->getConfigsCount(); ++i )
	{
		combobox->addItem( manager->getConfig( i )->getFullName() );
	}

	int self_index = comparator->blocks.indexOf( this );
	if( self_index < manager->getConfigsCount() )
	{
		combobox->setCurrentIndex( self_index );
	}
	else if( manager->getConfigsCount() > 0 )
	{
		combobox->setCurrentIndex( 0 );
	}
}

Config *CompareBlock::getConfig() const
{
	if( combobox->count() == 0 )
	{
		return nullptr;
	}

	return comparator->manager->getConfig( combobox->currentIndex() );
}
