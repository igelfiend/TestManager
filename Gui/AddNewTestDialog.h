#ifndef ADDNEWTESTDIALOG_H
#define ADDNEWTESTDIALOG_H

#include <QDialog>

class QPushButton;
class QLineEdit;
class QTextEdit;
class QCheckBox;
class QLabel;

class AddNewTestDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AddNewTestDialog( QWidget *parent = 0 );

    QString getTemplateCode() const;
    QString getPrevTest() const;
    bool isInsertAtFirst() const;

private:
    QPushButton *btnOk;
    QPushButton *btnCancel;

    QLineEdit *editPrevTemplateName;
    QTextEdit *editTemplateText;
    QCheckBox *checkBoxInsertFirst;

    QString template_code;
    QString prevTest;
    bool fInsertAtFirst;
};

#endif // ADDNEWTESTDIALOG_H
