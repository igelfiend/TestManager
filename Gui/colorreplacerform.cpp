#include "colorreplacerform.h"
#include "ui_colorreplacerform.h"

#include <QTableWidgetItem>
#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>
#include <QDomDocument>
#include <QFile>
#include <QTextStream>
#include <QTextCodec>

ColorReplacerForm::ColorReplacerForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ColorReplacerForm)
{
    ui->setupUi(this);

    ui->tableWidgetColorReplacement->setHorizontalHeaderItem( 0, new QTableWidgetItem( "Source color" ) );
    ui->tableWidgetColorReplacement->setHorizontalHeaderItem( 1, new QTableWidgetItem( "Target color" ) );

    tableAppendRowToTail();

    connect( ui->pushButtonAddRow, &QPushButton::clicked,
             this, &ColorReplacerForm::tableAppendRowToTail );

    connect( ui->pushButtonRemoveRow, &QPushButton::clicked,
             this, &ColorReplacerForm::tableRemoveRowFromTail );

    connect( ui->pushButtonSaveReplacementTable, &QPushButton::clicked,
             this, &ColorReplacerForm::saveColorReplacementTable );

    connect( ui->pushButtonLoadReplacementTable, &QPushButton::clicked,
             this, &ColorReplacerForm::loadColorReplacementTable );
}

ColorReplacerForm::~ColorReplacerForm()
{
    delete ui;
}

void ColorReplacerForm::setProcessingDevices(const QStringList &devices)
{
    _devices = devices;
    ui->listWidgetColorReplacingDevices->addItems( devices );
}

bool ColorReplacerForm::saveColorReplaceDataToFile(const ColorReplacementTable &data, const QString &filepath)
{
    QDomDocument d;
    QDomElement colorsNode = d.createElement( "colors" );
    d.appendChild( colorsNode );

    for( int i = 0; i < data.length(); ++i )
    {
        QDomElement colorNode = d.createElement( "color" );
        colorNode.setAttribute( "source", data[ i ].sourceColor );
        colorNode.setAttribute( "target", data[ i ].targetColor );

        colorsNode.appendChild( colorNode );
    }

    QFile file( filepath );
    if( !file.open( QIODevice::WriteOnly ) )
    {
        qCritical() << "ColorReplacerForm::saveColorReplaceDataToFile: error writing file.";
        return false;
    }

    QTextStream str( &file );
    str.setCodec( QTextCodec::codecForName( "UTF-8" ) );
    d.save( str, 4 );
    file.close();

    qInfo() << "File saved successfully!";
    return true;
}

void ColorReplacerForm::tableAppendRowToTail()
{
    int rowCount = ui->tableWidgetColorReplacement->rowCount();
    ui->tableWidgetColorReplacement->insertRow( rowCount );
    ui->tableWidgetColorReplacement->setItem( rowCount, 0, new QTableWidgetItem() );
    ui->tableWidgetColorReplacement->setItem( rowCount, 1, new QTableWidgetItem() );
}

void ColorReplacerForm::tableRemoveRowFromTail()
{
    int rowCount = ui->tableWidgetColorReplacement->rowCount();
    if( rowCount > 0 )
    {
        ui->tableWidgetColorReplacement->removeRow( rowCount-1 );
    }
}

ColorReplacementTable ColorReplacerForm::loadColorReplaceDataFromFile(const QString &filename)
{
    ColorReplacementTable result;
    QFile file( filename );

    if( !file.open( QIODevice::ReadOnly ) )
    {
        qWarning() << "ColorReplacerForm::loadColorReplaceDataFromFile: Error opening file: " << filename;
        return result;
    }

    QDomDocument d;
    int errLine;
    int errColumn;
    QString errString;

    if( !d.setContent( &file, true, &errString, &errLine, &errColumn ) )
    {
        QMessageBox mbox;
        mbox.setText( QString( "Error: %1 \nrow: %2 \ncolumn: %3" )
                                                  .arg( errString )
                                                  .arg( errLine )
                                                  .arg( errColumn ) );
        mbox.exec();
        return result;
    }

    file.close();

    QDomElement root = d.documentElement();

    QDomElement colorNode = root.firstChildElement( "color" );
    while( !colorNode.isNull() )
    {
        QString sourceColor = colorNode.attribute( "source" );
        QString targetColor = colorNode.attribute( "target" );

        result.append( colorRow( sourceColor, targetColor ) );

        qDebug() << "loaded: " << sourceColor << targetColor;
        colorNode = colorNode.nextSiblingElement( "color" );
    }

    return result;
}

void ColorReplacerForm::setDataToTable(const ColorReplacementTable &data)
{
    ui->tableWidgetColorReplacement->setRowCount( 0 );
    ui->tableWidgetColorReplacement->clearContents();
    for( int i = 0; i < data.length(); ++i )
    {
        QString sourceColor = data[ i ].sourceColor;
        QString targetColor = data[ i ].targetColor;

        ui->tableWidgetColorReplacement->insertRow( i );
        ui->tableWidgetColorReplacement->setItem( i, 0, new QTableWidgetItem( sourceColor ) );
        ui->tableWidgetColorReplacement->setItem( i, 1, new QTableWidgetItem( targetColor ) );
    }
}

ColorReplacementTable ColorReplacerForm::getDataFromTable() const
{
    ColorReplacementTable result;

    int rowCount = ui->tableWidgetColorReplacement->rowCount();
    for( int i = 0; i < rowCount; ++i )
    {
        QString sourceColor = ui->tableWidgetColorReplacement->item( i, 0 )->data( 0 ).toString();
        QString targetColor = ui->tableWidgetColorReplacement->item( i, 1 )->data( 0 ).toString();

        if( sourceColor.isEmpty() )
        {
            qWarning() << "ColorReplacerForm::getDataFromTable: source is empty.";
            continue;
        }
        result.append( colorRow( sourceColor, targetColor ) );
    }

    return result;
}

bool ColorReplacerForm::isSelfReplaced() const
{
    return ui->checkBoxReplacSourceFile->isChecked();
}

void ColorReplacerForm::saveColorReplacementTable()
{
    QString path = QFileDialog::getSaveFileName(this, "Saving color replacement data", qApp->applicationDirPath(), "Color tables (*.clt)" );

    if( path.isEmpty() )
    {
        QMessageBox mbox;
        mbox.setText( "Error while saving color table!" );
        mbox.exec();
    }

    ColorReplacementTable colorTable = getDataFromTable();

    saveColorReplaceDataToFile( colorTable, path );
}

void ColorReplacerForm::loadColorReplacementTable()
{
    QString path = QFileDialog::getOpenFileName(this, "Opening color replacement data", qApp->applicationDirPath(), "Color tables (*.clt)" );

    if( path.isEmpty() )
    {
        return;
    }

    ColorReplacementTable data = loadColorReplaceDataFromFile( path );
    if( !data.isEmpty() )
    {
        setDataToTable( data );
    }
}

QMap<QString, QString> ColorReplacementTable::toMap() const
{
    QMap<QString, QString> result;

    for( int i = 0; i < length(); ++i )
    {
        result.insert( this->at( i ).sourceColor, this->at( i ).targetColor );
    }

    return result;
}
