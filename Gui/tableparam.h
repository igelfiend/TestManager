#ifndef TABLEPARAM_H
#define TABLEPARAM_H

#include <QTableWidget>
#include <QVector>
#include "utils.h"

class TableParam : public QTableWidget
{
	Q_OBJECT
public:
	TableParam();
	TableParam( int rows, int cols, QWidget *parent );
	void	uploadData( QVector< QVector< QString> > data, StringType type );
	void	correctSize(int height );

	StringType getType() const;

public slots:
	void	insertRowWhereSelected( bool );
	void	removeRowWhereSelected( bool );

private:
	StringType	type;
};

#endif // TABLEPARAM_H
