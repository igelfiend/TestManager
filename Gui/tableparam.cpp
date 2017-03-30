#include "tableparam.h"
#include <QDebug>

TableParam::TableParam():
	QTableWidget()
{

}

TableParam::TableParam(int rows, int cols, QWidget *parent):
	QTableWidget( rows, cols, parent )
{

}

void TableParam::uploadData(QVector<QVector<QString> > data, StringType type)
{
	if( data.count() == 0 )
	{
		return;
	}

	if( ( type == StringType::Array ) ||
		( type == StringType::BracketsArray ) )
	{
		for( int i = 0; i < data.at( 0 ).count(); ++i )
		{
			QTableWidgetItem *item = new QTableWidgetItem( data[ 0 ][ i ] );
			this->setItem( i, 0, item );
		}
	}
	else
	{
		for( int i = 0; i < data.count(); ++i )
		{
			for( int j = 0; j < data.at( i ).count(); ++j )
			{
				QTableWidgetItem *item = new QTableWidgetItem( data[ i ][ j ] );
				this->setItem( i, j, item );
			}
		}
	}
}

void TableParam::correctSize( int height )
{
	this->resizeRowsToContents();
	this->resizeColumnsToContents();
	this->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

	int header_length = 0;
	for( int i = 0; i < this->columnCount(); ++i )
	{
		header_length += this->columnWidth( i );
	}
	header_length += 20;
	this->setMinimumSize( header_length, height );
}

void TableParam::insertRowWhereSelected(bool)
{
	int row = ( this->currentRow() == -1 ) ? (this->rowCount()) : (this->currentRow()+1);
	emit( this->insertRow( row ) );
}

void TableParam::removeRowWhereSelected(bool)
{
	int row = ( this->currentRow() == -1 ) ? (this->rowCount()-1) : (this->currentRow());
	emit( this->removeRow( row ) );
}

StringType TableParam::getType() const
{
	return type;
}

