#include "AddNewTestDialog.h"

#include <QPushButton>
#include <QLabel>
#include <QTextEdit>
#include <QLineEdit>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QDebug>

#include <QDialogButtonBox>

AddNewTestDialog::AddNewTestDialog(QWidget *parent):
    QDialog( parent )
{
    fInsertAtFirst = false;
    QVBoxLayout *mainLayout = new QVBoxLayout;

    editTemplateText = new QTextEdit( this );
    editTemplateText->setMinimumHeight( 300 );
    editTemplateText->setMinimumWidth( 400 );

    editPrevTemplateName = new QLineEdit( this );
    QLabel *lblInsertAfter = new QLabel( "Insert after(idd):", this );

    checkBoxInsertFirst = new QCheckBox( "Insert first" );

    QDialogButtonBox *btnBox = new QDialogButtonBox( QDialogButtonBox::Ok|QDialogButtonBox::Cancel, this );

    mainLayout->addWidget( editTemplateText );
    mainLayout->addWidget( lblInsertAfter );
    mainLayout->addWidget( editPrevTemplateName );
    mainLayout->addWidget( checkBoxInsertFirst );

    mainLayout->addWidget( btnBox );

    connect( btnBox, SIGNAL( accepted() ), this, SLOT( accept() ) );
    connect( btnBox, SIGNAL( rejected() ), this, SLOT( reject() ) );
    connect( checkBoxInsertFirst, &QCheckBox::stateChanged, [this](int state)
    {
        if( state == 0 )
        {
            editPrevTemplateName->setEnabled( true );
            fInsertAtFirst = false;
        }
        else
        {
            editPrevTemplateName->setEnabled( false );
            fInsertAtFirst = true;
        }
    } );

    connect( editTemplateText, &QTextEdit::textChanged, [this]()
    {
        template_code = editTemplateText->toPlainText();
    } );

    connect( editPrevTemplateName, &QLineEdit::textChanged, [this]()
    {
        prevTest = editPrevTemplateName->text();
    } );

    setLayout( mainLayout );
}

QString AddNewTestDialog::getTemplateCode() const
{
    return template_code;
}

QString AddNewTestDialog::getPrevTest() const
{
    return prevTest;
}

bool AddNewTestDialog::isInsertAtFirst() const
{
    return fInsertAtFirst;
}
