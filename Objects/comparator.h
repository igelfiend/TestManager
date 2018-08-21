#ifndef COMPARATOR_H
#define COMPARATOR_H

#include <QObject>
#include <QLayout>
#include <QLabel>
#include <QComboBox>

#include "manager.h"


class CompareBlock;

class Comparator: public QObject
{
	Q_OBJECT
	friend class CompareBlock;
public:
    Comparator(){}
	Comparator(Manager *manager, QVBoxLayout *outLayout );
    ~Comparator(){}

	void	update();
	void	execute();

public slots:
	void	addBlock(bool);
	void	removeBlock(bool);

private:
	Manager	*manager;
	QVBoxLayout	*outLayout;
	QVector<CompareBlock *> blocks;
};


class CompareBlock: private QHBoxLayout
{
	Q_OBJECT
public:
	CompareBlock();
    explicit CompareBlock( Comparator *comparator );
	~CompareBlock();

	void	update();
	Config	*getConfig() const;
private:
	Comparator	*comparator;
	QComboBox	*combobox;
	QLabel		*label;
};

#endif // COMPARATOR_H
