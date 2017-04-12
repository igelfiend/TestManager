#include "comparator.h"
#include "config.h"
#include "utils.h"
#include "mainwindow.h"

#include <QDockWidget>

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
}
