#ifndef UTILS_H
#define UTILS_H

#include <QDomElement>
#include <QList>
#include <QVariant>
#include <QVector>
#include <QLayout>

#define FIELDS QVector< QPair< QDomNode, QTextEdit* > >

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
class Manager;
class SpoilerGroupBox;
class QGroupBox;
class QTextEdit;

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
	static QDomText TextInside( QDomNode node );
	static void		removeChilds(QDomNode &node );

	static	void	fixAccuracyTestRange(const QString &test_name, const QString &version, Manager *manager );

	static	void	addEquipToPerformance(Manager *manager );

    static  void    fixPowerAccuracyNaming( Manager *manager );

	static	bool	NodeIsFieldType( const QDomNode &node );
	static	SpoilerGroupBox	*NodeToGroupBox(const QDomNode &node, FIELDS &fields);

private:
	static void		insertNodeAndAddToList( QDomNode &target, QStringList &target_list, const QString &node_name, const QString &data );
	static void		insertNode( QDomNode &target, const QString &node_name, const QString &data );
	static QDomNode	createEquipItem(const QStringList &dev_list, QDomDocument &doc );
	static void		addElementsToList( QStringList &target_list, const QString &data );
	static QVector< QString > StringToVector(QString str);
};

#endif // UTILS_H

