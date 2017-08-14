#ifndef SPOILERGROUPBOX_H
#define SPOILERGROUPBOX_H

#include <QGroupBox>

class SpoilerGroupBox : public QGroupBox
{
	Q_OBJECT
public:
	SpoilerGroupBox( QWidget *parent );
	SpoilerGroupBox( const QString &title, QWidget *parent = 0 );

public slots:
	void toggled_spoiler(bool flag);

private:
	int reserv_h_max;
};

#endif // SPOILERGROUPBOX_H
