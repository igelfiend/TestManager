#include "spoilergroupbox.h"
#include <QDebug>

SpoilerGroupBox::SpoilerGroupBox(QWidget *parent):
	QGroupBox( parent )
{
	setCheckable( true );

	reserv_h_max = maximumHeight();

	setSizePolicy( QSizePolicy::Expanding, QSizePolicy::MinimumExpanding );

	setStyleSheet( "QGroupBox "
					 "{"
						"border: 1px solid gray;"
						"border-radius: 3px;"
						"padding: 10px;	"
					 "}"
					"QGroupBox::indicator"
					"{"
						"width: 0px;"
						"height: 0px;"
					"}" );

	connect( this, SIGNAL( toggled(bool) ), this, SLOT( toggled_spoiler(bool) )  );
}

SpoilerGroupBox::SpoilerGroupBox(const QString &title, QWidget *parent):
	SpoilerGroupBox( parent )
{
	setTitle( title );
}

void SpoilerGroupBox::toggled_spoiler( bool flag )
{
	if( !flag )
	{
		reserv_h_max = maximumHeight();
		setMaximumHeight( 14 );
	}
	else
	{
		setMaximumHeight( reserv_h_max );
	}
}
