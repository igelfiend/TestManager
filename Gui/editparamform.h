#ifndef EDITPARAMFORM_H
#define EDITPARAMFORM_H

#include <QDialog>
#include <QDialogButtonBox>
#include <QBoxLayout>
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

	void	setText( QString text );
	QString	getText() const;

	QBoxLayout *getLayout() const;

	Group	*getCaller() const;
	void	setCaller(Group *value);

	StringType getDataType() const;
	void	setDataType(const StringType &value);


private:
	QVector< QVector< QString > > getTableData() const;

	Ui::EditParamForm *ui;
	Group * caller;
	StringType data_type;
};

#endif // EDITPARAMFORM_H
