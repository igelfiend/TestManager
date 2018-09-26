#ifndef COLORREPLACERFORM_H
#define COLORREPLACERFORM_H

#include <QDialog>
#include <QMap>
#include <QString>
#include <QStringList>
#include <QTableWidget>
#include <QPair>

namespace Ui {
    class ColorReplacerForm;
}

class colorRow
{
public:
    colorRow( const QString &src, const QString &tgt ):
        sourceColor( src ),
        targetColor( tgt ){}
    QString sourceColor;
    QString targetColor;
};

class ColorReplacementTable: public QList<colorRow>
{
public:
    QMap<QString, QString> toMap() const;
};

class ColorReplacerForm : public QDialog
{
    Q_OBJECT

public:
    explicit ColorReplacerForm(QWidget *parent = nullptr);
    ~ColorReplacerForm();

    void setProcessingDevices( const QStringList &devices );

    void setDataToTable( const ColorReplacementTable &data );
    ColorReplacementTable getDataFromTable() const;
    bool isSelfReplaced() const;

public slots:
    void saveColorReplacementTable();
    void loadColorReplacementTable();

    void tableAppendRowToTail();
    void tableRemoveRowFromTail();

private:
    ColorReplacementTable loadColorReplaceDataFromFile( const QString &filename );
    bool saveColorReplaceDataToFile( const ColorReplacementTable &data, const QString &filepath );

    Ui::ColorReplacerForm *ui;

    QStringList _devices;
};

#endif // COLORREPLACERFORM_H
