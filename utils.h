#ifndef UTILS_H
#define UTILS_H

#include <QDomElement>
#include <QList>
#include <QVariant>
#include <QVector>
#include <QLayout>

enum StringType
{
	Array,
	BracketsArray,
	BracketsMatrix,
	Matrix,
	Error,
	None
};

class Param;

class Utils
{
public:
	Utils();

	static	QDomElement getXmlElement(	const QDomNode & domNode,
										const QString& path,
										Qt::CaseSensitivity caseSensitivity );

	static	void	printNode(QDomNode node );
	static	QVector< QVector< QString > > parseString(QString str);
	static	QString saveToString( QVector<QVector<QString>> matrix, StringType string_type );
	static	QString saveToString( QVector<QString> matrix, StringType string_type );
	static	StringType getStringType(QString str);
	static	QString spacesToTabs(QString str);
	static	void	clearLayout( QLayout *layout );

private:
	static QVector< QString > StringToVector(QString str);
};

#endif // UTILS_H
