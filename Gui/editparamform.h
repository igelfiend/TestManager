#ifndef EDITPARAMFORM_H
#define EDITPARAMFORM_H

#include <QDialog>
#include <QDialogButtonBox>
#include <QBoxLayout>
#include <QObject>
#include "utils.h"

class Group;

namespace Ui {
	class EditParamForm;
	}

class EditParamForm : public QDialog
{
	Q_OBJECT

public:
	explicit EditParamForm(QWidget *parent = 0);
	~EditParamForm();

    void	setText( const QString &text );
	QString	getText() const;

	QBoxLayout *getLayout() const;

	Group	*getCaller() const;
	void	setCaller(Group *value);

	StringType getDataType() const;
	void	setDataType(const StringType &value);

	void	addConnection(auto con);
	void	clearConnections();


	QList<QMetaObject::Connection> getConnections() const;

private:
	QVector< QVector< QString > > getTableData() const;

	StringType	data_type;
	Group		*caller;
	Ui::EditParamForm	*ui;
	QList<QMetaObject::Connection>	connections;
};

#endif // EDITPARAMFORM_H
