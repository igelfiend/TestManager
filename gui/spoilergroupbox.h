#ifndef SPOILERGROUPBOX_H
#define SPOILERGROUPBOX_H

#include <QGroupBox>

class SpoilerGroupBox : public QGroupBox
{
	Q_OBJECT
public:
    explicit SpoilerGroupBox( QWidget *parent );
    SpoilerGroupBox( const QString &title, QWidget *parent = nullptr );

public slots:
	void toggled_spoiler(bool flag);

private:
	int reserv_h_max;
};

#endif // SPOILERGROUPBOX_H
