#include "editparamform.h"
#include "ui_editparamform.h"
#include "group.h"
#include "test_info.h"

#include <QTableWidget>
#include <QPlainTextEdit>
#include <QDebug>

EditParamForm::EditParamForm(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::EditParamForm)
{
	ui->setupUi(this);
	this->setWindowFlags(this->windowFlags() | Qt::WindowMinMaxButtonsHint);
}

EditParamForm::~EditParamForm()
{
	delete ui;
}

void EditParamForm::setText(const QString &text)
{
	QPlainTextEdit *edit = dynamic_cast<QPlainTextEdit *>( ui->verticalLayout_3->itemAt( 0 )->widget() );
	edit->setPlainText( text );
}

QString EditParamForm::getText() const
{
	QString result;

	switch( caller->getParamInfo()->getParamType() )
	{
	case Table:
		result = Utils::saveToString( getTableData(), data_type );
		return result;

	case Complex:
		return QString();

	default:
		QPlainTextEdit *edit = dynamic_cast<QPlainTextEdit *>( ui->verticalLayout_3->itemAt( 0 )->widget() );
		return edit->toPlainText();
	}
}

QBoxLayout *EditParamForm::getLayout() const
{
	return ui->verticalLayout_3;
}

Group *EditParamForm::getCaller() const
{
	return caller;
}

void EditParamForm::setCaller(Group *value)
{
	caller = value;
}

StringType EditParamForm::getDataType() const
{
	return data_type;
}

void EditParamForm::setDataType(const StringType &value)
{
	data_type = value;
}

void EditParamForm::addConnection(auto con)
{
	connections << con;
}

void EditParamForm::clearConnections()
{
	foreach (auto var, connections)
	{
		QObject::disconnect(var);
	}
}

QVector<QVector<QString> > EditParamForm::getTableData() const
{
	QVector< QVector <QString> > result;
	if( caller->getParamInfo()->getParamType() != ParamType::Table )
	{
		return result;
	}

	QTableWidget *table = dynamic_cast<QTableWidget *>( ui->verticalLayout_3->itemAt( 0 )->
														layout()->itemAt(0)->widget() );

	if( caller->getParamInfo()->getColumnNames().isEmpty() )
	{
		QVector<QString> row;
		for( int i = 0; i < table->rowCount(); ++i )
		{
			if( table->item( i, 0 ) )
			{
				row << table->item( i, 0 )->text();
			}
			else
			{
				row << "";
			}
		}
		result << row;
	}
	else
	{
		for( int i = 0; i < table->rowCount(); ++i )
		{
			QVector<QString> row;
			for( int j = 0; j < table->columnCount(); ++j)
			{
				if( table->item( i, j ) )
				{
					row << table->item( i, j )->text();
				}
				else
				{
					row << "";
				}
			}
			result.append( row );
		}
	}
	return result;
}

QList<QMetaObject::Connection> EditParamForm::getConnections() const
{
	return connections;
}
